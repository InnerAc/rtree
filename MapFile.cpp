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
	//打开文件
	HANDLE hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)//打开错误
	{
		if(!m_bSmooth)//当不阻塞时
		{
			TCHAR str[256];
			wsprintf(str, "无法打开文件[%s],文件可能不存在!", filePath);
			MessageBox(NULL, str, "FileMap错误", MB_OK);
		}
		return E_FAIL;
	}
	//创建文件映射对象
	DWORD dwFileSize = GetFileSize(hFile,NULL);//=文件长度+1('\0')
	m_fileSize = dwFileSize;
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if(hFileMap == NULL)
	{
		if(!m_bSmooth)
		{
			TCHAR str[256];
			wsprintf(str, "无法打开文件[%s],文件可能为空!", filePath);
			MessageBox(NULL, str, "FileMap错误", MB_OK);
		}
		return E_FAIL;
	}
	//将文件数据映射到地址空间
	m_pvFile = MapViewOfFile(hFileMap, FILE_MAP_READ, 0,0,0);
	if(m_pvFile == NULL)
	{	
		if(!m_bSmooth)
		{
			TCHAR str[256];
			wsprintf(str, "无法映射文件[%s]! %d", filePath, GetLastError());
			MessageBox(NULL, str, "FileMap错误", MB_OK);
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