

/********************************************************************
	created:	2014/04/09
	created:	9:4:2014   21:46
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\DataTypes.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	DataTypes
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __DataTypes__
#define __DataTypes__




namespace mray
{
namespace core
{

class DateTime
{
protected:
public:

	int year;
	int month;
	int day;

	DateTime():year(2014),month(1),day(1)
	{}
	virtual~DateTime(){}

	void Parse(const core::string& str);
};


core::stringw ConvertToStringW(const char* multibyte, int cnt);

}
}

#endif
