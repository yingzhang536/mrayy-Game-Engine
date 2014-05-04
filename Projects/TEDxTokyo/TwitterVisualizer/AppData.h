

#ifndef AppData_h__
#define AppData_h__

#include "ISingleton.h"

namespace mray
{
	namespace nui
	{
		class LeapDevice;
	}
namespace ted
{
	class TwitterProvider;
	class SessionContainer;

class AppData :public ISingleton<AppData>
{
protected:
	script::CSettingsFile s_values;
public:
	AppData();
	virtual~AppData();


	virtual void Load(const core::string& path);
	virtual void Save(const core::string& path);

	void SetValue(const core::string&catagory, const core::string&name, const core::string& v);
	core::string GetValue(const core::string&catagory, const core::string&name);

	bool Debugging ;

public:

	TwitterProvider* tweetProvider;
	nui::LeapDevice* leapDevice;

	SessionContainer* sessions;
};

#define gAppData ted::AppData::getInstance()

}
}

#endif // AppData_h__
