#include "stdafx.h"

#include "StringUtil.h"
#include <string>

namespace mray
{
namespace core
{

void StringUtil::SplitPathFileName(const core::string&str,core::string&path,core::string&file)
{
	core::string fullPath(str);
	NormalizePathSlashes(&fullPath[0]);
	int i=fullPath.findlast('\\');
	if(i!=-1)
	{
		path=fullPath.substr(0,i+1);
		file=fullPath.substr(i+1,fullPath.length()-i-1);
	}else
	{
		path=fullPath;
		file="";
	}
}
void StringUtil::SplitPathExt(const core::string&str,core::string&path,core::string&ext)
{
	core::string fullPath(str);
	NormalizePathSlashes(&fullPath[0]);
	int i=fullPath.findlast('.');
	if(i!=-1)
	{
		path=fullPath.substr(0,i);
		ext=fullPath.substr(i+1,fullPath.length()-i-1);
	}else
	{
		path=fullPath;
		ext="";
	}
}

void StringUtil::NormalizePathSlashes(mchar*str){
	for(mchar*p=str;*p;p++){
		if(*p=='/')
			*p='\\';
	}
}

core::string StringUtil::ToLower(const core::string& o)
{
	static const mchar diff = 'a' - 'A';
	core::string ret;
	if(o.length()==0)
		return ret;
	const mchar* ptr=o.c_str();
	ret.resize(o.length());//+1);
	for (int i=0; i<o.length(); ++i)
	{
		if (ptr[i]>='A' && ptr[i]<='Z')
			ret[i] = ptr[i]+diff;
		else
			ret[i]=ptr[i];
	}
//	ret[o.length()]=0;
	return ret;
}



//! Makes the tstring upper case.
core::string StringUtil::ToUpper(const core::string& o)
{
	static const mchar diff = 'A' - 'a';
	core::string ret;
	if(o.length()==0)
		return ret;
	const mchar* ptr=o.c_str();
	ret.resize(o.length());//+1);
	for (int i=0; i<o.length(); ++i)
	{
		if (ptr[i]>='a' && ptr[i]<='z')
			ret[i] = ptr[i]+diff;
		else
			ret[i]=ptr[i];
	}
//	ret[o.length()]=0;
	return ret;
}
//make "path" relative to "root"
//example:	root=c:\game\bin
//			path=c:\game\data
//result:	..\data
core::string StringUtil::MakePathRelative(const core::string&path,const core::string&root)
{
	int matched=0;
	//	path.make_lower();
	//	app.make_lower();
	core::string cpy(path);
	core::string cpyRoot(root);
	NormalizePathSlashes(&cpy[0]);
	NormalizePathSlashes(&cpyRoot[0]);

	for(int i=0;i<cpy.length() && i<cpyRoot.length();++i)
	{
		if(tolower(cpy[i])!=tolower(cpyRoot[i]))
			break;
		else if(cpy[i]=='\\')
			matched=i;
	}
	core::string minPath;
	int appFolders=1;
	for(int i=matched+1;i<cpyRoot.length();++i)
	{
		if(cpyRoot[i]=='\\'){
			minPath+=mT("..\\");
			++appFolders;
		}
	}
	minPath+=path.substr(matched+1,path.length()-matched);

	return minPath;
}

core::string StringUtil::Trim(const core::string& str,const core::string& chars)
{
	core::string ret;
	bool found=false;
	for(int i=0;i<str.length();++i)
	{
		if(chars.find(str[i])==-1 || found)
		{
			found=true;
			ret.appendChar(str[i]);
		}
	}
	for(int i=ret.length()-1;i>=0;--i)
	{
		if(chars.find(ret[i])==-1)
		{
			if(i<ret.length()-1)
			{
				ret[i+1]=0;
			}
			break;
		}
	}
	return ret.c_str();
}
 std::vector<core::string> StringUtil::Split(const core::string& str,const core::string& delimStr,uint maxSplits)
{
	std::vector<core::string> ret;
	ret.reserve(maxSplits ? maxSplits:10);

	core::string temp;
	const mchar*p=str.c_str();
	int cnt=0;
	for(;*p!=0;p++){
		if(delimStr.find(*p)!=-1 ){
			if(temp!=_T("\0")){
				ret.push_back(temp);
				temp=_T("\0");
				cnt++;
				if(maxSplits!=0 && cnt>=maxSplits)
					break;
			}
		}else
			temp+=*p;
	}
	if(maxSplits==0 || cnt<maxSplits){
		if(temp!=_T("\0")){
			ret.push_back(temp);
		}
	}
	return ret;
}

}
}