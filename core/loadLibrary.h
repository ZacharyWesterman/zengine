#pragma once
#ifndef LOADLIBRARY_H_INCLUDED
#define LOADLIBRARY_H_INCLUDED

#include "string.h"
#include <stdint.h>

#ifdef __linux__
    #error "loadLibrary.h" is incompatible with Linux.
#elif _WIN32

#include <windows.h>

namespace core
{
    namespace library
    {
        typedef HINSTANCE lib_t;

        typedef FARPROC proc_t;


        lib_t load(const string<char>& file_name)
        {
            return LoadLibrary(file_name.str());
        }

        proc_t get_function(lib_t Library, const string<char>& func_name)
        {
            return GetProcAddress(Library, func_name.str());
        }

        bool unload(lib_t Library)
        {
            return (bool)FreeLibrary(Library);
        }
    }
}
#else
    #error "loadLibrary.h" is incompatible with target OS.
#endif

#endif // LOADLIBRARY_H_INCLUDED
