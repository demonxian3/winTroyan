#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int nChmdShow){
    MSG msg;

    char text[] = "keyhook.dll load error";
    char title[] = "keyhook";

    BOOL error = FALSE;
    HINSTANCE dllhinst; 

    typedef VOID (CALLBACK * LPFNDLLFUNC)(VOID);
    LPFNDLLFUNC installHook;

    dllhinst = LoadLibrary("keyhook.dll");

    if(dllhinst != NULL){
        installHook = (LPFNDLLFUNC)GetProcAddress(dllhinst, "SetKbHook");
        if(!installHook){
            FreeLibrary(dllhinst);
            error = TRUE;
        }else
            installHook();
        
    }else
        error = TRUE;

    if(error){
        MessageBox(GetDesktopWindow(), text, title, MB_OK);
        return 0;
    }

    while(GetMessage(&msg, 0, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}
