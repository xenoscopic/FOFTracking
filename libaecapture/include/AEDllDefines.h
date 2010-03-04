#pragma once

#ifdef _WIN32
#	ifdef aecapture_EXPORTS
#		define aecapture_EXPORT __declspec(dllexport)
#	else
#		define aecapture_EXPORT __declspec(dllimport)
#	endif
#else
#	define aecapture_EXPORT
#endif
