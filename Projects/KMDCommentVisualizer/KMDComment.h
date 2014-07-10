

/********************************************************************
	created:	2014/03/31
	created:	31:3:2014   11:32
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\KMDComment.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	KMDComment
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __KMDComment__
#define __KMDComment__


#include "DataTypes.h"
#include "DateTime.h"

namespace mray
{
namespace kmd
{
	class KMDUser;
	class SessionDetails;

class KMDComment
{
protected:
public:
	static std::map<IDType, uint> CommentIDMap;
	static std::vector<KMDComment*> KMDCommentList;
	static void AddKMDComment(KMDComment* t);
	static KMDComment* GetCommentByID(IDType id,bool localOnly);

public:
	KMDComment() :ID(0), user(0), project(0)
	{}
	virtual~KMDComment(){}

	IDType ID;
	KMDUser* user;
	core::stringw text;
	core::DateTime date;
	SessionDetails* project;


	void LoadXML(xml::XMLElement* e);
	void SaveXML(xml::XMLElement* e);
};

}
}


#endif
