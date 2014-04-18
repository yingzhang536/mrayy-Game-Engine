#include "stdafx.h"

#include "WinDirOS.h"
#include "WinFileSystem.h"

#include <io.h>
#include <direct.h>
#include <windows.h>

#ifdef UNICODE
#define ChangeDir _wchdir
#else
#define ChangeDir _chdir
#endif

namespace mray{
namespace OS{

WinDirOS::WinDirOS(){

	tempPath.resize(MAX_PATH);

	refreshFiles();
}
WinDirOS::~WinDirOS(){
	cfiles.clear();
}

void WinDirOS::refreshFiles()
{
	
	cfiles.clear();
	cpath=gFileSystem.getCurrPath();

	cdriveLetter=gFileSystem.getCurrDrive()+'A'-1;

	cdriveVolume=gFileSystem.getCurrDriveName();

	WIN32_FIND_DATA fd;
	HANDLE hFile;
	SFileData fData;
	
	if( (hFile = FindFirstFile( mT("*"), &fd )) != 0 )
	{
		do{
		//	char_to_wchar(fname,fd.name);
			fData.name=fd.cFileName;
			fData.size=(int)fd.nFileSizeLow;
			fData.isDir=(_A_SUBDIR & fd.dwFileAttributes)!=0;
			cfiles.push_back(fData);

		}
		while(FindNextFile(hFile,&fd));
		FindClose(hFile);
	}

}
	
const  core::string& WinDirOS::getCurrPath(){
	return cpath;
}

const std::vector<SFileData> & WinDirOS::getFiles(){
	return cfiles;
}


const  core::string& WinDirOS::getShortFileName(int index){
	if(index<0 || index>cfiles.size())return core::string::Empty;
	return cfiles[index].name;
}

const  core::string& WinDirOS::getFullFileName(int index){
	if(index<0 || index>cfiles.size())return core::string::Empty;
	
	//wsprintf(&tempPath[0],mT("%s\\%s"),cpath.c_str(),cfiles[index].name.c_str());

	tempPath=cpath+mT("\\")+cfiles[index].name;

	return tempPath;
}

int WinDirOS::getFileSize(int index)
{
	if(index<0 || index>cfiles.size())return 0;
	return cfiles[index].size;
}

bool WinDirOS::isFileDirectory(int index)
{
	if(index<0 || index>cfiles.size())return 0;
	return cfiles[index].isDir;
}

bool WinDirOS::changeDir(int index)
{
	if(index<0 || index>cfiles.size())return 0;
	if(ChangeDir(cfiles[index].name.c_str())==-1)return 0;
	refreshFiles();
	return 1;
}


bool WinDirOS::changeDir()
{
	if(_chdir("..")==-1)return 0;
	refreshFiles();
	return 1;
}
bool WinDirOS::changeDir(const  core::string& path)
{
	if(ChangeDir(path.c_str())==-1)return 0;
	refreshFiles();
	return 1;
}

int WinDirOS::getFilesCount()
{
	return cfiles.size();
}
	



int WinDirOS::getFileIndex(const  core::string& fName){
	for(int i=0;i<cfiles.size();++i)
	{
		if(cfiles[i].name.equals_ignore_case(fName))
			return i;
	}
	return -1;
}

char WinDirOS::getCurrDriveLetter(){
	return cdriveLetter;
}

core::string  WinDirOS::getCurrDriveVolume(){
	return cdriveVolume;
}


bool WinDirOS::createDirectory(const  core::string& dirName){
	bool ret=gFileSystem.createDirectory(dirName);
	if(ret)
		refreshFiles();
	return ret;
}


//! return file extension
core::string WinDirOS::getFileType(int index){
	if(index<0 || index>cfiles.size())return core::string::Empty;
	int p=cfiles[index].name.findlast('.');
	if(p==-1)return core::string::Empty;
	return cfiles[index].name.substr(p+1,cfiles[index].name.length()-p-1);
}


}
}



