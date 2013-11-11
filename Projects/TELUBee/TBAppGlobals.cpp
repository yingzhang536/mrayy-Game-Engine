

#include "StdAfx.h"
#include "TBAppGlobals.h"
#include "Application.h"
#include "XMLTree.h"
#include "XMLWriter.h"
#include "TBeeOptiTrackObject.h"


#include <winsock2.h>		// for struct in_addr



namespace mray
{
namespace TBee
{


Application* TBAppGlobals::App=0;
SeeThroughWindow* TBAppGlobals::seeTrough=0;
script::CSettingsFile TBAppGlobals::s_values;
bool TBAppGlobals::IsDebugging=false;
core::string TBAppGlobals::ServerIP="";
db::ISQLManager* TBAppGlobals::sqlManager=0;
MapObject* TBAppGlobals::mapObj=0;
core::string TBAppGlobals::userName;
core::string TBAppGlobals::userPassword;
RobotInfoManager* TBAppGlobals::robotInfoManager=0;
TBRobotInfo* TBAppGlobals::selectedRobot=0;
core::string TBAppGlobals::DVIPort;
TBeeOptiTrackObject* TBAppGlobals::optiObj=0;
InputManager* TBAppGlobals::inputMngr = 0;
video::OculusDevice* TBAppGlobals::oculusDevice = 0;

int TBAppGlobals::MajorVer=1;
int TBAppGlobals::MinorVer=0;

scene::EStereoMode TBAppGlobals::StereoMode=scene::EStereo_None;

bool TBAppGlobals::usingOculus=false;


math::vector2d TBAppGlobals::GetStereoScaleRatio()
{
	switch(StereoMode)
	{
	case scene::EStereo_None:
	case scene::EStereo_Anglyph:
		return math::vector2d(1,1);
	case scene::EStereo_Interlance:
	case scene::EStereo_TopDown:
		return math::vector2d(1,0.5);
	case scene::EStereo_SideBySide:
		return math::vector2d(0.5,1);
	}
	return math::vector2d(1,1);
}
core::string TBAppGlobals::GetVersion()
{
	return core::StringConverter::toString(MajorVer)+"."+core::StringConverter::toString(MinorVer);
}
core::string TBAppGlobals::GetBuild()
{
	return core::string(__DATE__)+" "+core::string(__TIME__);
}
void TBAppGlobals::SetValue(const core::string&catagory,const core::string&name,const core::string& v)
{
	s_values.setPropertie(catagory,name,v);
}

core::string TBAppGlobals::GetValue(const core::string&catagory,const core::string&name)
{
	return s_values.getPropertie(catagory,name);
}
void TBAppGlobals::Load(const core::string& path)
{
	OS::IStreamPtr stream=gFileSystem.openFile(path,OS::BIN_READ);
	if(!stream)
		return;
	s_values.loadSettings(stream);
	stream->close();
	/*
	xml::XMLTree t;
	if(!t.load(path))
		return;
	xml::XMLElement* root= t.getSubElement("Settings");
	if(!root)
		return;

	xml::XMLElement* tag=root->getSubElement("Catagory");
	while(tag)
	{
		xml::XMLAttribute*attr=tag->getAttribute("Name");
		xml::xmlAttributesMapIT it= root->getAttributesBegin();
		xml::xmlAttributesMapIT end= root->getAttributesEnd();
		for (;it!=end;++it)
		{
			SetValue(attr->value,it->second.name,it->second.value);
		}
		tag=tag->nextSiblingElement("Catagory");
	}*/

}
void TBAppGlobals::Save(const core::string& path)
{
	OS::IStreamPtr stream=gFileSystem.openFile(path,OS::BIN_WRITE);
	if(!stream)
		return;
	s_values.writeSettings(stream);
	stream->close();
	/*
	xml::XMLElement root("Settings");
	std::map<core::string,core::string>::iterator it= s_values.begin();
	for(;it!=s_values.end();++it)
	{
		root.addAttribute(it->first,it->second);
	}

	xml::XMLWriter wrtr;
	wrtr.addElement(&root);

	core::string xmlTxt= wrtr.flush();

	OS::IStreamPtr stream=gFileSystem.openFile(path,OS::BIN_WRITE);
	if(!stream)
		return;
	OS::StreamWriter sw(stream);
	sw.writeString(xmlTxt);
	stream->close();*/


}


void TBAppGlobals::ConnectOpti()
{
	if(!optiObj || optiObj->IsConnected())
		return;
	in_addr ipaddress;

	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		printf("Error when getting local host name \n");
		return;
	}

	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
		printf("Bad host lookup.\n");
		return;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		ipaddress = addr;	
	}
	core::string ipaddr=inet_ntoa(ipaddress);

	optiObj->Connect(ipaddr,ipaddr);
}
void TBAppGlobals::DisconnectOpti()
{
	optiObj->Disconnect();
}

}
}

