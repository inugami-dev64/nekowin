#ifndef __NEKODLL_H
#define __NEKODLL_H

#ifdef LIBNWIN_EXPORT_LIBRARY

    #if defined(LIBNWIN_STATIC)
        #define LIBNWIN_API
    #else
        #define GLAD_GLAPI_EXPORT
        #define GLAD_GLAPI_EXPORT_BUILD
        #if defined(WIN32) || defined(__CYGWIN__)
            #if defined(__GNUC__)
                #define LIBNWIN_API __attribute__ ((dllexport))
            #elif defined (_MSC_BUILD)
                #define LIBNWIN_API __declspec(dllexport)
            #endif
        #else
            #if defined(__GNUC__)
                #define LIBNWIN_API __attribute__ ((visibility ("default")))
            #else       
                #define LIBNWIN_API
            #endif
        #endif  
    #endif
#else
    #if defined(LIBNWIN_STATIC)
        #define LIBNWIN_API
    #else
        #define GLAD_GLAPI_EXPORT
        #if defined(WIN32) || defined(__CYGWIN__)
            #if defined(__GNUC__)
                #define LIBNWIN_API __attribute__ ((dllimport))
            #else
                #define LIBNWIN_API __declspec(dllimport)
            #endif
        #else
            #define LIBNWIN_API
        #endif
    #endif
#endif

#endif
