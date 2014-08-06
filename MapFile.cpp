#include "MapFile.h"

CMapFile::CMapFile()
{
	m_pvFile = nullptr;
	m_bSmooth= false;
	m_fileSize = -1;
}

CMapFile::~CMapFile()
{
	CloseMapFile();
}

HRESULT CMapFile::OpenMapFile(char *filePath)
{
	if(m_pvFile != nullptr)
	{
		UnmapViewOfFile(m_pvFile);
		m_pvFile = nullptr;
	}
	//���ļ�
	HANDLE hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)//�򿪴���
	{
		if(!m_bSmooth)//��������ʱ
		{
			TCHAR str[256];
			wsprintf(str, "�޷����ļ�[%s],�ļ����ܲ�����!", filePath);
			MessageBox(NULL, str, "FileMap����", MB_OK);
		}
		return E_FAIL;
	}
	//�����ļ�ӳ�����
	DWORD dwFileSize = GetFileSize(hFile,NULL);//=�ļ�����+1('\0')
	m_fileSize = dwFileSize;
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if(hFileMap == NULL)
	{
		if(!m_bSmooth)
		{
			TCHAR str[256];
			wsprintf(str, "�޷����ļ�[%s],�ļ�����Ϊ��!", filePath);
			MessageBox(NULL, str, "FileMap����", MB_OK);
		}
		return E_FAIL;
	}
	//���ļ�����ӳ�䵽��ַ�ռ�
	m_pvFile = MapViewOfFile(hFileMap, FILE_MAP_READ, 0,0,0);
	if(m_pvFile == NULL)
	{	
		if(!m_bSmooth)
		{
			TCHAR str[256];
			wsprintf(str, "�޷�ӳ���ļ�[%s]! %d", filePath, GetLastError());
			MessageBox(NULL, str, "FileMap����", MB_OK);
		}
		return E_FAIL;
	}
	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CMapFile::CloseMapFile()
{
	if(m_pvFile != nullptr)
	{
		if(UnmapViewOfFile(m_pvFile))
		{
			m_pvFile = nullptr;
			CloseHandle(hFileMap);
			return S_OK;
		}
		return E_FAIL;
	}
	return S_OK;
}