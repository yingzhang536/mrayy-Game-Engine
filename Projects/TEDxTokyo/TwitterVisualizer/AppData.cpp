

#include "stdafx.h"
#include "AppData.h"




namespace mray
{
namespace ted
{


	AppData::AppData()
	{
		Debugging = false;
		tweetProvider = 0;
		sessions = 0;

		leapDevice=0;
	}
	AppData::~AppData()
	{
	}


	void AppData::SetValue(const core::string&catagory, const core::string&name, const core::string& v)
	{
		s_values.setPropertie(catagory, name, v);
	}

	core::string AppData::GetValue(const core::string&catagory, const core::string&name)
	{
		return s_values.getPropertie(catagory, name);
	}
	void AppData::Load(const core::string& path)
	{
		OS::IStreamPtr stream = gFileSystem.openFile(path, OS::BIN_READ);
		if (!stream)
			return;
		s_values.loadSettings(stream);
		stream->close();

	}
	void AppData::Save(const core::string& path)
	{
		OS::IStreamPtr stream = gFileSystem.openFile(path, OS::BIN_WRITE);
		if (!stream)
			return;
		s_values.writeSettings(stream);
		stream->close();
	}

}
}
