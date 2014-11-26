// GUISchemeGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Engine.h"
#include "WinFileSystem.h"
#include "Engine.h"
#include "WinOSystem.h"

#include "GUIElementFactoryDef.h"
#include "GUIOverlay.h"
#include "SoftwareBufferManager.h"

using namespace mray;
class GenerateCHeader
{
protected:
	std::map<core::string,int> m_duplicates;
	struct ElementsIfo
	{
		core::string type;
		core::string name;
	};
	std::set<core::string> m_types;
	std::vector<ElementsIfo> m_elements;
public:
	core::string GetNodeString(xml::XMLElement* e,GUI::GUIManager*mngr)
	{
		xml::XMLAttribute* attr;
		attr=e->getAttribute("GenerateObject");
		if(attr && core::StringConverter::toBool(attr->value)==false)
			return "";

		core::string elemType=e->getName();/*
		const GUI::IGUIElementFactory* fact= mngr->GetElementFactory(elemType);
		if(!fact)
			return "";

		elemType=fact->GetElementType();*/
		m_types.insert(elemType);

		core::string elemName;

		attr=e->getAttribute("Name");
		if(attr==0)
		{
			return "";
		}else
			elemName=attr->value;
		core::string newName=elemName;
		std::map<core::string,int>::iterator it=m_duplicates.find(newName);
		if(it!=m_duplicates.end())
		{
			printf("found duplicated element name:%s\n",elemName.c_str());
			return "";
		}
		/*while(it!=m_duplicates.end())
		{
			newName=elemName+it->second;
			it->second++;
			it=m_duplicates.find(newName);
		}*/
		m_duplicates[newName]=0;
		core::string res=elemType+"* "+newName+";";
		ElementsIfo ifo;
		ifo.name=newName;
		ifo.type=elemType;
		m_elements.push_back(ifo);

		return res;

	}
	core::string VisitNode(xml::XMLElement* e,GUI::GUIManager*mngr)
	{
		core::string ret=GetNodeString(e,mngr);
		if(ret!="")
			ret="\t"+ret+"\n";

		xml::XMLElement* elem=e->getSubElement("Children");
		if(elem)
		{
			xml::xmlSubElementsMapIT it= elem->getElementsBegin();

			for(;it!=elem->getElementsEnd();++it)
			{
				if((*it)->GetType()!=xml::ENT_Element)	
					continue;
				ret+=VisitNode((xml::XMLElement*)*it,mngr);
			}
		}

		return ret;
	}
	core::string Generate(GUI::GUIOverlay*o,GUI::GUIManager*mngr)
	{

		core::string elements;

		std::list<xml::XMLElement*> elems= o->GetElements();
		for (std::list<xml::XMLElement*>::iterator it=elems.begin();it!=elems.end();++it)
		{
			elements+=VisitNode(*it,mngr);
		}


		core::string header;

		header = "#ifndef __" + o->GetName() + "__\n";
		header += "#define __" + o->GetName() + "__\n";
		header +="#include \"IGUISchemeBase.h\"\n";

		for(std::set<core::string>::iterator it=m_types.begin();it!=m_types.end();++it)
		{
			header+="#include \""+*it+".h\"\n";
		}

		header+="namespace mray{\n"\
			"\nusing namespace GUI;\n"\
			"class "+o->GetName()+":public GUI::IGUISchemeBase\n";
		header+="{\n"\
			"\npublic:\n";



		header+=elements;
		header+="\npublic:\n"\
			"\n\t"+o->GetName()+"():";

		for(int i=0;i<m_elements.size();++i)
		{
			header+=m_elements[i].name+"(0)";
			if(i<m_elements.size()-1)
				header+=",";
		}

		header+="\n\t{\t\t\n";
		for(int i=0;i<m_elements.size();++i)
		{
			core::string str;
			str="m_elementsMap[\""+m_elements[i].name+"\"]=(IGUIElement**)&"+m_elements[i].name;
			header+="\t\t"+str+";\n";
		}

		header+="\n\t}\n";

		header+="\n};\n}\n";
		header += "#endif\n";

		return header;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{

	if(argc<2)
	{
		printf("Usage: %s [overlay name] [targetPath]\n",argv[0]);
		return 0;
	}
	core::string basePath="";
	core::string fileName;
	if(argc==3)
	{
		basePath=argv[2];
		if(basePath.length()>0 && basePath[basePath.length()-1]!='\\')
			basePath+="\\";
	}else
	{
		basePath=argv[1];
		int i=basePath.findlast('\\');
		if(i!=-1)
		{
			basePath=basePath.substr(0,i+1);
		}
	}
	{
		core::string tmp=argv[1];
		int i=tmp.findlast('\\');
		if(i!=-1)
		{
			tmp=tmp.substr(i+1,tmp.length()-i);
		}
		fileName=basePath+tmp;
	}
	new OS::WinFileSystem();
	new Engine(new OS::WinOSystem());
	new video::SoftwareBufferManager();

	GUI::GUIManager* mngr=new GUI::GUIManager(0);

	GUI::GUIOverlay* o=GUI::GUIOverlayManager::getInstance().LoadOverlay(argv[1]);
	if(!o)
	{
		printf("Failed to load overlay file\n");
	}else
	{
		GenerateCHeader g;
		core::string ret=g.Generate(o,mngr);
		int i=fileName.findlast('.');
		if(i==-1)
			fileName=fileName+".h";
		else
		{
			fileName=fileName.substr(0,i)+".h";
		}

		OS::IStreamPtr stream=gFileSystem.openFile(fileName,OS::BIN_WRITE);
		OS::StreamWriter wrt;
		wrt.setStream(stream);
		wrt.writeString(ret);
		stream->close();
	}


	delete Engine::getInstancePtr();
	return 0;
}

