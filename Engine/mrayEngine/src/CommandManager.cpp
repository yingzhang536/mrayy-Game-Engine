#include "stdafx.h"


#include "CommandManager.h"
#include "IFileSystem.h"
#include "StreamReader.h"
#include "StringUtil.h"

namespace mray{

	class CPrintCommands:public ICommand
	{
	protected:
		core::string m_msg;
	public:
		CPrintCommands(CommandManager* o):ICommand(o)
		{
		}

		virtual core::string getCommandName(){
			return mT("help");
		}

		virtual core::string getCommandInfo(){
			return mT("List the Avaliable Commands");
		}

		virtual int getCommandMinArgCount(){return 0;}

		virtual int getCommandMaxArgCount(){return 1;}

		virtual bool onCommand(std::vector<core::string>& args)
		{
			if (args.size() == 1)
			{
				CommandList::const_iterator p = m_owner->getCommands().begin();
				CommandList::const_iterator end = m_owner->getCommands().end();
				m_msg = mT("List of available commands:\n");
				for (; p != end; ++p)
				{
					if (p->second->isEnabled()){
						m_msg += p->second->getCommandName()+"\n";
					}
				}
			}
			else
			{
				CommandList::const_iterator p = m_owner->getCommands().begin();
				CommandList::const_iterator end = m_owner->getCommands().end();
				for (; p != end; ++p)
				{
					if (p->second->getCommandName().equals_ignore_case(args[1])){
						m_msg = mT("Details about command:") ;
						m_msg += p->second->getCommandName();
						m_msg += mT("\n");
						m_msg += p->second->getCommandInfo();
						/*m_msg += mT("\n");
						m_msg += mT("Min Parameters count: ") + core::StringConverter::toString(p->second->getCommandMinArgCount());
						m_msg += mT("\n");
						m_msg += mT("Max Parameters count: ") + core::StringConverter::toString(p->second->getCommandMaxArgCount());
			*/			
						break;
					}
				}
			}
			return true;
		}

		virtual const core::string& getLastMessage(){ return m_msg; }

	};

	class CExecCommand :public ICommand
	{
	protected:
		core::string m_msg;
	public:
		CExecCommand(CommandManager* o) :ICommand(o)
		{
		}

		virtual core::string getCommandName(){
			return mT("exec");
		}

		virtual core::string getCommandInfo(){
			return mT("Execute an external commands file");
		}

		virtual int getCommandMinArgCount(){ return 1; }

		virtual int getCommandMaxArgCount(){ return 1; }

		virtual bool onCommand(std::vector<core::string>& args)
		{
			OS::IStreamPtr file= gFileSystem.openFile(args[1]);
			if (!file)
			{
				m_msg = "Failed to load command file: " + args[1];
				return false;
			}
			OS::StreamReader reader(file);
			m_msg = "Executing external command file: "+args[1]+"\n";
			while (!file->eof())
			{
				core::string line= reader.readLine();
				line=core::StringUtil::Trim(line,"\r \t");
				if (line == "")
					continue;
				m_owner->execCommand(line);
				m_msg += " >>" + line+"\n";
				m_msg += m_owner->getLastMessage()+"\n";
			}
			file->close();
			return true;
		}

		virtual const core::string& getLastMessage(){ return m_msg; }

	};

CommandManager::CommandManager(){
	addCommand(new CPrintCommands(this));
	addCommand(new CExecCommand(this));
}
CommandManager::~CommandManager(){
}

void CommandManager::addCommand(GCPtr<ICommand> cmd){
	core::string s=core::StringUtil::ToLower(cmd->getCommandName());
	m_commands[s]=cmd;
	cmd->SetOwnerManager(this);
}

bool CommandManager::execCommand(const core::string&cmd){
	m_args=core::StringUtil::Split(cmd,mT(" "));

	if(m_args.size()==0){
		m_lastMsg=mT("Enter Command Name!");
		return false;
	}
	core::string s=core::StringUtil::ToLower(m_args[0]);
	CommandList::iterator it= m_commands.find(s);
	bool res=false;
	if(it!=m_commands.end()){
		GCPtr<ICommand> cmd=it->second;
		if (m_args.size() - 1<cmd->getCommandMinArgCount() && cmd->getCommandMinArgCount() != -1 ||
			m_args.size() - 1>cmd->getCommandMaxArgCount() && cmd->getCommandMaxArgCount() != -1)
		{
			m_lastMsg=mT("Parameters count not matched!");
		}
		else
		{
			res = cmd->onCommand(m_args);
			m_lastMsg = cmd->getLastMessage();
		}
	}else{
		m_lastMsg=mT("Command Not Found!");
	}

	MessageLog(m_lastMsg);
	return res;
}

void CommandManager::removeCommand(GCPtr<ICommand> cmd){
	CommandList::iterator it= m_commands.begin();
	for (;it!=m_commands.end();++it)
	{
		if(it->second==cmd){
			m_commands.erase(it);
			return;
		}
	}
}
void CommandManager::removeCommand(const core::string&name){
	m_commands.erase(name);
}

const  core::string&CommandManager::getLastMessage(){
	return m_lastMsg;
}

const CommandList& CommandManager::getCommands(){
	return m_commands;
}

}
