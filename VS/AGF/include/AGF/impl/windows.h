#ifndef AGF_IMPL_WINDOWS_H
#define AGF_IMPL_WINDOWS_H


// #define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WIN8
#define GDIPVER 0x110

#if defined(UNICODE) || defined(_UNICODE)
#error "This project requires unicode to be disabled"
#endif

#include <Windows.h>


#endif // AGF_IMPL_WINDOWS_H