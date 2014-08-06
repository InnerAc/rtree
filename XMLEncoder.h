/*
*版本：V1.2	(2014.6.5)
*作者：岳胜涛
*功能：实现初步的xml解析
*具体更改:
		1、添加对tagName的解析
		2、优化速度
		3、添加对id的解析
*
*/
#ifndef XMLENCODER_H
#define XMLENCODER_H
//检测内存泄露
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#include "MapFile.h"
#include <list>
#include <stack>
#include <iostream>
#include <fstream>

using namespace std;

class XMLEncoder;
class XMLNode;

typedef list<XMLNode*> XMLNodeList;

class XMLEncoder
{
public:
	XMLEncoder();
	~XMLEncoder();

	HRESULT		Load(char *filePath);//读取XML文件
	void		UnLoad();//卸载文件
	static	void		SkipBlankspace(char **pFile);//跳过接下来的空格

	void		*m_pvFile;//文件指针
	XMLNode		*m_pRootNode;//根结点
	XMLNode		*m_pDTDNode;//DTD结点	
private:
	CMapFile	mapFile;
	void		EncodeFile(char* pFile);

	char		*TagName_Text;
	char		*TagName_DTD;
	XMLNode		*NodeGroup;
	DWORD		NodeGoupSize;
	float		sizeScale;
	stack<XMLNode*>		NodeGroupStack;

};

enum NodeType
{
	notDef,
	root,
	element,
	text,
	DTD

};

class XMLNode
{
public:
	XMLNode();
	~XMLNode();

	NodeType m_nodeType;
	char *m_pTagNameStart;//标签名称
	char *m_pTagNameEnd;
	char *m_pIDStart;//ID名称
	char *m_pIDEnd;
	char *m_pStart;//信息区域的文件指针，指向区域开始处
	char *m_pValidEnd;////信息区域的文件指针，指向区域有意义的结束处，即对于<></>这种类型，仅仅包含在<>中有效信息的这一区域
	char *m_pEnd;//信息区域的文件指针，指向区域结束处
	unsigned long m_childLen;//包含子节点个数
	unsigned long m_totalChildLen;//包含该节点往下所有的结点个数
	XMLNodeList m_childNode;
};



#endif
