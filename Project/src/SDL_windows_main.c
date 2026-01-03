/*
    SDL_windows_main.c, placed in the public domain by Sam Lantinga  4/13/98

    The WinMain function -- calls your program's main() function
*/
//#define _CRT_SECURE_NO_WARNINGS
#include"..\msvc\resource.h"

#include "SDL_config.h"

#ifdef __WIN32__

/* Include this so we define UNICODE properly */
#include "SDL_windows.h"
#include <shellapi.h> /* CommandLineToArgvW() */

/* Include the SDL main definition header */
#include "SDL.h"
#include "SDL_main.h"
#include "m_fixed.h"
#include"m_config.h"
#pragma warning(disable: 5287)
#pragma warning(disable: 4101)

#include "nuklear.h"


#ifdef main
#undef main
#endif /* main */

/* Pop up an out of memory message, returns to Windows */
static BOOL OutOfMemory(void)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "Out of memory - aborting", NULL);
    return FALSE;
}

#if defined(_MSC_VER)
/* The VC++ compiler needs main/wmain defined */
#define console_ansi_main main
#if UNICODE
#define console_wmain wmain
#endif
#endif

//Faces for the config window
static HBITMAP g_hFaceBmp_1 = NULL;
static HBITMAP g_hFaceBmp_2 = NULL;
static HBITMAP g_hFaceBmp_3 = NULL;
static HBITMAP g_hFaceBmp_4 = NULL;
static HBITMAP g_hFaceBmp_5 = NULL;

int Registry_AudioEnabled = 1;
int Registry_MusicLevel = 100;
int Registry_SFXLevel = 100;

HANDLE       hScreenSaverMutex;


void LoadSettingsFromRegistry(BOOL* AudioEnabled, int* MusicLevel, int* SFXLevel)
{
    HKEY hKey = NULL;

    LONG result = RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\DukeDoom\\DoomScreenSaver",
        0,
        KEY_READ,
        &hKey
    );

    if (result != ERROR_SUCCESS)
    {
        return; // key doesn't exist -> keep defaults
    }

    DWORD dwType;
    DWORD dwSize;
    DWORD dwValue;

    // --- AudioEnabled ---
    dwSize = sizeof(DWORD);
    if (RegQueryValueExA(
        hKey,
        "AudioEnabled",
        NULL,
        &dwType,
        (LPBYTE)&dwValue,
        &dwSize
    ) == ERROR_SUCCESS && dwType == REG_DWORD)
    {
        if (AudioEnabled)
            *AudioEnabled = (dwValue != 0);
    }

    // --- MusicLevel ---
    dwSize = sizeof(DWORD);
    if (RegQueryValueExA(
        hKey,
        "MusicLevel",
        NULL,
        &dwType,
        (LPBYTE)&dwValue,
        &dwSize
    ) == ERROR_SUCCESS && dwType == REG_DWORD)
    {
        if (MusicLevel)
            *MusicLevel = (int)dwValue;
    }

    // --- SFXLevel ---
    dwSize = sizeof(DWORD);
    if (RegQueryValueExA(
        hKey,
        "SFXLevel",
        NULL,
        &dwType,
        (LPBYTE)&dwValue,
        &dwSize
    ) == ERROR_SUCCESS && dwType == REG_DWORD)
    {
        if (SFXLevel)
            *SFXLevel = (int)dwValue;
    }

    RegCloseKey(hKey);

    if (Registry_MusicLevel == 0 && Registry_SFXLevel == 0)
    {
        Registry_AudioEnabled = FALSE;
    }

	s_musicvolume = BETWEEN(0, Registry_MusicLevel, 100);
	s_sfxvolume = BETWEEN(0, Registry_SFXLevel, 100);
}


void SaveSettingsToRegistry(BOOL AudioEnabled, int MusicLevel, int SFXLevel)
{
    HKEY hKey = NULL;
    DWORD dwDisposition;

    LONG result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        "Software\\DukeDoom\\DoomScreenSaver",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        &dwDisposition
    );

    if (result != ERROR_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Couldn't Save Settings to Registry", NULL);

        return;
    }

    MusicLevel = BETWEEN(0, MusicLevel, 100);
    SFXLevel = BETWEEN(0, SFXLevel, 100);

    DWORD audioEnabledValue = AudioEnabled ? 1 : 0;
    DWORD musicLevelValue = (DWORD)MusicLevel;
    DWORD sfxLevelValue = (DWORD)SFXLevel;

    RegSetValueExA(
        hKey,
        "AudioEnabled",
        0,
        REG_DWORD,
        (const BYTE*)&audioEnabledValue,
        sizeof(DWORD)
    );

    RegSetValueExA(
        hKey,
        "MusicLevel",
        0,
        REG_DWORD,
        (const BYTE*)&musicLevelValue,
        sizeof(DWORD)
    );

    RegSetValueExA(
        hKey,
        "SFXLevel",
        0,
        REG_DWORD,
        (const BYTE*)&sfxLevelValue,
        sizeof(DWORD)
    );

    RegCloseKey(hKey);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Done!", "Settings Saved to Registry Successfully", NULL);

}

