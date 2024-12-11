//  g++ start.cpp -o start -lallegro -lallegro_primitives
#include "allegro5/allegro.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>  // Optional, for TTF font support
#include <cmath>

#define PI M_PI

class GanttChartRenderer
{
public:
    void drawBoundary(int x1, int y1, int x2, int y2, int thickness)
    {
        al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(50, 50, 50), thickness);
    }

    void renderTaskBlock(int x1, int y1, int x2, int y2, char type, const char *label)
    {
        if (type == 'A')
            al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(80, 170, 90));
        else
            al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(90, 130, 200));
        drawBoundary(x1, y1, x2, y2, 2);
        if (label)
        {
            renderTaskLabel(x1, y1 - 20, label);
        }
    }

    void renderLine(int x1, int y1, int x2, int y2, int thickness, char lineType)
    {
        ALLEGRO_COLOR color = lineType == 'S' ? al_map_rgb(100, 100, 100) : al_map_rgb(200, 80, 80);
        al_draw_line(x1, y1, x2, y2, color, thickness);
    }

    void markTaskRelease(float x1, float y1)
    {
        renderLine(x1, y1 - 60, x1, y1, 2, 'S');
    }

    void markDeadline(float x1, float y1)
    {
        renderLine(x1, y1 - 60, x1, y1, 2, 'D');
    }

    void renderLine(float y1)
    {
        renderLine(25, y1, 1500, y1, 2, 'N');
    }
    void renderTaskLabel(int x, int y, const char *label)
    {
        ALLEGRO_FONT *font = al_create_builtin_font();
        al_draw_text(font, al_map_rgb(0, 0, 0), x, y, ALLEGRO_ALIGN_LEFT, label);
        al_destroy_font(font);
    }
};
