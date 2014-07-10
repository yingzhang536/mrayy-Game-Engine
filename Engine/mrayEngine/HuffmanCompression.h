

#ifndef HuffmanCompression_h__
#define HuffmanCompression_h__

#include "ICompressor.h"


namespace mray
{
namespace Compress
{

class HuffmanCompression :public ICompressor
{
protected:
public:
	HuffmanCompression();
	virtual ~HuffmanCompression();


	virtual bool isLossless() { return true; }

	virtual int compress(OS::IStream*input, OS::IStream* output) ;
	virtual int decompress(OS::IStream*input, OS::IStream*output) ;

	bool CompressString(const core::string& str, OS::IStream* output);
	core::string DecompressString(OS::IStream* input);
};

}
}

#endif // HuffmanCompression_h__

