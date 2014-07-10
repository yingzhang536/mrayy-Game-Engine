

/********************************************************************
	created:	2012/07/30
	created:	30:7:2012   0:19
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTSharedMem\CompileConfig.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTSharedMem
	file base:	CompileConfig
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___CompileConfig___
#define ___CompileConfig___


#ifdef VTSHAREDMEM_EXPORT
#define VTSHAREDMEM_DLL __declspec(dllexport)
#else
#define VTSHAREDMEM_DLL __declspec(dllimport)

#endif



#endif

