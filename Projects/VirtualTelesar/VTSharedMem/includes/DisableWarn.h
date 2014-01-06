
/********************************************************************
	created:	2009/05/12
	created:	12:5:2009   21:54
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\DisableWarn.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	DisableWarn
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___DisableWarn___
#define ___DisableWarn___
// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4786)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4503)

// disable: "conversion from 'double' to 'float', possible loss of data
#   pragma warning (disable : 4244)

// disable: "truncation from 'double' to 'float'
#   pragma warning (disable : 4305)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "C++ Exception Specification ignored"
// This is because MSVC 6 did not implement all the C++ exception
// specifications in the ANSI C++ draft.
#   pragma warning( disable : 4290 )

// disable: "no suitable definition provided for explicit template
// instantiation request" 
#   pragma warning( disable: 4661)

// disable: deprecation warnings when using CRT calls in VC8
// These show up on all C runtime lib code in VC8, disable since they clutter
// the warnings with things we may not be able to do anything about (e.g.
// generated code from nvparse etc). I doubt very much that these calls
// will ever be actually removed from VC anyway, it would break too much code.
#	pragma warning( disable: 4996)

// disable: "conditional expression constant"
#   pragma warning (disable : 201)

#   pragma warning (disable : 4251)


#endif //___DisableWarn___