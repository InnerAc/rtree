#include "XMLEncoder.h"

XMLEncoder::XMLEncoder()
{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	m_pRootNode = nullptr;
	TagName_Text = new char[4];
	memcpy(TagName_Text, "text", 4);
	TagName_DTD = new char[3];
	memcpy(TagName_DTD, "DTD", 3);

	sizeScale = 0.005f;
}

XMLEncoder::~XMLEncoder()
{
	//mapFile.CloseMapFile();
	UnLoad();
	delete TagName_Text;
	delete TagName_DTD;
}

XMLNode::XMLNode()
{
	m_nodeType = notDef;
	m_pStart = nullptr;
	m_pEnd = nullptr;
	m_pValidEnd = nullptr;
	m_childLen = 0;
	m_totalChildLen = 0;
	m_pTagNameStart = nullptr;
	m_pTagNameEnd = nullptr;
	m_pIDStart = nullptr;
	m_pIDEnd = nullptr;
}

XMLNode::~XMLNode()
{

}

HRESULT XMLEncoder::Load(char *filePath)
{
	UnLoad();
	HRESULT hr = mapFile.OpenMapFile(filePath);//打开内存映射
	if(FAILED(hr))	return E_FAIL;
	m_pRootNode = new XMLNode;
	m_pRootNode->m_nodeType = root;
	m_pvFile = mapFile.m_pvFile;//获取文件指针
	EncodeFile((char*)m_pvFile);
	return S_OK;
}

