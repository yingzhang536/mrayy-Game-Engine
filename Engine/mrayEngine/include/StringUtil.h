

/********************************************************************
	created:	2011/02/23
	created:	23:2:2011   15:17
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\StringUtil.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	StringUtil
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef StringUtil_h__
#define StringUtil_h__

#include "mstring.h"

namespace mray
{
namespace core
{

class MRAY_DLL StringUtil
{
private:
protected:
public:

	static void SplitPathFileName(const core::string&str,core::string&path,core::string&file);
	static void SplitPathExt(const core::string&str,core::string&path,core::string&ext);



	//! Makes the tstring lower case.
	static core::string ToLower(const core::string& o);

	//! Makes the tstring upper case.
	static core::string ToUpper(const core::string& o);





	//make "path" relative to "root"
	//example:	root=c:\game\bin
	//			path=c:\game\data
	//result:	..\data
	static core::string MakePathRelative(const core::string&path,const core::string&root);

	//convert '/' chars into '\\'
	static void NormalizePathSlashes(mchar*str);

	static core::string Trim(const core::string& str,const core::string& chars=mT(" \t"));

	static std::vector<core::string> Split(const core::string& str,const core::string& delimStr,uint maxSplits=0);
};

}
}

#endif // StringUtil_h__
