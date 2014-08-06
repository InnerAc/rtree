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
	HRESULT hr = mapFile.OpenMapFile(filePath);//���ڴ�ӳ��
	if(FAILED(hr))	return E_FAIL;
	m_pRootNode = new XMLNode;
	m_pRootNode->m_nodeType = root;
	m_pvFile = mapFile.m_pvFile;//��ȡ�ļ�ָ��
	EncodeFile((char*)m_pvFile);
	return S_OK;
}

/*
ÿ������һ����ʼ��־��Ϊ��Ԫ���½�һ����㣬��ֵm_pStartλ�ã���ѹ�븸���ջ����һ����Ԫ�ؾ��Ը�ջ�Ķ�������Ϊ���ڵ㡣
ÿ������һ��������־��ȥ�����ջ��ջ������ֵm_pEndλ�ã���ʹ�ý���ջ
*/
void XMLEncoder::EncodeFile(char* pFile)
{
#ifndef _DEBUG
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//����ʵʱ���ȼ�
#endif // _DEBUG
	char *pCh = pFile;
	m_pRootNode->m_pStart = pCh;
	stack<XMLNode*> parentStack;//���ڵ��ջ
	stack<XMLNode*> validStack;//��Ч����λ��ջ
	stack<int> totalChildStack;//�ӽڵ�������ջ
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
			if(*(++pCh) == '/')//'</'��Ϊ������־
			{
				if(!parentStack.empty())
				{
					parent = parentStack.top();
					while (*(++pCh) !='>');//һֱλ�Ƶ�'>'��
					parent->m_pEnd = pCh;
					parentStack.pop();//�ýڵ��Ѿ��������򵯳����ڵ�ջ
				}

				if (!totalChildStack.empty())
				{
					long temp = totalChildStack.top();
					totalChildStack.pop();
					totalChildStack.top() += temp;
					parentStack.top()->m_totalChildLen = totalChildStack.top();
				}

			}
			else if((*pCh>='0'&&*pCh<='9') || (*pCh>='A' && *pCh<='z') || (*pCh == ' '))//'<'��Ϊ��ʼ��־
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
				parentStack.push(node);//�½ڵ���ջ
				validStack.push(node);
				totalChildStack.top()++;//���Խ���ۼ�ջ����
				totalChildStack.push(0);
				SkipBlankspace(&pCh);//�����ո�ȿհ��ַ�
				node->m_pTagNameStart = pCh;
				while ((*pCh>='0'&&*pCh<='9') || (*pCh>='A' && *pCh<='z'))
				{
					pCh++;
				}
				pCh--;
				node->m_pTagNameEnd = pCh;
			}
			else if(*pCh=='!')//����<!��ʼ����
			{
				if(*(pCh+1)=='-'&&*(pCh+2)=='-')//����ע�� <!-- -->
				{
					while (*pCh != '>')//���Ե��ж�ע�ͽ�βΪ'>'
					{
						pCh++;
					}
				}
				else//����DTD:������DTD���ݣ�����DTD��㣨��ʹDTD�����ӽڵ㣩��Ϊһ������
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
					m_pDTDNode = node;//�洢DTD���
					int t = 0;//�����洢<>��������t<0ʱ��֤�����һ��>,��DTD����
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

					totalChildStack.top()++;//���Խ���ۼ�ջ����
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
			if(*(++pCh) == '>' && !parentStack.empty())//'/>'��Ϊ������־
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
			SkipBlankspace(&pCh);//���������֮�����ı�ʱ,��ȥ���ո�֮��Ŀ��Ʒ�
			if(*pCh == '\0')	break;
			if(*pCh != '<' && *pCh != '/' && *pCh != '>')
			{
				//XMLNode* node = new XMLNode;//���text���
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

				while (*(++pCh)!='<'){if(*pCh=='\0') break;}//�ı��������ж�����
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
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);//�ָ���׼���ȼ�
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
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//����ʵʱ���ȼ�
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
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);//�ָ���׼���ȼ�
}
