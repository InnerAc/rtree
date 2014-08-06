/*
*�汾��V1.2	(2014.6.5)
*���ߣ���ʤ��
*���ܣ�ʵ�ֳ�����xml����
*�������:
		1����Ӷ�tagName�Ľ���
		2���Ż��ٶ�
		3����Ӷ�id�Ľ���
*
*/
#ifndef XMLENCODER_H
#define XMLENCODER_H
//����ڴ�й¶
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

	HRESULT		Load(char *filePath);//��ȡXML�ļ�
	void		UnLoad();//ж���ļ�
	static	void		SkipBlankspace(char **pFile);//�����������Ŀո�

	void		*m_pvFile;//�ļ�ָ��
	XMLNode		*m_pRootNode;//�����
	XMLNode		*m_pDTDNode;//DTD���	
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
	char *m_pTagNameStart;//��ǩ����
	char *m_pTagNameEnd;
	char *m_pIDStart;//ID����
	char *m_pIDEnd;
	char *m_pStart;//��Ϣ������ļ�ָ�룬ָ������ʼ��
	char *m_pValidEnd;////��Ϣ������ļ�ָ�룬ָ������������Ľ�������������<></>�������ͣ�����������<>����Ч��Ϣ����һ����
	char *m_pEnd;//��Ϣ������ļ�ָ�룬ָ�����������
	unsigned long m_childLen;//�����ӽڵ����
	unsigned long m_totalChildLen;//�����ýڵ��������еĽ�����
	XMLNodeList m_childNode;
};



#endif
