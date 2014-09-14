

#include "stdafx.h"
#include "ARCommands.h"


namespace mray
{


ARCreateObject::ARCreateObject(CommandManager* o, IARCommandListener* listener)
:ICommand(o), m_listener(listener)
{

}

ARCreateObject::~ARCreateObject()
{

}


core::string ARCreateObject::getCommandName()
{
	return "create";
}


core::string ARCreateObject::getCommandInfo()
{
	return " create [id] [filename] {[x,y,z] [pitch,yaw,roll]}\n "
		"Create an AR Object at the specified location and orientation. important to provide unique id so you can manage it later";
}


int ARCreateObject::getCommandMinArgCount()
{
	return 2;
}


int ARCreateObject::getCommandMaxArgCount()
{
	return 4;
}


bool ARCreateObject::onCommand(std::vector<core::string>& args)
{
	uint id = core::StringConverter::toUInt(args[1]);
	core::string fname = args[2];
	math::vector3d pos,dir;
	if (args.size() > 3)
		pos = core::StringConverter::toVector3d(args[3]);
	if (args.size() > 4)
		dir = core::StringConverter::toVector3d(args[4]);

	m_listener->CreateARObject(id, fname, pos, dir);

	return true;
}


const core::string& ARCreateObject::getLastMessage()
{
	return m_lastMsg;
}

//////////////////////////////////////////////////////////////////////////


ARUpdateCommand::ARUpdateCommand(CommandManager* o, IARCommandListener* listener)
:ICommand(o), m_listener(listener)
{

}

ARUpdateCommand::~ARUpdateCommand()
{

}


core::string ARUpdateCommand::getCommandName()
{
	return "update";
}


core::string ARUpdateCommand::getCommandInfo()
{
	return "Format: update [id] [x,y,z] [pitch,yaw,roll]\n"
		"Update ar object location and direction to new values";
}


int ARUpdateCommand::getCommandMinArgCount()
{
	return 3;
}


int ARUpdateCommand::getCommandMaxArgCount()
{
	return 3;
}


bool ARUpdateCommand::onCommand(std::vector<core::string>& args)
{
	uint id = core::StringConverter::toUInt(args[1]);
	math::vector3d pos, dir;
	pos = core::StringConverter::toVector3d(args[2]);
	dir = core::StringConverter::toVector3d(args[3]);

	m_listener->UpdateARObject(id, pos, dir);

	return true;
}


const core::string& ARUpdateCommand::getLastMessage()
{
	return m_lastMsg;
}

//////////////////////////////////////////////////////////////////////////

ARMoveCommand::ARMoveCommand(CommandManager* o, IARCommandListener* listener)
	:ICommand(o), m_listener(listener)
{

}

ARMoveCommand::~ARMoveCommand()
{

}


core::string ARMoveCommand::getCommandName()
{
	return "move";
}


core::string ARMoveCommand::getCommandInfo()
{
	return "Format: move [id] [x,y,z] [pitch,yaw,roll]\n"
		"Move AR object location and direction relative to their location";
}


int ARMoveCommand::getCommandMinArgCount()
{
	return 3;
}


int ARMoveCommand::getCommandMaxArgCount()
{
	return 3;
}


bool ARMoveCommand::onCommand(std::vector<core::string>& args)
{
	uint id = core::StringConverter::toUInt(args[1]);
	math::vector3d pos, dir;
	pos = core::StringConverter::toVector3d(args[2]);
	dir = core::StringConverter::toVector3d(args[3]);

	m_listener->MoveARObject(id, pos, dir);

	return true;
}


const core::string& ARMoveCommand::getLastMessage()
{
	return m_lastMsg;
}

//////////////////////////////////////////////////////////////////////////


ARRemoveCommand::ARRemoveCommand(CommandManager* o, IARCommandListener* listener)
:ICommand(o), m_listener(listener)
{

}

ARRemoveCommand::~ARRemoveCommand()
{

}


core::string ARRemoveCommand::getCommandName()
{
	return "remove";
}


core::string ARRemoveCommand::getCommandInfo()
{
	return "Format: remove [id] \n"
		"calling this function will remove and object with specific id";
}


int ARRemoveCommand::getCommandMinArgCount()
{
	return 1;
}


int ARRemoveCommand::getCommandMaxArgCount()
{
	return 1;
}


bool ARRemoveCommand::onCommand(std::vector<core::string>& args)
{
	uint id = core::StringConverter::toUInt(args[1]);

	m_listener->RemoveARObject(id);

	return true;
}


const core::string& ARRemoveCommand::getLastMessage()
{
	return m_lastMsg;
}


//////////////////////////////////////////////////////////////////////////


ARQeuryCommand::ARQeuryCommand(CommandManager* o, IARCommandListener* listener)
:ICommand(o), m_listener(listener)
{

}

ARQeuryCommand::~ARQeuryCommand()
{

}


core::string ARQeuryCommand::getCommandName()
{
	return "query";
}


core::string ARQeuryCommand::getCommandInfo()
{
	return "Format: query [id] \n"
		" Query for location and direction of an object with id ";
}


int ARQeuryCommand::getCommandMinArgCount()
{
	return 1;
}


int ARQeuryCommand::getCommandMaxArgCount()
{
	return 1;
}


bool ARQeuryCommand::onCommand(std::vector<core::string>& args)
{
	uint id = core::StringConverter::toUInt(args[1]);
	math::vector3d pos, dir;
	if (!m_listener->QueryARObject(id, pos, dir))
	{
		m_lastMsg = "Object with ID="+core::StringConverter::toString(id)+" was not found!" ;
	}
	else
	{
		m_lastMsg = "Position= " + core::StringConverter::toString(pos)+"\n";
		m_lastMsg += "Direction= " + core::StringConverter::toString(dir);
	}
	return true;
}


const core::string& ARQeuryCommand::getLastMessage()
{
	return m_lastMsg;
}

//////////////////////////////////////////////////////////////////////////


ARAlphaCommand::ARAlphaCommand(CommandManager* o, IARCommandListener* listener)
	:ICommand(o), m_listener(listener)
{

}

ARAlphaCommand::~ARAlphaCommand()
{

}


core::string ARAlphaCommand::getCommandName()
{
	return "alpha";
}


core::string ARAlphaCommand::getCommandInfo()
{
	return "Format: alpha [id] [value] \n"
		"All AR objects: alpha  [value] \n"
		" Change alpha value. 0->100: 0 is fully transparent, 100 is fully opaque ";
}


int ARAlphaCommand::getCommandMinArgCount()
{
	return 1;
}


int ARAlphaCommand::getCommandMaxArgCount()
{
	return 2;
}


bool ARAlphaCommand::onCommand(std::vector<core::string>& args)
{
	float alpha = 0;
	uint id;
	if (args.size() == 2)
	{
		alpha = core::StringConverter::toFloat(args[1]);
		id = -1;
	}else 
	{
		id = core::StringConverter::toUInt(args[1]);
		alpha = core::StringConverter::toFloat(args[2]);
	}
	m_listener->SetARAlpha(id, alpha*0.01f);
	return true;
}


const core::string& ARAlphaCommand::getLastMessage()
{
	return m_lastMsg;
}


}
