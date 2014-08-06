#ifndef MAPFILE_H
#define MAPFILE_H

#include <string>
#include <Windows.h>
#include <tchar.h>
class CMapFile
{
public:
	CMapFile();
	~CMapFile();
	
	void	*m_pvFile;
	bool	m_bSmooth;//�Ƿ�����
	DWORD	m_fileSize;//�ļ���С

	HRESULT OpenMapFile(char *filePath);
	HRESULT CloseMapFile();


private:
	HANDLE hFileMap;
};

#endif