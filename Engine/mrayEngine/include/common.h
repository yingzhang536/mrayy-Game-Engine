

#ifndef __H_common___
#define __H_common___


#include "mString.h"


namespace mray{

#define BIT(x) (1<<(x))


enum EEngineCatagories
{
	eEngine = BIT(0),
	eRender= BIT(1),
	eAI= BIT(2),
	eScene= BIT(3),
	eGUI= BIT(4),
	eOS= BIT(5),
	eSound= BIT(6),
	ePhysics= BIT(7),
	eVideo= BIT(8),
	eScript= BIT(9),
	eOther= BIT(10)
};

template <class T>
class ParameterMap:public std::map<core::string,T> 
{
public:
};

#define foreachIt(type,it,list)for(type it=list.begin();it!=list.end();++it)
	



}





#endif

