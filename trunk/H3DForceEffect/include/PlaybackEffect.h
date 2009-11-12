#ifndef __CUTTING_SKILLS_H__
#define __CUTTING_SKILLS_H__

// note that _WIN32 is always defined when _WIN64 is defined.
#if( defined( _WIN64 ) || defined(WIN64) )
// set when on 64 bit Windows
#define H3D_WIN64
#elif( defined( _WIN32 ) || defined(WIN32) )
// set when on 32 bit Windows
#define H3D_WIN32
#endif

#if( defined( H3D_WIN32 ) || defined( H3D_WIN64 ) )
// set when on 32 or 64 bit Windows
#define H3D_WINDOWS
#endif

// If building as a library, do additional configuration
#ifdef BUILD_AS_LIBRARY

// If on windows set up dll export/imports in the standard way
// If not on windows, define PLAYBACKEFFECT_API as empty
#ifdef H3D_WINDOWS
#include <windows.h>

#ifdef PLAYBACKEFFECT_EXPORTS
#define PLAYBACKEFFECT_API __declspec(dllexport)
#else
#define PLAYBACKEFFECT_API __declspec(dllimport)
#endif

#ifdef _MSC_VER
// disable dll-interface warnings for stl-exports 
#pragma warning( disable: 4251 )
#endif

#else // H3D_WINDOWS
#define PLAYBACKEFFECT_API
#endif

#else // BUILD_AS_LIBRARY

// Not building as library define PLAYBACKEFFECT_API as empty
#define PLAYBACKEFFECT_API

#endif // BUILD_AS_LIBRARY

#endif // Include guard