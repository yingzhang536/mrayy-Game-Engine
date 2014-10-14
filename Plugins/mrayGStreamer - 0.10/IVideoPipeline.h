


#ifndef IVideoPipeline_h__
#define IVideoPipeline_h__



namespace mray
{
namespace video
{
	
//this is a gstreamer pipeline interface
class IVideoPipeline
{
protected:
public:
	IVideoPipeline(){}
	virtual ~IVideoPipeline(){}

	//generate pipeline string to be used by gstreamer
	virtual core::string  GetPipelineString() = 0;
	
	//create pipeline  
	virtual bool CreatePipeline() = 0;

	//get number of source elements used in this pipeline
	virtual int GetSourceCount() = 0;
	//get a specific source element name
	virtual core::string GetSourceName(int i) = 0;

	//get number of sink elements used in this pipeline
	virtual int GetSinkCount() = 0;
	//get a specific sink element name
	virtual core::string GetSinkName(int i) = 0;

	virtual void SetElementValue(const core::string& elemName, const core::string& valueName, const core::string& value) = 0;
	virtual const core::string& GetElementValue(const core::string& elemName, const core::string& valueName) = 0;
};

}
}

#endif // IVideoPipeline_h__
