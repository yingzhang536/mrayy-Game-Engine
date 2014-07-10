

/********************************************************************
	created:	2014/04/09
	created:	9:4:2014   21:42
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\UserD.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	UserD
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __UserD__
#define __UserD__

#include "DataTypes.h"

class SACommand;

namespace mray
{
namespace kmd
{

class KMDUser
{
protected:
public:
	static std::map<IDType, uint> UserIDMap;
	static std::vector<KMDUser*> KMDUserList;
	static void AddKMDUser(KMDUser* t);
	static KMDUser* GetUserByID(IDType id);
	static KMDUser* GetUserByName(const core::string& name);
public:

	//IDType ID;
	core::string name;
	core::string imageUrl;

	KMDUser() {}
	virtual~KMDUser(){}

	void LoadXML(xml::XMLElement* e);
	void SaveXML(xml::XMLElement* e);

	static KMDUser* studentUser;
	static KMDUser* facultyUser;
};

}
}


#endif
