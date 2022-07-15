#include "graphics.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <cmath>

using namespace genv;
using namespace std;

const int X=400;
const int Y=400;
const double PI = 3.14159265;

struct Color {
    int value;
    bool increases = true;
    Color(int n) : value(n) {}
    bool value_increases() {
        if (value == 255)
            return increases = false;
        else if (value == 0)
            return increases = true;
        else return increases;
    }
};

struct RGB{
    Color r, g, b;
    RGB(Color _r, Color _g, Color _b) : r(_r), g(_g), b(_b) {}
};

int main()
{
    bool ctrl_pressed = false, left_pressed = false, right_pressed = false;
    int ox = 0, oy = 0;
    //int dx = 0, dy = 0;

    gout.open(X, Y);

    event ev;
    gin.timer(5);
    gout << refresh;

    RGB c(255, 155, 0);
    gout << color(c.r.value, c.g.value, c.b.value) << ellipse(50, 50, 20, 10) << refresh;
    gout << color(255, 255, 255) << circle(ox, oy, 2);
    gout << color(c.r.value, c.g.value, c.b.value) << ellipse(50, 50, 20, 10);
    gout << color(c.r.value, c.g.value, c.b.value) << circle(-30, -30, 10);


    while (gin >> ev && ev.keycode != key_escape) {

        if (ev.type == ev_mouse) {

            if (ev.button == btn_left)
                left_pressed = true;
            else if (ev.button == -btn_left)
                left_pressed = false;

            if (ev.button == btn_right)
                right_pressed = true;
            else if (ev.button == -btn_right)
                right_pressed = false;

            //rotation
            if (right_pressed) {
                //int vec1_x = ev.pos_x - ox;
                //int vec1_y = ev.pos_y - oy;
                //int vec2_x = ;
                //int vec2_y = ;
                //angle = (vec1_x * vec2_x + vec1_y * vec2_y) / ( sqrt(vec1_x*vec1_x+vec1_y*vec1_y) * sqrt(vec2_x*vec2_x+vec2_y*vec2_y) );
                gout << rotate(/*angle*/ 20*PI/180.0);
            }

            //dx = ev.pos_x;
            //dy = ev.pos_y;

        }


        if (ev.type == ev_key || ev.type == ev_mouse) {

            if (ev.keycode == key_lctrl)
                ctrl_pressed = true;

            else if ((ev.button == btn_left && ctrl_pressed) || (left_pressed && ctrl_pressed && ev.button == 0)) {

                gout << color(0, 0, 0) << move_to(ox, 0) << line_to(ox, Y) << move_to(0, oy) << line_to(X, oy) << circle(0, 0, 2);

                ox = ev.pos_x; oy = ev.pos_y;

                gout << origin(ox, oy);

                gout << color(255, 255, 255) << move_to(ox, 0) << line_to(ox, Y) << move_to(0, oy) << line_to(X, oy) << circle(0, 0, 2);

            } else if (ev.keycode == -key_lctrl)
                ctrl_pressed = false;
        }


        if (c.r.value_increases())
            c.r.value++;
        else
            c.r.value--;

        if (c.g.value_increases())
            c.g.value++;
        else
            c.g.value--;

        if (c.b.value_increases())
            c.b.value++;
        else
            c.b.value--;

        gout << color(c.r.value, c.g.value, c.b.value) << ellipse(50, 50, 20, 10);

        if (left_pressed && !ctrl_pressed)
            gout << color(c.r.value, c.g.value, c.b.value) << move_to(ev.pos_x, ev.pos_y) << dot;

        gout << refresh;


    }

    return 0;
}
