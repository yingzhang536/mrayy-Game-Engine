
#include "stdafx.h"
#include "HuffmanCompression.h"
#include "IStream.h"

#include <huffman/huffman.h>


namespace mray
{
namespace Compress
{

HuffmanCompression::HuffmanCompression()
:ICompressor("Huffman")
{
}

HuffmanCompression::~HuffmanCompression()
{
}


int HuffmanCompression::compress(OS::IStream*input, OS::IStream* output)
{
	uint outLen = 0;
	uchar *outBuf = 0;
	uchar *bytes = 0;
	uint len = input->readToEnd((void**)&bytes);
	core::string ret;
	if (huffman_encode_memory(bytes, len, &outBuf, &outLen) == 0)
	{
		output->write(outBuf, outLen);
	}
	delete[] bytes;
	delete[] outBuf;
	return outLen;
}

int HuffmanCompression::decompress(OS::IStream*input, OS::IStream*output)
{
	uint outLen = 0;
	uchar *outBuf = 0;
	uchar *bytes = 0;
	uint len = input->readToEnd((void**)&bytes);
	core::string ret;
	if (huffman_decode_memory(bytes, len, &outBuf, &outLen) == 0)
	{
		output->write(outBuf, outLen);
	}
	delete[] bytes;
	delete[] outBuf;
	return outLen;
}


bool HuffmanCompression::CompressString(const core::string& str, OS::IStream* output)
{
	uint outLen = 0;
	uchar *outBuf = 0;
	bool ok = huffman_encode_memory((const uchar*)str.c_str(), str.length(), &outBuf, &outLen) == 0;
	if (ok)
		output->write(outBuf, outLen);
	
	delete [] outBuf;
	return ok;
}

core::string HuffmanCompression::DecompressString(OS::IStream* input)
{
	uint outLen = 0;
	uchar *outBuf = 0;
	uchar *bytes=0;
	uint len=input->readToEnd((void**)&bytes);
	core::string ret;
	if (huffman_decode_memory(bytes, len, &outBuf, &outLen) == 0)
	{
		ret = (mchar*)bytes;
	}
	delete[] bytes;
	delete[] outBuf;
	return ret;
}


}
}

