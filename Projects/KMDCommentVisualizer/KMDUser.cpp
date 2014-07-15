
#include "stdafx.h"
#include "KMDUser.h"

#include "SQLAPI.h"
#include "DataTypes.h"
#include "IDBHandler.h"
#include "StringUtil.h"
#include "TextureResourceManager.h"

namespace mray
{
namespace kmd
{
class UserD;


std::map<IDType, uint> KMDUser::UserIDMap;
std::vector<KMDUser*> KMDUser::KMDUserList;
KMDUser* KMDUser::studentUser = 0;
KMDUser* KMDUser::facultyUser = 0;

void KMDUser::AddKMDUser(KMDUser* t)
{
	KMDUserList.push_back(t);
	GUID id(core::StringUtil::ToLower(t->name));
	UserIDMap[id.ID()] = KMDUserList.size() - 1;
}
KMDUser* KMDUser::GetUserByID(IDType id)
{
	std::map<IDType, uint>::iterator it = UserIDMap.find(id);
	if (it == UserIDMap.end())
	{
		return 0;
	}
	return KMDUserList[it->second];
}

KMDUser* KMDUser::GetUserByName(const core::string& name)
{
	GUID id(core::StringUtil::ToLower(name));
	return GetUserByID(id.ID());
}

void KMDUser::LoadXML(xml::XMLElement* e)
{
//	ID = core::StringConverter::toUInt(e->getValueString("ID"));
	imageUrl = e->getValueString("imageUrl");
	name = e->getValueString("name");
	title = e->getValueString("Title");


	m_imgTexture = 0;
	if (imageUrl != "")
		m_imgTexture = gTextureResourceManager.loadTexture2D(imageUrl);

	if (m_imgTexture.isNull())
		m_imgTexture = gTextureResourceManager.loadTexture2D("DefaultPicture.png");
	if (m_imgTexture)
		m_imgTexture->setMipmapsFilter(false);
}
void KMDUser::SaveXML(xml::XMLElement* e)
{
	xml::XMLElement* elem = new xml::XMLElement("User");
	e->addSubElement(elem);

//	elem->addAttribute("ID", core::StringConverter::toString(ID));
	elem->addAttribute("imageUrl", imageUrl);
	elem->addAttribute("name", name);
}


}
}

