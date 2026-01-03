

#include <Windows.h>
#include <Windowsx.h>
#include "SDL_main.h"
#include "SDL_syswm.h"

#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"

static WNDPROC  oldProc;
static HICON    icon;
extern int MainLoopTics;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_INPUT: // Raw input (optional)
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		{
			if (MainLoopTics > 10)
			{
				//End screensaver whenever any input is received
                exit(0);

			}
		}

        //{
        //    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        //    break;
        //}


        default:
            break;
    }

    if (msg == WM_SETCURSOR)
    {
        if (LOWORD(lParam) == HTCLIENT && !MouseShouldBeGrabbed())
        {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
    }
    else if (msg == WM_SYSCOMMAND)
    {
        if ((wParam & 0xFFF0) == SC_MAXIMIZE)
        {
            I_ToggleFullscreen(true);
            return true;
        }
        else if ((wParam & 0xFFF0) == SC_KEYMENU)
            return false;
    }
    else if (msg == WM_SYSKEYDOWN)
    {
        if (wParam == VK_RETURN && !(lParam & 0x40000000))
        {
            I_ToggleFullscreen(true);
            return true;
        }
    }
    else if (msg == WM_SIZE)
    {
        if (!vid_fullscreen)
            I_WindowResizeBlit();
    }
    else if (msg == WM_GETMINMAXINFO)
    {
        LPMINMAXINFO    minmaxinfo = (LPMINMAXINFO)lParam;

        minmaxinfo->ptMinTrackSize.x = (vid_widescreen ? WIDEVANILLAWIDTH : VANILLAWIDTH) + windowborderwidth;
        minmaxinfo->ptMinTrackSize.y = ACTUALVANILLAHEIGHT + windowborderheight;

        return false;
    }

    return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
}

static HANDLE       hInstanceMutex;

static STICKYKEYS   g_StartupStickyKeys = { sizeof(STICKYKEYS), 0 };
static TOGGLEKEYS   g_StartupToggleKeys = { sizeof(TOGGLEKEYS), 0 };
static FILTERKEYS   g_StartupFilterKeys = { sizeof(FILTERKEYS), 0 };

static void I_AccessibilityShortcutKeys(bool bAllowKeys)
{
    if (bAllowKeys)
    {
        // Restore StickyKeys/etc to original state
        SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
        SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_StartupToggleKeys, 0);
        SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);
    }
    else
    {
        // Disable StickyKeys/etc shortcuts
        STICKYKEYS  skOff = g_StartupStickyKeys;
        TOGGLEKEYS  tkOff = g_StartupToggleKeys;
        FILTERKEYS  fkOff = g_StartupFilterKeys;

        if (!(skOff.dwFlags & SKF_STICKYKEYSON))
        {
            // Disable the hotkey and the confirmation
            skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
            skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
        }

        if (!(tkOff.dwFlags & TKF_TOGGLEKEYSON))
        {
            // Disable the hotkey and the confirmation
            tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
            tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
        }

        if (!(fkOff.dwFlags & FKF_FILTERKEYSON))
        {
            // Disable the hotkey and the confirmation
            fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
            fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
        }
    }
}

void I_InitWindows32(void)
{
    HINSTANCE       handle = GetModuleHandle(NULL);
    SDL_SysWMinfo   info = { 0 };
    HWND            hwnd;

    SDL_VERSION(&info.version);

    SDL_GetWindowWMInfo(window, &info);
    hwnd = info.info.win.window;

    icon = LoadIcon(handle, "IDI_ICON1");
    SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)icon);

    oldProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

    windowborderwidth = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)) * 2;
    windowborderheight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)) * 2
        + GetSystemMetrics(SM_CYCAPTION);
}

void I_ShutdownWindows32(void)
{
    DestroyIcon(icon);
    ReleaseMutex(hInstanceMutex);
    CloseHandle(hInstanceMutex);
    I_AccessibilityShortcutKeys(true);
}

int SDL_main(int argc, char* argv[])
{
    myargc = argc;

    if ((myargv = (char **)malloc(myargc * sizeof(myargv[0]))))
    {
        memcpy(myargv, argv, myargc * sizeof(myargv[0]));

        for (int i = 0; i < myargc; i++)
            M_NormalizeSlashes(myargv[i]);
    }

    M_FindResponseFile();

    hInstanceMutex = CreateMutex(NULL, true, DOOMRETRO_MUTEX);

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (hInstanceMutex)
            CloseHandle(hInstanceMutex);

        SetForegroundWindow(FindWindow(DOOMRETRO_MUTEX, NULL));
        return 1;
    }

    // Save the current sticky/toggle/filter key settings so they can be restored later
    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
    SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_StartupToggleKeys, 0);
    SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);

    I_AccessibilityShortcutKeys(false);

    D_DoomMain();

    return 0;
}
