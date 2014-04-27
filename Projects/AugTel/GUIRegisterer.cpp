

#include "stdafx.h"
#include "GUIRegisterer.h"

#include "GUITypingTextBox.h"
#include "GUIElementFactory.h"
#include "GUILoadingElement.h"


namespace mray
{
namespace GUI
{

void GUIRegisterer::RegisterElements()
{
	REGISTER_GUIElement_FACTORY(GUITypingTextBox);
	REGISTER_GUIElement_FACTORY(GUILoadingElement);
}

}
}