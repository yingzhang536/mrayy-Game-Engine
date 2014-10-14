


#ifndef NetworkStreamVideoPipeline_h__
#define NetworkStreamVideoPipeline_h__

#include "IVideoPipeline.h"

namespace mray
{
namespace video
{
	
class NetworkStreamVideoPipeline :public IVideoPipeline
{
protected:
public:
	NetworkStreamVideoPipeline();
	virtual ~NetworkStreamVideoPipeline();

	void SetIPAddress(const core::string& ip, int videoPort, int audioPort);

	virtual core::string  GetPipelineString() ;

	virtual bool CreatePipeline() ;


	virtual int GetSourceCount() ;
	virtual core::string GetSourceName(int i) ;

	virtual int GetSinkCount() ;
	virtual core::string GetSinkName(int i) ;

	virtual void SetElementValue(const core::string& elemName, const core::string& valueName, const core::string& value) ;
	virtual const core::string& GetElementValue(const core::string& elemName, const core::string& valueName) ;
};

}
}

#endif // NetworkStreamVideoPipeline_h__