/* Gets the arguments with GetCommandLine, converts them to argc and argv
   and calls SDL_main */
static int main_getcmdline(void)
{
    LPWSTR *argvw;
    char **argv;
    int i, argc, result;

    argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argvw) {
        return OutOfMemory();
    }

    /* Note that we need to be careful about how we allocate/free memory here.
     * If the application calls SDL_SetMemoryFunctions(), we can't rely on
     * SDL_free() to use the same allocator after SDL_main() returns.
     */

    /* Parse it into argv and argc */
    argv = (char **)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (argc + 1) * sizeof(*argv));
    if (!argv) {
        return OutOfMemory();
    }
    for (i = 0; i < argc; ++i) {
        DWORD len;
        char *arg = WIN_StringToUTF8W(argvw[i]);
        if (!arg) {
            return OutOfMemory();
        }
        len = (DWORD)SDL_strlen(arg);
        argv[i] = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size_t)len + 1);
        if (!argv[i]) {
            return OutOfMemory();
        }
        SDL_memcpy(argv[i], arg, len);
        SDL_free(arg);
    }
    argv[i] = NULL;
    LocalFree(argvw);

    SDL_SetMainReady();

    /* Run the application main() code */
    result = SDL_main(argc, argv);

    /* Free argv, to avoid memory leak */
    for (i = 0; i < argc; ++i) {
        HeapFree(GetProcessHeap(), 0, argv[i]);
    }
    HeapFree(GetProcessHeap(), 0, argv);

    return result;
}

/* This is where execution begins [console apps, ansi] */
int console_ansi_main(int argc, char *argv[])
{
    return main_getcmdline();
}

#if UNICODE
/* This is where execution begins [console apps, unicode] */
int console_wmain(int argc, wchar_t *wargv[], wchar_t *wenvp)
{
    return main_getcmdline();
}
#endif

#define CONFIG_WINDOW_WIDTH 800
#define CONFIG_WINDOW_HEIGHT 600

//config window proc
static LRESULT CALLBACK NukeWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}


