

#include "stdafx.h"
#include "DataTypes.h"

#include <windows.h>

namespace mray
{
namespace core
{

void DateTime::Parse(const core::string& str)
{
}

core::stringw ConvertToStringW(const char* multibyte, int cnt)
{
	wchar_t buffer[1024];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,multibyte, cnt + 1, buffer, 1024);
	return buffer;

}

}
}
