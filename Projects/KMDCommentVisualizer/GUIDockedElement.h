


#ifndef GUIDockedElement_h__
#define GUIDockedElement_h__



namespace mray
{
namespace GUI
{
	class IGUIElement;
class GUIDockedElement
{
protected:

	IGUIElement* m_left;
	IGUIElement* m_right;
	IGUIElement* m_top;
	IGUIElement* m_bottom;

	IGUIElement* m_target;
public:
	GUIDockedElement(IGUIElement* target);
	virtual ~GUIDockedElement();

	void SetDockLeft(GUI::IGUIElement* e){ m_left = e; }
	void SetDockRight(GUI::IGUIElement* e){ m_right = e; }
	void SetDockTop(GUI::IGUIElement* e){ m_top = e; }
	void SetDockBottom(GUI::IGUIElement* e){ m_bottom = e; }

	virtual void UpdateDocking();
	
};

}
}

#endif // GUIDockedElement_h__