int LaunchConfigWindow()
{
    GdiFont* font;
    struct nk_context* ctx;
    DWORD MyStyles = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_BORDER;
    WNDCLASSW wc;
    ATOM atom;
    RECT rect = { 0, 0, CONFIG_WINDOW_WIDTH, CONFIG_WINDOW_HEIGHT };
    //DWORD style = WS_OVERLAPPEDWINDOW;
    //DWORD style = WS_POPUPWINDOW;
    DWORD exstyle = WS_EX_TOOLWINDOW | WS_EX_APPWINDOW;
    HWND wnd;
    HDC dc;
    int running = 1;
    int needs_refresh = 1;


    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = NukeWindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    atom = RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, MyStyles, FALSE, exstyle);
    wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"DOOM Screen Saver Settings V(1.0)",
        MyStyles, CW_USEDEFAULT, CW_USEDEFAULT,
        CONFIG_WINDOW_WIDTH, CONFIG_WINDOW_HEIGHT, GetForegroundWindow(), 
        NULL, wc.hInstance, NULL);
    dc = GetDC(wnd);
    //GetWindowRect(wnd, &rect);
    /* GUI */
    font = nk_gdifont_create("Arial", 24);
    ctx = nk_gdi_init(font, dc, CONFIG_WINDOW_WIDTH, CONFIG_WINDOW_HEIGHT);
    set_style(ctx, THEME_RED);

    enum
    {
        SOUND_DISABLED, SOUND_ENABLED
    };

    int SoundStatus = (Registry_AudioEnabled == 0) ? SOUND_DISABLED : SOUND_ENABLED;
    int MusicVolumeSlider = Registry_MusicLevel;
    int SFXVolumeSlider = Registry_SFXLevel;

    while (running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (needs_refresh == 0)
        {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                running = 0;
            else
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = 1;
        }
        else
            needs_refresh = 0;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = 1;
        }
        nk_input_end(ctx);

        RECT clientRect;
        GetClientRect(wnd, &clientRect);
        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;

        /* GUI */
        if (nk_begin(ctx, "DOOM", nk_rect(0, 0, (float)clientWidth, (float)clientHeight), NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 30, 3);
            nk_label(ctx, "Sound: ", NK_TEXT_LEFT);
            if (nk_option_label(ctx, "Enable", SoundStatus == SOUND_ENABLED))
                SoundStatus = SOUND_ENABLED;
            if (nk_option_label(ctx, "Disable", SoundStatus == SOUND_DISABLED))
                SoundStatus = SOUND_DISABLED;

            if (SoundStatus == SOUND_DISABLED)
            {
                nk_widget_disable_begin(ctx);
            }
            nk_layout_row_dynamic(ctx, 2, 1);
            nk_rule_horizontal(ctx, nk_white, nk_true);

            nk_layout_row_begin(ctx, NK_DYNAMIC, 50, 3);
            nk_layout_row_push(ctx, 0.2f);
            nk_label(ctx, "Music Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 0.6f);
            nk_slider_int(ctx, 0, &MusicVolumeSlider, 100, 1);
            nk_layout_row_push(ctx, 0.2f);
            nk_labelf(ctx, NK_TEXT_LEFT, "%d%%", MusicVolumeSlider);
            nk_layout_row_end(ctx);

            nk_spacing(ctx, 1);
            nk_layout_row_dynamic(ctx, 2, 1);
            nk_rule_horizontal(ctx, nk_white, nk_true);

            nk_layout_row_begin(ctx, NK_DYNAMIC, 50, 3);
            nk_layout_row_push(ctx, 0.2f);
            nk_label(ctx, "SFX Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 0.6f);
            nk_slider_int(ctx, 0, &SFXVolumeSlider, 100, 1);
            nk_layout_row_push(ctx, 0.2f);
            nk_labelf(ctx, NK_TEXT_LEFT, "%d%%", SFXVolumeSlider);
            nk_layout_row_end(ctx);
            if (SoundStatus == SOUND_DISABLED)
            {
                nk_widget_disable_end(ctx);
            }

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 50, 2);
            if (nk_button_label(ctx, "CANCEL"))
                break;
			if (nk_button_label(ctx, "APPLY"))
			{
				SaveSettingsToRegistry((SoundStatus == SOUND_ENABLED) ? TRUE : FALSE, MusicVolumeSlider, SFXVolumeSlider);
				Registry_MusicLevel = MusicVolumeSlider;
				Registry_SFXLevel = SFXVolumeSlider;
				Registry_AudioEnabled = (SoundStatus == SOUND_ENABLED);

				break;
			}

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "Developed by Wiaam Suleiman ( JJZ8422@outlook.com )", NK_TEXT_CENTERED);
            nk_label(ctx, "www.github.com/Zayno", NK_TEXT_CENTERED);
            nk_label(ctx, "", NK_TEXT_CENTERED);
            nk_label(ctx, "Based on DOOM RETRO project", NK_TEXT_CENTERED);
            nk_label(ctx, "WWW.DOOMRETRO.COM", NK_TEXT_CENTERED);

        }
        nk_end(ctx);

        nk_gdi_render(nk_rgb(0, 0, 0));
    }


    nk_gdifont_del(font);
    ReleaseDC(wnd, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    nk_gdi_shutdown();

    CloseHandle(hScreenSaverMutex);

    return 0;
}

//needed for parsing window handle from command line in Preview mode
HWND ParseWindowHandle(const char* str)
{
    const char* p = str;
    ULONG_PTR value = 0;

    // Skip leading whitespace
    while (*p == ' ')
        p++;

    // Expect "/p"
    if (*p++ != '/')
        return NULL;
    if (*p != 'p' && *p != 'P')
        return NULL;
    p++;

    // Skip whitespace between /p and number
    while (*p == ' ')
        p++;

    // Parse digits
    if (*p < '0' || *p > '9')
        return NULL;

    while (*p >= '0' && *p <= '9')
    {
        value = value * 10 + (*p - '0');
        p++;
    }

    return (HWND)value;
}

void CleanupFacesBmp()
{
    if (g_hFaceBmp_1)
    {
        DeleteObject(g_hFaceBmp_1);
        g_hFaceBmp_1 = NULL;
    }

    if (g_hFaceBmp_2)
    {
        DeleteObject(g_hFaceBmp_2);
        g_hFaceBmp_2 = NULL;
    }

    if (g_hFaceBmp_3)
    {
        DeleteObject(g_hFaceBmp_3);
        g_hFaceBmp_3 = NULL;
    }

    if (g_hFaceBmp_4)
    {
        DeleteObject(g_hFaceBmp_4);
        g_hFaceBmp_4 = NULL;
    }

    if (g_hFaceBmp_5)
    {
        DeleteObject(g_hFaceBmp_5);
        g_hFaceBmp_5 = NULL;
    }
}

