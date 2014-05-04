

#ifndef TwitterOptions_h__
#define TwitterOptions_h__

#include "mString.h"
#include "TwitterDataStructure.h"

namespace mray
{
namespace Twitter
{

struct TwitterSearchOptions
{
	TwitterSearchOptions() :count(0), sinceID(0), include_entities(true)
	{
	}
	core::string Q;
	uint count;	//0 means ignore
	unsigned long sinceID; // 0 means ignore
	bool include_entities;
};

struct GetTweetOptions
{
	GetTweetOptions() :id(0),  include_entities(true)
	{
	}
	IDType id;
	bool include_entities;
};

struct UserSearchOptions
{
	UserSearchOptions() :id(0), include_entities(true)
	{
	}

	IDType id;
	core::string screenName; //utf8
	bool include_entities;
};

}
}

#endif // TwitterOptions_h__
