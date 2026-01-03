

#include <ctype.h>
#include <string.h>
#include <windows.h>
#include <ShellAPI.h>

#include "SDL.h"

#include "c_console.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_misc.h"
#include "version.h"

static void GetVersionToken(const char *src, char *out, size_t outlen)
{
    const char  *p = src;
    size_t      i = 0;

    if (!src || !out || !outlen)
        return;

    while (*p && !isdigit(*p))
        p++;

    if (!*p)
    {
        if ((p = strchr(src, 'v')) && isdigit(*(p + 1)))
            p++;
        else
            p = src;
    }

    while (*p && i + 1 < outlen)
        if (isdigit(*p) || *p == '.' || *p == '-' || *p == '+' || isalpha(*p))
            out[i++] = *p++;
        else
            break;

    out[i] = '\0';
}

static BOOL CALLBACK FindWindowForProcess(HWND hwnd, LPARAM lParam)
{
    struct
    {
        DWORD   pid;
        HWND    *out;
    } *ctx = (void *)lParam;

    DWORD   pid = 0;

    GetWindowThreadProcessId(hwnd, &pid);

    if (pid == ctx->pid && IsWindowVisible(hwnd) && !GetWindow(hwnd, GW_OWNER))
    {
        *(ctx->out) = hwnd;
        return FALSE;
    }

    return TRUE;
}

void D_OpenURLInBrowser(const char *url, const char *warning)
{
    SHELLEXECUTEINFOA   sei = { 0 };
    HANDLE              hProc;

    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = GetActiveWindow();
    sei.lpVerb = "open";
    sei.lpFile = url;
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExA(&sei) && (hProc = sei.hProcess))
    {
        HWND    found = NULL;

        struct FindWindowCtx
        {
            DWORD   pid;
            HWND    *out;
        } ctx = {
            GetProcessId(hProc),
            &found
        };

        WaitForInputIdle(hProc, 5000);
        EnumWindows(FindWindowForProcess, (LPARAM)&ctx);

        if (found)
        {
            DWORD   currentThread = GetCurrentThreadId();
            DWORD   windowThread = GetWindowThreadProcessId(found, NULL);

            AttachThreadInput(currentThread, windowThread, TRUE);

            ShowWindow(found, SW_SHOWNORMAL);
            SetForegroundWindow(found);
            BringWindowToTop(found);

            AttachThreadInput(currentThread, windowThread, FALSE);
        }

        CloseHandle(hProc);
    }
    else if (warning)
        C_Warning(0, warning);
}
