#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <math.h>
#include "wingraph.h"

#include "internal.h"

#define background_colour RGB(0xFF, 0xFF, 0xFF)

#define dotted_line_colour RGB(128, 128, 128)
#define dash_amount_perc (0.5)
#define dash_perc (0.05)
#define dotted_line_thickness (1)

#define line_thickness (2)

__declspec(dllexport) HWND create_new_graphW(HWND parent, WCHAR *name, int x, int y, int width, int height)
{
    HWND graph;

    if (parent)
        graph = CreateWindowW(L"WinGraph", name, WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, width, height, parent, NULL, GetModuleHandleW(NULL), NULL);
    else
        graph = CreateWindowExW(WS_EX_TOPMOST, L"WinGraph", name, WS_VISIBLE, x, y, width, height, NULL, NULL, GetModuleHandleW(NULL), NULL);
    return graph;
}

/*
    use for all double to integer conversions (future proofing)
*/
static inline int d_to_i(double val)
{
    return (int)round(val);
}

static int normalize_range(double val, double min, double max, double new_min, double new_max)
{
    double new_val = 0;

    if (max == min)
        return new_min;
    new_val = new_min + ((val - min) / (max - min)) * (new_max - new_min);
    return d_to_i(new_val);
}

static HRESULT paint_background(HDC hdc, RECT rc)
{
    HBRUSH hBrush = CreateSolidBrush(background_colour);

    FillRect(hdc, &rc, hBrush);
    DeleteObject(hBrush);
    return S_OK;
}

/*
    Component:
        0: to check x values
        1: to check y values
    Returned point:
        x = min
        y = max
*/
static POINT_2D get_min_max(POINTS_2D points, short check_y)
{
    POINT_2D ret = { INFINITY, -INFINITY };

    for (ULONG i = 0; i < points.num_points; i++)
    {
        if (check_y)
        {
            if (ret.x > points.points[i].y)
                ret.x = points.points[i].y;
            if (points.points[i].y > ret.y)
                ret.y = points.points[i].y;
        }else
        {
            if (ret.x > points.points[i].x)
                ret.x = points.points[i].x;
            if (points.points[i].x > ret.y)
                ret.y = points.points[i].x;
        }
    }
    return ret;
}

/*
    this needs to be re-done...
*/
static void paint_dotted_lines(COLORREF colour, HDC hdc, POINT start, POINT end)
{
    double length = end.y - start.y;
    double dash_length = length * dash_perc;
    double dash_length_neg;
    int dash_count;
    POINT current_point = { start.x, start.y };
    HPEN pen, oldPen;

    dash_count = round(dash_amount_perc / dash_perc);
    dash_length_neg = (length * (1 - dash_perc)) / (dash_count - 1);
    pen = CreatePen(PS_SOLID, line_thickness, colour);
    oldPen = (HPEN)SelectObject(hdc, pen);

    for (int i = 0; i < dash_count; i++)
    {
        MoveToEx(hdc, current_point.x, current_point.y, NULL);
        LineTo(hdc, current_point.x, current_point.y + (LONG)dash_length);
        current_point.y += (LONG)dash_length_neg;
    }
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void paint_line(COLORREF colour, HDC hdc, POINT start, POINT end)
{
    HPEN pen = CreatePen(PS_SOLID, line_thickness, colour);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, start.x, start.y, NULL);
    LineTo(hdc, end.x, end.y);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static HRESULT paint_LINE_2D(Graph2D *graph, LINE_2D *line, HDC hdc, RECT rect)
{
    POINT_2D x_min_max;
    POINT_2D x_min_max_view;
    POINT_2D y_min_max;
    ULONG num_points = line->points.num_points;
    ULONG min_points = 0;

    x_min_max = get_min_max(line->points, 0);
    y_min_max = get_min_max(line->points, 1);

    if (graph->x_view_min != -1)
        x_min_max_view.x = graph->x_view_min;
    else
        x_min_max_view.x = x_min_max.x;

    if (graph->x_view_max != -1)
        x_min_max_view.y = graph->x_view_max;
    else
        x_min_max_view.y = x_min_max.y;

    for (ULONG i = min_points; i < num_points; i++)
    {
        if (i + 1 == line->points.num_points)
            break;
        int start_x = normalize_range(line->points.points[i].x, x_min_max_view.x, x_min_max_view.y, rect.left, rect.right);
        int start_y = normalize_range(line->points.points[i].y, y_min_max.x, y_min_max.y, rect.bottom, rect.top);
        int end_x = normalize_range(line->points.points[i+1].x, x_min_max_view.x, x_min_max_view.y, rect.left, rect.right);
        int end_y = normalize_range(line->points.points[i+1].y, y_min_max.x, y_min_max.y, rect.bottom, rect.top);
        POINT start = { start_x, start_y };
        POINT end = { end_x, end_y };
        POINT dotted_start = { end_x, rect.bottom };
        paint_line(line->colour, hdc, start, end);
        if (rect.right > end.x)
            paint_dotted_lines(dotted_line_colour, hdc, end, dotted_start);
    }
    return S_OK;
}

LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    Graph2D *graph_data = NULL;
    HRESULT result;

    switch (msg)
    {
        case WM_CREATE:
        {
            graph_data = NULL;
            graph_data = malloc(sizeof(Graph2D));
            if (!graph_data)
                return -1;
            GetClientRect(hwnd, &rc);
            result = init_new_graph(graph_data, hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
            if (FAILED(result))
                return -1;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)graph_data);
            return 0;
        }
        case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;

            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(graph_data->hwnd, &rc);
            paint_background(hdc, rc);
            for (ULONG i = 0; i < graph_data->num_lines; i++)
                paint_LINE_2D(graph_data, &graph_data->lines[i], hdc, rc);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            graph_free(graph_data);
            break;
        }
        /* Custom Messages */
        case WG_ADD_LINES: /* wParam: # number of lines to add  lParam: 0 */
        {
            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            graph_add_lines(graph_data, wParam);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WG_SET_POINTS: /* wParam: line index  lParam: POINTS_2D */
        {
            POINTS_2D *points = (POINTS_2D*)lParam;

            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            line_set_points(graph_data, wParam, points);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WG_SET_POINT: /* Unimplemented */
            break;
        case WG_ADD_POINTS: /* wParam: line index  lParam: POINTS_2D */
        {
            POINTS_2D *points = (POINTS_2D*)lParam;

            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            line_add_points(graph_data, wParam, points);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WG_ADD_POINT: /* Unimplemented */
            break;
        case WG_SET_LINE_COLOUR: /* wParam: line index  lParam: colour */
        {
            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            line_set_colour(graph_data, wParam, lParam);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WG_SET_VIEW_X: /* wParam: min-x  lParam: max-x */
        {
            graph_data = (Graph2D*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            graph_data->x_view_min = (double)wParam;
            graph_data->x_view_max = (double)lParam;
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

__declspec(dllexport)  HRESULT Init_WinGraph(HINSTANCE instance)
{
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = ControlProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"WinGraph";

    if (!RegisterClassW(&wc))
    {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS)
        {
            printf("RegisterClass failed: %lu\n", err);
            return E_FAIL;
        }
    }
    return S_OK;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#pragma GCC diagnostic pop
