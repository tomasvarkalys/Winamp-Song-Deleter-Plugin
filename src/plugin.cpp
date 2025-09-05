#include <windows.h>
#include <shellapi.h>
#include "../WinampSDK/gen.h"
#include "../WinampSDK/wa_ipc.h"
#include "../WinampSDK/ipc_pe.h"
#include "resource.h"

#define TRAY_ICON_UID 1
#define WM_TRAYICON (WM_USER + 100)
#define WINAMP_STOP 40047
#define WINAMP_NEXT 40048

static HINSTANCE g_hInst;
static HWND g_hwndWinamp;
static HWND g_hwndTray;
static NOTIFYICONDATA nid = {0};

static void RemoveTrayIcon();
static void AddTrayIcon();
static void CreateTrayWindow();
static void DeleteCurrentSongToRecycleBin();
static int Init();
static void Config();
static void Quit();

// Plugin structure
static winampGeneralPurposePlugin plugin = {
    GPPHDR_VER,
    const_cast<char*>("Winamp Song Deleter Plugin"),
    Init,
    Config,
    Quit,
    0,
    0
};

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        g_hInst = hInst;
    }
    return TRUE;
}

// Implementation of the plugin interface functions
static int Init() {
    g_hInst = plugin.hDllInstance;
    g_hwndWinamp = plugin.hwndParent;
    CreateTrayWindow();
    AddTrayIcon();
    return 0;
}

static void Config() {
    MessageBoxA(NULL, "Winamp Song Deleter Plugin\nDeletes current song to recycle bin.", "Config", MB_OK);
}

static void Quit() {
    RemoveTrayIcon();
    DestroyWindow(g_hwndTray);
}

// The main exported function that Winamp calls
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
    return &plugin;
}

// Helper functions implementations
static void RemoveTrayIcon() {
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwndTray;
    nid.uID = TRAY_ICON_UID;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

static void AddTrayIcon() {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = g_hwndTray;
    nid.uID = TRAY_ICON_UID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_TRAYICON));
    if (!nid.hIcon) return;
    lstrcpyA(nid.szTip, "Delete current song");
    Shell_NotifyIcon(NIM_ADD, &nid);
}



static void DeleteCurrentSongToRecycleBin() {
    int pos = SendMessage(g_hwndWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);
    if (pos < 0) return;

    int playlistLen = SendMessage(g_hwndWinamp, WM_WA_IPC, 0, IPC_GETLISTLENGTH);
    if (playlistLen <= 0) return;

    char *filename = (char *)SendMessage(g_hwndWinamp, WM_WA_IPC, pos, IPC_GETPLAYLISTFILE);
    if (!filename || !*filename) return;

    int deletePos = pos;

    // Move to next track first, then delete (ensures file handle is released)
    SendMessage(g_hwndWinamp, WM_COMMAND, WINAMP_NEXT, 0);
    Sleep(500);

    // Convert to Unicode for long path support
    wchar_t wFilePath[32768] = {0};
    int convertResult = MultiByteToWideChar(CP_UTF8, 0, filename, -1, wFilePath, 32767);
    if (convertResult == 0) return;

    // Create double-null terminated Unicode string
    size_t wLen = wcslen(wFilePath);
    wFilePath[wLen] = L'\0';
    wFilePath[wLen + 1] = L'\0';

    // Move to recycle bin using Unicode version
    SHFILEOPSTRUCTW fileOp = {0};
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = wFilePath;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    int result = SHFileOperationW(&fileOp);
    if (result == 0) {
        // Remove from playlist
        HWND playlistHwnd = (HWND)SendMessage(g_hwndWinamp, WM_WA_IPC, IPC_GETWND_PE, IPC_GETWND);
        if (playlistHwnd) {
            SendMessage(playlistHwnd, WM_WA_IPC, IPC_PE_DELETEINDEX, deletePos);
        }
    }
}

static LRESULT CALLBACK TrayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON) {
        switch (lParam) {
            case WM_LBUTTONUP:
                DeleteCurrentSongToRecycleBin();
                return 0;
            case WM_RBUTTONUP:
                return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void CreateTrayWindow() {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = "WinampSongDeleterTray";
    if (!RegisterClassA(&wc)) return;

    g_hwndTray = CreateWindowA(
        "WinampSongDeleterTray",
        "",
        WS_OVERLAPPED,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        g_hInst,
        NULL
    );
}
