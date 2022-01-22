#ifndef __NEKODLL_H
#define __NEKODLL_H

#ifdef LIBNWIN_EXPORT_LIBRARY
	#if defined(LIBNWIN_STATIC)
		#define LIBNWIN_API 
	#elif defined(__GNUC__)
		#define LIBNWIN_API __attribute__ ((dllexport))
	#elif defined (_MSC_BUILD)
		#define LIBNWIN_API __declspec(dllexport)
	#endif
#else
	#if defined(LIBNWIN_STATIC)
		#define LIBNWIN_API
	#elif defined(__GNUC__)
		#define GLAD_GLAPI_EXPORT
		#define LIBNWIN_API __attribute__ ((dllimport))
	#elif defined(_MSC_BUILD)
		#define GLAD_GLAPI_EXPORT
		#define LIBNWIN_API __declspec(dllimport)
	#endif
#endif

#endif
