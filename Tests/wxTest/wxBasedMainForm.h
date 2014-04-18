#ifndef __wxBasedMainForm__
#define __wxBasedMainForm__

/**
@file
Subclass of MainForm, which is generated by wxFormBuilder.
*/

#include "wxEngineEditor.h"
#include "wxRenderWindow.h"

#include "wxMRAYPropGrid.h"
#include <wx/aui/framemanager.h>

using namespace mray;
/** Implementing MainForm */
class wxBasedMainForm : public MainForm
{
protected:
	// Handlers for MainForm events.
	void OnNewFile( wxCommandEvent& event );
	void OnOpenFile( wxCommandEvent& event );
	void OnSaveFile( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );

	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnKeyUp(wxKeyEvent& event);

	void _FillKeyEvent(mray::KeyboardEvent* dst,wxKeyEvent& src);
	DECLARE_EVENT_TABLE()

	wxRenderWindow* m_renderWindow;
	wxMRAYPropGrid* m_propManager;
	wxAuiManager *m_auimanager;

	void InitPropGrid();


public:
	/** Constructor */
	wxBasedMainForm( wxWindow* parent );
};

#endif // __wxBasedMainForm__
