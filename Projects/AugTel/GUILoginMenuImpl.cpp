
#include "stdafx.h"
#include "GUILoginMenuImpl.h"
#include "RobotInfoManager.h"
#include "ATAppGlobal.h"


namespace mray
{


GUILoginMenuImpl::GUILoginMenuImpl()
{
}

GUILoginMenuImpl::~GUILoginMenuImpl()
{
}

void GUILoginMenuImpl::UpdateRobotList()
{
	if (!RobotLst)
		return;
	RobotLst->ClearItems();
	const std::vector<TBee::TBRobotInfo>& lst=gAppData.robotInfoManager->GetRobots();
	int si = 0;
	for (int i = 0; i < lst.size();++i)
	{
		TBee::TBRobotInfo* ifo = (TBee::TBRobotInfo*) &lst[i];
		if (gAppData.selectedRobot ==ifo)
			si = i;
		GUI::StringListItem* itm = new GUI::StringListItem(lst[i].name, (void*)ifo);
		RobotLst->AddItem(itm);
	}
	RobotLst->SetSelectedItem(si);
}
TBee::TBRobotInfo* GUILoginMenuImpl::UpdateSelectedRobot()
{
	if (!RobotLst)
		return 0;
	int si = RobotLst->GetSelectedItem();
	if (si == -1)
		si = 0;
	GUI::StringListItem* itm = (GUI::StringListItem*)RobotLst->GetItem(si);
	if (itm)
	{
		gAppData.selectedRobot = (TBee::TBRobotInfo*)itm->userData;
	}
	return gAppData.selectedRobot;
}


}
