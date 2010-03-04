#pragma once

#ifdef _WIN32
#	ifdef aedesktop_EXPORTS
#		define aedesktop_EXPORT __declspec(dllexport)
#	else
#		define aedesktop_EXPORT __declspec(dllimport)
#	endif
#else
#	define aedesktop_EXPORT
#endif
