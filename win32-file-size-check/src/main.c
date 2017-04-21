#include <windows.h>
#include <cstdio>

char szClassName[] = "WindowsApp";

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(
        HINSTANCE hThisInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpszArgument,
        int nFunsterStil)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx(
        0, szClassName, szClassName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
        CW_USEDEFAULT, 800, 600, HWND_DESKTOP, NULL, hThisInstance, NULL);

    char szFileName[MAX_PATH] = {};

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = "txt";
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    GetOpenFileName(&ofn);

    HANDLE file = CreateFile(szFileName, GENERIC_READ,
        0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    DWORD hsize;
    DWORD lsize = GetFileSize(file, &hsize);

    char buf[64] = {};
    snprintf(buf, 64, "lsize - %dB\nhsize - %dB", lsize, hsize);
    MessageBox(hwnd, buf, szFileName, MB_OK);

    ExitProcess(0);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}
