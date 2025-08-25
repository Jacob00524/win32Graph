#include "wingraph.h"

typedef struct Graph2D
{
    int x;
    int y;
    int width;
    int height;

    double x_view_min;
    double x_view_max;

    ULONG num_lines;
    LINE_2D *lines;

    HWND hwnd;
}Graph2D;

HRESULT init_new_graph(Graph2D *out, HWND hwnd, int x, int y, int width, int height);
HRESULT graph_free(Graph2D *graph);

HRESULT graph_add_lines(Graph2D *graph, ULONG number_of_lines);
HRESULT line_set_points(Graph2D *graph, ULONG line_index, POINTS_2D *points);
HRESULT line_add_points(Graph2D *graph, ULONG line_index, POINTS_2D *points);
HRESULT line_set_colour(Graph2D *graph, ULONG line_index, COLORREF colour);