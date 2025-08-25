#include <windows.h>
#include <winerror.h>

#include <stdio.h>
#include <string.h>

#include "internal.h"
#include "wingraph.h"

HRESULT reg_class(HINSTANCE instance);

HRESULT init_new_graph(Graph2D *out, HWND hwnd, int x, int y, int width, int height)
{
    if (!out)
        return E_POINTER;
    if (!hwnd)
        return E_INVALIDARG;
    memset(out, 0, sizeof(*out));
    out->x = x;
    out->y = y;
    out->height = height;
    out->width = width;
    out->hwnd = hwnd;
    return S_OK;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
HRESULT sort_points_x(POINTS_2D points)
{
    printf("sort_points_x TODO...\n");
    return E_NOTIMPL;
}
#pragma GCC diagnostic pop

HRESULT graph_is_valid(Graph2D *graph)
{
    if (!graph)
        return E_POINTER;
    if (graph->lines && graph->num_lines > 0)
        return TRUE;
    return FALSE;
}

HRESULT graph_add_lines(Graph2D *graph, ULONG number_of_lines)
{
    LINE_2D *new_pointer;

    if (!graph)
        return E_POINTER;
    if (graph_is_valid(graph))
    {
        new_pointer = realloc(graph->lines, sizeof(LINE_2D) * (graph->num_lines + number_of_lines));
        if (!new_pointer)
            return E_OUTOFMEMORY;
        memset(new_pointer + graph->num_lines, 0, sizeof(LINE_2D) * number_of_lines);
        graph->lines = new_pointer;
        graph->num_lines += number_of_lines;
    }else
    {
        graph->lines = calloc(number_of_lines, sizeof(LINE_2D));
        graph->num_lines = number_of_lines;
    }
    return S_OK;
}

INT line_is_valid(LINE_2D *line)
{
    if (!line)
        return E_POINTER;
    if (line->points.points && line->points.num_points > 0)
        return TRUE;
    return FALSE;
}

HRESULT line_free(LINE_2D *line)
{
    HRESULT res = line_is_valid(line);

    if (FAILED(res))
        return res;
    free(line->points.points);
    memset(line, 0, sizeof(*line));
    return S_OK;
}

HRESULT line_set_points(Graph2D *graph, ULONG line_index, POINTS_2D *points)
{
    LINE_2D *line = NULL;

    if (!points || !graph)
        return E_POINTER;
    if (graph->num_lines <= line_index)
        return E_INVALIDARG;
    line = &(graph->lines[line_index]);
    if (line_is_valid(line))
        line->points.points = realloc(line->points.points, sizeof(POINT_2D) * points->num_points);
    else
        line->points.points = calloc(points->num_points, sizeof(POINT_2D));
    if (!line->points.points)
        return E_OUTOFMEMORY;
    line->points.num_points = points->num_points;
    memcpy(line->points.points, points->points, sizeof(POINT_2D) * points->num_points);
    sort_points_x(line->points);
    return S_OK;
}

/*
    current memory is still valid if E_OUTOFMEMORY
*/
HRESULT line_add_points(Graph2D *graph, ULONG line_index, POINTS_2D *points)
{
    LINE_2D *line = NULL;
    POINT_2D *new_pointer;

    if (!points || !graph)
        return E_POINTER;
    if (graph->num_lines <= line_index)
        return E_INVALIDARG;
    line = &(graph->lines[line_index]);
    if (!line_is_valid(line))
        return line_set_points(graph, line_index, points);
    
    new_pointer = realloc(line->points.points, sizeof(POINT_2D) * (line->points.num_points + points->num_points));
    if (!new_pointer)
        return E_OUTOFMEMORY;
    memcpy(line->points.points + line->points.num_points, points->points, sizeof(POINT_2D) * points->num_points);
    line->points.num_points += points->num_points;
    sort_points_x(line->points);
    return S_OK;
}

HRESULT graph_free(Graph2D *graph)
{
    if (graph_is_valid(graph))
        free(graph->lines);
    memset(graph, 0, sizeof(*graph));
    return S_OK;   
}

HRESULT line_set_colour(Graph2D *graph, ULONG line_index, COLORREF colour)
{
    if (!graph)
        return E_POINTER;
    if (graph->num_lines <= line_index)
        return E_INVALIDARG;
    graph->lines[line_index].colour = colour;
    return S_OK;
}

/* Unused for future use maybe... */

HRESULT line_set_point(LINE_2D *line, ULONG index, POINT_2D point)
{
    HRESULT res = line_is_valid(line);

    if (FAILED(res))
        return res;
    if (line->points.num_points <= index)
        return E_INVALIDARG;
    line->points.points[index] = point;
    return S_OK;
}