/*
每当出现一个开始标志，为该元素新建一个结点，赋值m_pStart位置，并压入父结点栈，下一个新元素就以该栈的顶点结点作为父节点。
每当出现一个结束标志，去父结点栈的栈顶，赋值m_pEnd位置，并使该结点出栈
*/
void XMLEncoder::EncodeFile(char* pFile)
{
#ifndef _DEBUG
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//设置实时优先级
#endif // _DEBUG
	char *pCh = pFile;
	m_pRootNode->m_pStart = pCh;
	stack<XMLNode*> parentStack;//父节点的栈
	stack<XMLNode*> validStack;//有效结束位的栈
	stack<int> totalChildStack;//子节点总量的栈
	XMLNode	*parent,*valid;
	DWORD groupIndex;
	parentStack.push(m_pRootNode);
	validStack.push(m_pRootNode);
	totalChildStack.push(0);
	NodeGoupSize = (DWORD)(mapFile.m_fileSize*sizeScale) + 1;
	cout<<NodeGoupSize<<endl;
	NodeGroup = new XMLNode[NodeGoupSize];
	NodeGroupStack.push(NodeGroup);
	groupIndex = 0;
	while (*pCh != '\0')
	{
		if(*pCh == '<')
		{
			if(*(++pCh) == '/')//'</'作为结束标志
			{
				if(!parentStack.empty())
				{
					parent = parentStack.top();
					while (*(++pCh) !='>');//一直位移到'>'处
					parent->m_pEnd = pCh;
					parentStack.pop();//该节点已经结束，则弹出父节点栈
				}

				if (!totalChildStack.empty())
				{
					long temp = totalChildStack.top();
					totalChildStack.pop();
					totalChildStack.top() += temp;
					parentStack.top()->m_totalChildLen = totalChildStack.top();
				}

			}
			else if((*pCh>='0'&&*pCh<='9') || (*pCh>='A' && *pCh<='z') || (*pCh == ' '))//'<'作为起始标志
			{
				//XMLNode* node = new XMLNode;
				XMLNode* node;
				if(groupIndex < NodeGoupSize)
					node = NodeGroup+(groupIndex++);
				else
				{
					NodeGoupSize = (DWORD)(NodeGoupSize*sizeScale) + 1;
					NodeGroup = new XMLNode[NodeGoupSize];
					NodeGroupStack.push(NodeGroup);
					groupIndex = 0;
					node = NodeGroup+(groupIndex++);
				}
				node->m_pStart = pCh-1;
				node->m_nodeType = element;
				if(!parentStack.empty())
				{
					parent = parentStack.top();
					parent->m_childNode.push_back(node);
					parent->m_childLen++;
				}
				parentStack.push(node);//新节点入栈
				validStack.push(node);
				totalChildStack.top()++;//总自结点累计栈操作
				totalChildStack.push(0);
				SkipBlankspace(&pCh);//跳过空格等空白字符
				node->m_pTagNameStart = pCh;
				while ((*pCh>='0'&&*pCh<='9') || (*pCh>='A' && *pCh<='z'))
				{
					pCh++;
				}
				pCh--;
				node->m_pTagNameEnd = pCh;
			}
			else if(*pCh=='!')//解析<!起始符号
			{
				if(*(pCh+1)=='-'&&*(pCh+2)=='-')//解析注释 <!-- -->
				{
					while (*pCh != '>')//粗略的判断注释结尾为'>'
					{
						pCh++;
					}
				}
				else//解析DTD:不解析DTD内容，仅将DTD结点（即使DTD包含子节点）作为一个整体
				{
					//XMLNode* node = new XMLNode;
					XMLNode* node;
					if(groupIndex < NodeGoupSize)
						node = NodeGroup+(groupIndex++);
					else
					{
						NodeGoupSize = (DWORD)(NodeGoupSize*sizeScale) + 1;
						NodeGroup = new XMLNode[NodeGoupSize];
						NodeGroupStack.push(NodeGroup);
						groupIndex = 0;
						node = NodeGroup+(groupIndex++);
					}
					m_pDTDNode = node;//存储DTD结点
					int t = 0;//用来存储<>对数，当t<0时，证明多出一个>,即DTD结束
					node->m_pStart = pCh-1;
					node->m_nodeType = DTD;
					node->m_pTagNameStart = TagName_DTD;
					node->m_pTagNameEnd = TagName_DTD + 2;
					if(!parentStack.empty())
					{
						parent = parentStack.top();
						parent->m_childNode.push_back(node);
						parent->m_childLen++;
					}

					totalChildStack.top()++;//总自结点累计栈操作
					pCh++;
					while (t>=0)
					{
						if(*pCh == '<')
							t++;
						else if(*pCh == '>')
							t--;
						pCh++;
					}
					node->m_pEnd = pCh-1;
					node->m_pValidEnd = pCh-1;
				}
			}
		}
		else if(*pCh == '/')
		{
			if(*(++pCh) == '>' && !parentStack.empty())//'/>'作为结束标志
			{
				if(!parentStack.empty())
				{
					parent = parentStack.top();
					parent->m_pEnd = pCh;
					parentStack.pop();
				}

				if (!validStack.empty())
				{
					valid = validStack.top();
					valid->m_pValidEnd = pCh;
					validStack.pop();
				}

				if (!totalChildStack.empty())
				{
					long temp = totalChildStack.top();
					totalChildStack.pop();
					totalChildStack.top() += temp;
					parentStack.top()->m_totalChildLen = totalChildStack.top();
				}

			}
		}
		else if(*pCh == '>')
		{
			if (!validStack.empty())
			{
				valid = validStack.top();
				valid->m_pValidEnd = pCh;
				validStack.pop();
			}

			//while (*(++pCh) == ' ' || *pCh == '\r' ||  *pCh == '\n' || *pCh == '\t');
			pCh++;
			SkipBlankspace(&pCh);//当结束标记之后是文本时,先去掉空格之类的控制符
			if(*pCh == '\0')	break;
			if(*pCh != '<' && *pCh != '/' && *pCh != '>')
			{
				//XMLNode* node = new XMLNode;//添加text结点
				XMLNode* node;
				if(groupIndex < NodeGoupSize)
					node = NodeGroup+(groupIndex++);
				else
				{
					NodeGoupSize = (DWORD)(NodeGoupSize*sizeScale) + 1;
					NodeGroup = new XMLNode[NodeGoupSize];
					NodeGroupStack.push(NodeGroup);
					groupIndex = 0;
					node = NodeGroup+(groupIndex++);
				}

				node->m_nodeType = text;
				node->m_pStart = pCh;
				node->m_pTagNameStart = TagName_Text;
				node->m_pTagNameEnd = TagName_Text+3;
				if(!parentStack.empty())
				{
					parent = parentStack.top();
					parent->m_childNode.push_back(node);
					parent->m_childLen++;
					parent->m_totalChildLen++;
				}

				totalChildStack.top()++;

				while (*(++pCh)!='<'){if(*pCh=='\0') break;}//文本结束的判定条件
				pCh--;
				node->m_pEnd = pCh;
				node->m_pValidEnd = pCh;
			}
			else
				pCh--;
		}
		else if(*pCh=='i'&&*(pCh+1)=='d'&&*(pCh+2)=='=')//id
		{
			pCh+=4;
			parent = parentStack.top();
			parent->m_pIDStart = pCh;
			while (*pCh != '\"')
			{
				pCh++;
			}
			parent->m_pIDEnd = pCh-1;
		}
		pCh++;
	}
	m_pRootNode->m_pEnd = pCh;
	m_pRootNode->m_pValidEnd = pCh;
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);//恢复标准优先级
}

void XMLEncoder::SkipBlankspace(char **pFile)
{
	while (((**pFile)==' '||(**pFile)=='\t'||(**pFile)=='\n'||(**pFile)=='\r') && (**pFile) != '\0')
	{
		(*pFile)++;
	} 

}

void XMLEncoder::UnLoad()
{
#ifndef _DEBUG
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//设置实时优先级
#endif // _DEBUG
	if(m_pRootNode == nullptr)
		return;
	mapFile.CloseMapFile();
	while (!NodeGroupStack.empty())
	{
		delete []NodeGroupStack.top();
		NodeGroupStack.pop();
	}
	delete m_pRootNode;
	m_pRootNode = nullptr;
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);//恢复标准优先级
}
