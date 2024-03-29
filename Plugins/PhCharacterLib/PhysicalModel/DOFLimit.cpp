

#include "stdafx.h"
#include "DOFLimit.h"

namespace mray{
namespace PhysicsBasedCharacter
{
	
		void DOFLimit::Serialize(xml::XMLElement*elem)
		{
			if(!elem)
				return;
			xml::xmlAttributesMapIT it= elem->getAttributesBegin();
			xml::xmlAttributesMapIT end= elem->getAttributesEnd();
	
			core::stringc name;
	
			for(;it!=end;++it){
				xml::XMLAttribute&attr= it->second;
				core::string_to_char(attr.name,name);
				name.make_lower();
	
				if(name=="xlimit"){
					xlimitType=(physics::ED6JointMotion)core::StringConverter::toEnum(mT("ED6JointMotion"),attr.value);
				}else if(name=="ylimit"){
					ylimitType=(physics::ED6JointMotion)core::StringConverter::toEnum(mT("ED6JointMotion"),attr.value);
				}else if(name=="zlimit"){
					zlimitType=(physics::ED6JointMotion)core::StringConverter::toEnum(mT("ED6JointMotion"),attr.value);
				}else if(name=="swing1"){
					swing1Type=(physics::ED6JointMotion)core::StringConverter::toEnum(mT("ED6JointMotion"),attr.value);
				}else if(name=="swing2"){
					swing2Type=(physics::ED6JointMotion)core::StringConverter::toEnum(mT("ED6JointMotion"),attr.value);
				}else if(name=="twist"){
					twistType=(physics::ED6JointMotion)core::StringConverter::toEnum(mT("ED6JointMotion"),attr.value);
				}else if(name=="collision"){
					collision=core::StringConverter::toBool(attr.value);
				}

			}
	
			xml::xmlSubElementsMapIT eit=elem->getElementsBegin();
			xml::xmlSubElementsMapIT eEnd=elem->getElementsEnd();
			core::stringc elemName;
			for(;eit!=eEnd;++eit)
			{
				if((*eit)->GetType()!=xml::ENT_Element)
					continue;
				xml::XMLElement* e=dynamic_cast<xml::XMLElement*>(*eit);
				core::string_to_char(e->getName(),elemName);
				elemName.make_lower();
	
				if(elemName=="linearlimit")
				{
					it= e->getAttributesBegin();
					end= e->getAttributesEnd();
					for(;it!=end;++it)
					{
						xml::XMLAttribute&attr= it->second;
						core::string_to_char(attr.name,name);
						name.make_lower();
						if(name=="value"){
							linearLimitDesc.value=core::StringConverter::toFloat(attr.value);
						}else if(name=="restitution"){
							linearLimitDesc.restitution=core::StringConverter::toFloat(attr.value);
						}else if(name=="spring"){
							linearLimitDesc.spring=core::StringConverter::toFloat(attr.value);
						}else if(name=="damping"){
							linearLimitDesc.damping=core::StringConverter::toFloat(attr.value);
						}
					}
				}else if(elemName=="swing1limit")
				{
					it= e->getAttributesBegin();
					end= e->getAttributesEnd();
					for(;it!=end;++it)
					{
						xml::XMLAttribute&attr= it->second;
						core::string_to_char(attr.name,name);
						name.make_lower();
						if(name=="low"){
							swing1Limit.low=core::StringConverter::toFloat(attr.value);
						}else if(name=="high"){
							swing1Limit.high=core::StringConverter::toFloat(attr.value);
						}else if(name=="restitution"){
							swing1Limit.restitution=core::StringConverter::toFloat(attr.value);
						}else if(name=="spring"){
							swing1Limit.spring=core::StringConverter::toFloat(attr.value);
						}else if(name=="damping"){
							swing1Limit.damping=core::StringConverter::toFloat(attr.value);
						}
					}
				}else if(elemName=="swing2limit")
				{
					it= e->getAttributesBegin();
					end= e->getAttributesEnd();
					for(;it!=end;++it){
						xml::XMLAttribute&attr= it->second;
						core::string_to_char(attr.name,name);
						name.make_lower();
						if(name=="low"){
							swing2Limit.low=core::StringConverter::toFloat(attr.value);
						}else if(name=="high"){
							swing2Limit.high=core::StringConverter::toFloat(attr.value);
						}else if(name=="restitution"){
							swing2Limit.restitution=core::StringConverter::toFloat(attr.value);
						}else if(name=="spring"){
							swing2Limit.spring=core::StringConverter::toFloat(attr.value);
						}else if(name=="damping"){
							swing2Limit.damping=core::StringConverter::toFloat(attr.value);
						}
					}
				}else if(elemName=="twistlowlimit")
				{
					it= e->getAttributesBegin();
					end= e->getAttributesEnd();
					for(;it!=end;++it){
						xml::XMLAttribute&attr= it->second;
						core::string_to_char(attr.name,name);
						name.make_lower();
						if(name=="value"){
							twistLimit.low.value=core::StringConverter::toFloat(attr.value);
						}else if(name=="restitution"){
							twistLimit.low.restitution=core::StringConverter::toFloat(attr.value);
						}else if(name=="spring"){
							twistLimit.low.spring=core::StringConverter::toFloat(attr.value);
						}else if(name=="damping"){
							twistLimit.low.damping=core::StringConverter::toFloat(attr.value);
						}
					}
				}else if(elemName=="twisthighlimit")
				{
					it= e->getAttributesBegin();
					end= e->getAttributesEnd();
					for(;it!=end;++it){
						xml::XMLAttribute&attr= it->second;
						core::string_to_char(attr.name,name);
						name.make_lower();
						if(name=="value"){
							twistLimit.high.value=core::StringConverter::toFloat(attr.value);
						}else if(name=="restitution"){
							twistLimit.high.restitution=core::StringConverter::toFloat(attr.value);
						}else if(name=="spring"){
							twistLimit.high.spring=core::StringConverter::toFloat(attr.value);
						}else if(name=="damping"){
							twistLimit.high.damping=core::StringConverter::toFloat(attr.value);
						}
					}
				}
	
			}
	
	
		}
}
}