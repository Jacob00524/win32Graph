#include <stdio.h>
#include "main.h"

#include <wingraph/wingraph.h>

HWND main_window;
HWND graph;
POINT_2D points_array[6] = {{0,0}, {1, 2}, {2, 1}, {3, 2}, {4, 9}, {8, 0}};
POINTS_2D points = {(ULONG)6, (POINT_2D*)&points_array};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            HWND graph = create_new_graphW(hwnd, L"Example Graph", 10, 10, 400, 300);
            if (!graph)
            {
                printf("Graph creation failed\n");
                exit(1);
            }
            SendMessage(graph, WG_ADD_LINES, 1, 0);
            SendMessage(graph, WG_SET_LINE_COLOUR, 0, RGB(0xFF, 0, 0));
            SendMessage(graph, WG_SET_POINTS, 0, (LPARAM)&points);
            SendMessage(graph, WG_SET_VIEW_X, 0.7, 4);
            UpdateWindow(graph);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"ExampleWindow";
    //HMODULE hMod = LoadLibrary("wingraph.dll");
    MSG msg;

    /*if (!hMod)
    {
        printf("Load Library Failed\n");
        return 1;
    }*/
    Init_WinGraph(hInstance);

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    main_window = CreateWindowW(CLASS_NAME, L"Example Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);

    if (!main_window)
        return 1;

    ShowWindow(main_window, nCmdShow);
    UpdateWindow(main_window);

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int) msg.wParam;
}
