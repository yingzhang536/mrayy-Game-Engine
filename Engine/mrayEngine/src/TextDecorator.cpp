#include "stdafx.h"

#include "TextDecorator.h"

#include "TextDecorateNodeFactory.h"
#include "TextDecorateNode.h"


namespace mray
{
namespace GUI
{



//////////////////////////////////////////////////////////////////////////


TextDecorator::TextDecorator()
{
}
TextDecorator::~TextDecorator()
{
}
TextDecorator::EPredictionResult TextDecorator::GetPrediction(const mchar*ptr)
{
	if(*ptr=='<')
	{
		ptr++;
		if(!*ptr)
			return ENormalText;
		if(*ptr=='/')
			return ECloseTag;
		return EOpenTag;
	}
	return ENormalText;
}

const mchar* TextDecorator::_ParserText(const mchar*ptr,ITextDecorateNode* parent)
{

	enum EState
	{
		ENormal,
		ETagOpen,
		ETagClose,
		ETagEnder,
		EInStartTag,
		EInEndTag
	};

	EState state=ENormal;

	bool tagStart=false;
	bool tagEnd=false;
	core::string tagName;
	core::string tagValue;
	core::string text;
	bool readingValue=false;
	bool skipChar=false;

	ITextDecorateNode* node=0;

	while(*ptr)
	{
		mchar c=*ptr;
		if(!skipChar)
		{
			switch (c)
			{
			case '<':
				{
					EPredictionResult r=GetPrediction(ptr);
					if(r!=ENormalText && text!=mT(""))
					{
						TextDecorateNode*d=new TextDecorateNode(text);
						parent->AddChild(d);
						text=mT("");
					}
					if(r==EOpenTag)
					{
						state=ETagOpen;
						if(!tagStart)
							tagStart=true;
						tagName=mT("");
						tagValue=mT("");
						readingValue=false;
					}else if(r==ECloseTag)
					{
						++ptr;
						state=EInEndTag;
						if(!tagEnd)
							tagEnd=true;
						tagName=mT("");
					}else
						state=ENormal;
				}
				break;
			case '>':
				if(state==EInEndTag)
				{
					return ptr;
				}else if(state==EInStartTag)
				{
					//create Node
					node=TextDecorateNodeFactory::getInstance().CreateNode(tagName);
					node->ParseParameter(tagValue);
					if(parent)
						parent->AddChild(node);

					ptr=_ParserText(ptr+1,node);
				}
				state=ETagClose;
				break;
			case '\\':
				if(state==ENormal && !skipChar){
					skipChar=true;
					continue;
				}
				break;
			case '=':
				if(state==EInStartTag){
					readingValue=true;
				}
				break;
			default:
				if(state==ETagOpen)
					state=EInStartTag;

				if(state==EInStartTag ){
					if(!readingValue)
						tagName+=c;
					else tagValue+=c;
				}else if(state==EInEndTag)
				{
					tagName+=c;
				}else{
					state=ENormal;
				}

			}
		}
		if(state==ENormal)
			text+=c;
		skipChar=false;

		++ptr;
	}
	return ptr;
}

ITextDecorateNode* TextDecorator::ParseText(const core::string&text)
{
	const mchar*ptr=text.c_str();

	TextDecorateDummyNode* root=new TextDecorateDummyNode(mT("Root"));

	_ParserText(ptr,root);
	return root;
}

}
}