//paint random face on Windows ScreenSaver window
void PaintRandFace(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetWindowRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

	HBITMAP RandFace = NULL;
	int r = (rand() % 5) + 1;

    switch(r)
    {
        case 1:
            RandFace = g_hFaceBmp_1;
            break;
        case 2:
            RandFace = g_hFaceBmp_2;
            break;
        case 3:
            RandFace = g_hFaceBmp_3;
            break;
        case 4:
            RandFace = g_hFaceBmp_4;
            break;
        case 5:
            RandFace = g_hFaceBmp_5;
            break;

		default:
            RandFace = g_hFaceBmp_1;
            break;
    }


    if (RandFace)
    {
        BITMAP bm;
        GetObject(RandFace, sizeof(bm), &bm);

        HDC memDC = CreateCompatibleDC(hdc);
        HGDIOBJ old = SelectObject(memDC, RandFace);
        StretchBlt(hdc, 0, 0, width, height, memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        SelectObject(memDC, old);
        DeleteDC(memDC);
		RandFace = NULL;
    }

    EndPaint(hwnd, &ps);
}

HINSTANCE g_hInst = NULL;

static LRESULT CALLBACK RedWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_hFaceBmp_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
            g_hFaceBmp_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
            g_hFaceBmp_3 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
            g_hFaceBmp_4 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
            g_hFaceBmp_5 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));

            if (!(g_hFaceBmp_1 && g_hFaceBmp_2 && g_hFaceBmp_3 && g_hFaceBmp_4 && g_hFaceBmp_5))
            {
                MessageBoxA(NULL,
                    ("Error loading faces"),
                    ("no good"),
                    MB_OK | MB_ICONINFORMATION); // MB_OK is the default button type
            }

            return 0;
        }

        case WM_PAINT:
        {
			PaintRandFace(hwnd);
            return 0;
        }
        case WM_TIMER:
        {
            if (rand() % 3 == 0)
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
        case WM_CLOSE:
			CleanupFacesBmp();

            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


HWND CreateRedChildWindow(HWND hwndParent)
{
    static const char* CLASS_NAME = "RedChildWindowClass";
    static BOOL registered = FALSE;
    RECT rect;
    HWND hwndChild;

    if (!registered)
    {
        WNDCLASSA wc = { 0 };
        wc.lpfnWndProc = RedWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        if (!RegisterClassA(&wc))
            return NULL;

        registered = TRUE;
    }

    GetClientRect(hwndParent, &rect);

    hwndChild = CreateWindowExA(
        0,
        CLASS_NAME,
        NULL,
        WS_CHILD | WS_VISIBLE,
        0, 0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hwndParent,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    SetTimer(hwndChild, 1, 300, NULL); // 100 ms

    return hwndChild;
}

void RunMessageLoop(void)
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        if (msg.message == WM_QUIT)
            return;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/* This is where execution begins [windowed apps] */
int WINAPI MINGW32_FORCEALIGN
WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) /* NOLINT(readability-inconsistent-declaration-parameter-name) */
{
	g_hInst = hInst;

	//Comment/uncomment the following line to always launch the screen saver for testing
    //return main_getcmdline();


    hScreenSaverMutex = CreateMutexA(NULL, TRUE, SCREEN_SAVER_MUTEX);
    if (hScreenSaverMutex == NULL)
    {
        MessageBox(NULL,
            TEXT("Failed to create mutex"),
            TEXT("no good"),
            MB_OK | MB_ICONINFORMATION); // MB_OK is the default button type

        return 0;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Another instance is already running
        CloseHandle(hScreenSaverMutex);
        return 0;
    }


	LoadSettingsFromRegistry(&Registry_AudioEnabled, &Registry_MusicLevel, &Registry_SFXLevel);
    


	//Microsoft Screen Saver command line options:
    //https://learn.microsoft.com/en-us/previous-versions/troubleshoot/windows/win32/screen-saver-command-line

    if (strstr(szCmdLine, "S") || strstr(szCmdLine, "s"))
    {
        return main_getcmdline();
    }

    if (strstr(szCmdLine, "C") || strstr(szCmdLine, "c"))
    {

        return LaunchConfigWindow();
    }

    if (strstr(szCmdLine, "p") || strstr(szCmdLine, "P"))
    {
        HWND PreviewWindow = ParseWindowHandle(szCmdLine);

        if (PreviewWindow && IsWindow(PreviewWindow))
        {


            HWND hwndChild = CreateRedChildWindow(PreviewWindow);
            if (!hwndChild)
                return 1;

            RunMessageLoop();

            DestroyWindow(hwndChild);
            UnregisterClassA("RedChildWindowClass", GetModuleHandle(NULL));
        }
        else
        {
            MessageBox(NULL,
                TEXT("Window handle no good"),
                TEXT("no good"),
                MB_OK | MB_ICONINFORMATION); // MB_OK is the default button type
        }
    }

	//return LaunchConfigWindow();// if "C" or NOTHING is passed, launch config window
    CloseHandle(hScreenSaverMutex);

	return 0;
}

#endif /* __WIN32__ */

/* vi: set ts=4 sw=4 expandtab: */
