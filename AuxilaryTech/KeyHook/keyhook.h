#ifndef _KEYHOOK
#define _KEYHOOK

#include <windows.h>
#define DLL_EXPORT __declspec(dllexport)

DLL_EXPORT void SetKbHook(void);
DLL_EXPORT void RemoveKbHook(void);

#endif