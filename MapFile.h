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
	bool	m_bSmooth;//是否阻塞
	DWORD	m_fileSize;//文件大小

	HRESULT OpenMapFile(char *filePath);
	HRESULT CloseMapFile();


private:
	HANDLE hFileMap;
};

#endif