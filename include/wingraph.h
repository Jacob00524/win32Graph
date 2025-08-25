#pragma once

typedef struct POINT_2D
{
    double x;
    double y;
}POINT_2D;

typedef struct POINTS_2D
{
    ULONG num_points;
    POINT_2D *points;
}POINTS_2D;

typedef struct LINE_2D
{
    POINTS_2D points;
    COLORREF colour;
}LINE_2D;

/* Messages */
#define WG_ADD_LINES (WM_USER + 1) /* wParam: Number of lines to add  lParam: 0 */

#define WG_SET_POINTS (WM_USER + 2) /* wParam: line index  lParam: POINTS_2D  Note: It allocates new space for POINTS_2D, application is responsible to free passed in data */
#define WG_SET_POINT (WM_USER + 3) /* Unimplemented */

#define WG_ADD_POINTS (WM_USER + 4) /* wParam: line index  lParam: POINTS_2D */
#define WG_ADD_POINT (WM_USER + 5) /* Unimplemented */

#define WG_SET_LINE_COLOUR (WM_USER + 6) /* wParam: line index  lParam: colour */

#define WG_SET_VIEW_X (WM_USER + 7) /* wParam: min-x  lParam: max-x */

/* Exposed Functions */
HRESULT Init_WinGraph(HINSTANCE instance); /* Call this first. */
__declspec(dllexport) HWND create_new_graphW(HWND parent, WCHAR *name, int x, int y, int width, int height);
