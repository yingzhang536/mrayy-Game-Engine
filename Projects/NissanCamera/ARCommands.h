


#ifndef ARCommands_h__
#define ARCommands_h__

#include "ICommand.h"

namespace mray
{
	class IARCommandListener
	{
	public: 
		virtual void CreateARObject(uint id, const core::string& name, const math::vector3d& pos, const math::vector3d& dir){}
		virtual void UpdateARObject(uint id, const math::vector3d& pos, const math::vector3d& dir){}
		virtual void MoveARObject(uint id, const math::vector3d& pos, const math::vector3d& dir){}
		virtual void RemoveARObject(uint id){}
		virtual void SelectARObject(uint id){}
		virtual void SetARAlpha(uint id,float v){}
		virtual bool QueryARObject(uint id, math::vector3d& pos, math::vector3d& dir){ return false; }
		virtual void ListARObjects(std::vector<uint> &ids){}
		virtual void ChangeARFov(float fov){}
	};
	
class ARCreateObject:public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARCreateObject(CommandManager* o, IARCommandListener* listener);
	virtual ~ARCreateObject();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo() ;

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount() ;

	virtual bool onCommand(std::vector<core::string>& args) ;

	virtual const core::string& getLastMessage();
};

class ARUpdateCommand :public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARUpdateCommand(CommandManager* o, IARCommandListener* listener);
	virtual ~ARUpdateCommand();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo();

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount();

	virtual bool onCommand(std::vector<core::string>& args);

	virtual const core::string& getLastMessage();
};

class ARMoveCommand :public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARMoveCommand(CommandManager* o, IARCommandListener* listener);
	virtual ~ARMoveCommand();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo();

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount();

	virtual bool onCommand(std::vector<core::string>& args);

	virtual const core::string& getLastMessage();
};
class ARAlphaCommand :public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARAlphaCommand(CommandManager* o, IARCommandListener* listener);
	virtual ~ARAlphaCommand();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo();

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount();

	virtual bool onCommand(std::vector<core::string>& args);

	virtual const core::string& getLastMessage();
};


class ARRemoveCommand :public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARRemoveCommand(CommandManager* o, IARCommandListener* listener);
	virtual ~ARRemoveCommand();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo();

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount();

	virtual bool onCommand(std::vector<core::string>& args);

	virtual const core::string& getLastMessage();
};

class ARQeuryCommand :public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARQeuryCommand(CommandManager* o, IARCommandListener* listener);
	virtual ~ARQeuryCommand();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo();

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount();

	virtual bool onCommand(std::vector<core::string>& args);

	virtual const core::string& getLastMessage();
};
class ARFovCommand :public ICommand
{
protected:
	IARCommandListener* m_listener;
	core::string m_lastMsg;
public:
	ARFovCommand(CommandManager* o, IARCommandListener* listener);
	virtual ~ARFovCommand();

	virtual core::string getCommandName();

	virtual core::string getCommandInfo();

	virtual int getCommandMinArgCount();

	virtual int getCommandMaxArgCount();

	virtual bool onCommand(std::vector<core::string>& args);

	virtual const core::string& getLastMessage();
};


}

#endif // ARCommands_h__
