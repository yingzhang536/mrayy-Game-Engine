

#ifndef GUIProfilePicture_h__
#define GUIProfilePicture_h__

#include "GUIStaticImage.h"

namespace mray
{

namespace GUI
{


class GUIProfilePicture :public GUIStaticImage
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

public:
	GUIProfilePicture(IGUIManager* m);
	virtual ~GUIProfilePicture();

	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUIProfilePicture);

}
}

#endif // GUIProfilePicture_h__
