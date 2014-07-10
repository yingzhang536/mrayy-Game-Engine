
#ifndef IDepthProvider_h__
#define IDepthProvider_h__



namespace mray
{
	
class IDepthProvider
{
protected:
public:
	IDepthProvider(){}
	virtual ~IDepthProvider(){}


	virtual void Init() = 0;
	virtual void Start(int w, int h) = 0;
	virtual void Close() = 0;

	virtual bool IsStarted() = 0;

};

}

#endif // IDepthProvider_h__
