
/********************************************************************
	created:	2009/07/29
	created:	29:7:2009   19:08
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\BaseFont.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	BaseFont
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___BaseFont___
#define ___BaseFont___


#include "IFont.h"
#include "IStream.h"
#include "TextureUnit.h"
#include "IVideoDevice.h"

namespace mray{
namespace GUI{

	class LanguageFontSpecification;
	//class GUIMultiQuadRenderOperation;

class MRAY_DLL BaseFont:public IFont
{
private:
protected:

	std::vector<video::TextureUnitPtr>  m_textures;
	core::string attrFileName;
	struct SCharAttr
	{
		uchar texID;
		uint c;
		math::rectf texcoords;
		math::vector2d rectSize;
		bool operator < (const SCharAttr &o)const{return c<o.c;}
	};

	class CharacterRange
	{
		ushort m_min;
		ushort m_max;
		std::vector<SCharAttr> m_characters;
	public:

		CharacterRange()
		{
			m_min=0;
			m_max=0;
		}

		ushort GetMin()const{return m_min;}
		ushort GetMax()const{return m_max;}
		void SetMinMax(int min,int max)
		{
			m_min=min;
			m_max=max;
			if(m_min>m_max)
			{
				m_min=m_max=0;
				return;
			}
			m_characters.resize(m_max-m_min+1);
		}

		inline bool IsInRange(uint c){return (c>=m_min && c<=m_max);}
		inline SCharAttr* GetCharacterInfo(uint c){
			if(c<m_min || c>m_max)
				return 0;
			return &m_characters[c-m_min];
		}

		void LoadFromXml(xml::XMLElement* e);

	};

	//typedef std::map<ushort,SCharAttr> CharAttrMap; 
	typedef std::vector<CharacterRange*> CharacterRangesList;
	CharacterRangesList m_charsAttr;
/*
	std::vector<math::rectf> m_texRects;
	std::vector<math::vector2d> m_rectsSize;*/
	std::vector<uint> m_tmpIndices;
	std::vector<uint> m_printBuffer;

	math::Point2df CharSZ;

	virtual uint calcSizeInternal();

	SCharAttr* GetCharacterInfo(uint c);

	//GUIMultiQuadRenderOperation*m_renderOperation;


	void print(const math::rectf& pos,FontAttributes*attributs,const  uint* txt,IGUIRenderer*renderer);
public:
	BaseFont(const core::string&name);
	virtual~BaseFont();

	const video::TextureUnitPtr&  getTexture(uint i);

	virtual void print(const math::rectf& pos,FontAttributes*attributs,const math::rectf*clip,const core::UTFString& txt,IGUIRenderer*renderer);

	virtual int getCharFromPos(const math::vector2d& p,FontAttributes*attributs,const math::rectf &rc,const core::UTFString& txt);
	virtual math::Point2df getCharPos(int len,FontAttributes*attributs,const math::rectf &rc,const core::UTFString& txt);
	virtual math::Point2df getTextDimension(const  core::UTFString& txt,FontAttributes*attributs,const math::rectf *rc);
	virtual math::Point2df getCharDimension( uint c,int size);

};

}
}


#endif //___BaseFont___
