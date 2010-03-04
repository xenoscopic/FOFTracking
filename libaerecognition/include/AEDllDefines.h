#pragma once

#ifdef _WIN32
#	ifdef aerecognition_EXPORTS
#		define aerecognition_EXPORT __declspec(dllexport)
#	else
#		define aerecognition_EXPORT __declspec(dllimport)
#	endif
#else
#	define aerecognition_EXPORT
#endif
