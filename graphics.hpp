#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

#include <string>
#include <vector>
#include <SDL_keycode.h>

struct SDL_Surface;
struct SDL_Window;
struct _TTF_Font;
struct SDL_Renderer;

namespace genv
{

/*********** Graphical output device definition ***********/

class canvas {
public:
    canvas();
    virtual ~canvas();
    canvas(int w, int h);
    canvas(const canvas & c);
	genv::canvas& operator=(const genv::canvas &c);
    bool open(unsigned width, unsigned height);
    bool save(const std::string& file) const;
    void transparent(bool t) {transp=t;}
    void set_color(int r, int g, int b);
    bool move_point(int x, int y);
    void draw_dot();
    void draw_line(int x, int y);
    void draw_box(int x, int y);

    void draw_text(const std::string& str);
    void blitfrom(const canvas &c, short x1, short y1, short x2, short y2, short x3, short y3);

    // új --------------------------------------------------------------
    void draw_circle(int x, int y, int r);
    void draw_ellipse(int x, int y, int a, int b);
    void rotate(double angle);
    void set_origin(short _ox, short _oy);
    int orig_x() const { return ox; }
    int orig_y() const { return oy; }
    double get_angle() const { return angle; }
    void set_angle(double _angle) { angle = _angle; }
    // új --------------------------------------------------------------

    bool load_font(const std::string& fname, int fontsize = 16, bool antialias=true);
    void set_antialias(bool antialias) {antialiastext=antialias;}

    int x() const { return pt_x; }
    int y() const { return pt_y; }

    int cascent() const;
    int cdescent() const;

    int twidth(const std::string& s) const;

    virtual void refresh() {}

    template <typename T>
    inline void call_with_rel(T meth, int vec_x, int vec_y) {
	if (vec_x || vec_y) {
	    int dx=vec_x-sgn(vec_x);
	    int dy=vec_y-sgn(vec_y);
	    (this->*meth)(dx, dy);
	}
    }

protected:

    template <typename T>
    inline int sgn(const T& a) {
		if (a<0) return -1;
		if (a>0) return 1;
		return 0;
    }

    // új --------------------------------------------------------------
    short ox;
    short oy;
    double angle;
    // új --------------------------------------------------------------

    short pt_x;
    short pt_y;
    SDL_Surface* buf;
    int draw_clr;
    bool transp;
    _TTF_Font* font;
    bool antialiastext;
    std::string loaded_font_file_name;
    int font_size;

};




// Class of output device (singleton)
class groutput : public canvas
{
public:
    static groutput& instance();
    virtual ~groutput();

    void showmouse(bool toggle);
    void movemouse(int x, int y);
    bool open(unsigned width, unsigned height, bool fullscreen=false);
    virtual void refresh();
	void set_title(const std::string& title);
	void message(std::string errortext);

private:
	SDL_Window * window;
	SDL_Renderer* renderer;
    groutput();
};

// Global accessor for the output device instance
extern groutput& gout;

// Generic operator for applying global manipulators
template <typename Op>
inline canvas& operator << (canvas& out, Op oper)
{ oper(out); return out; }


// Global manipulators for frequently used operations
inline void refresh(canvas& out) { out.refresh(); }
inline void dot(canvas& out) { out.draw_dot(); }

struct stamp
{
    canvas &c;
    int x1,y1,x2,y2,x3,y3;
    stamp(canvas&cc, int sx1, int sy1, int xsize, int ysize, int tx1, int ty1) :
        c(cc), x1(sx1), y1(sy1),x2(xsize), y2(ysize), x3(tx1), y3(ty1) {}
    stamp(canvas&cc, int tx1, int ty1) :
        c(cc), x1(-1), y1(-1),x2(-1), y2(-1), x3(tx1), y3(ty1) {}
    void operator () (canvas& out)
    { out.blitfrom(c,x1,y1,x2,y2,x3,y3); }
};

struct color
{
    int red, green, blue;
    color(int r, int g, int b) : red(r), green(g), blue(b) {}
    void operator () (canvas& out)
    { out.set_color(red, green, blue); }
};

struct move
{
    int vec_x, vec_y;
    move(int x, int y) : vec_x(x), vec_y(y) {}
    void operator () (canvas& out)
    { out.move_point(vec_x, vec_y); }
};

struct move_to
{
    int pos_x, pos_y;
    move_to(unsigned x, unsigned y) : pos_x(x), pos_y(y) {}
    void operator () (canvas& out)
    { out.move_point(pos_x - out.x(), pos_y - out.y()); }
};

struct line
{
    int vec_x, vec_y;
    line(int x, int y) : vec_x(x), vec_y(y) {}
    void operator () (canvas& out)
    { out.call_with_rel(&canvas::draw_line,vec_x, vec_y); }
};

struct line_to
{
    int pos_x, pos_y;
    line_to(unsigned x, unsigned y) : pos_x(x), pos_y(y) {}
    void operator () (canvas& out)
    { out.draw_line(pos_x - out.x(), pos_y - out.y()); }
};

struct box
{
    int vec_x, vec_y;
    box(int x, int y) : vec_x(x), vec_y(y) {}
    void operator () (canvas& out)
    { out.call_with_rel(&canvas::draw_box, vec_x, vec_y); }
};

struct box_to
{
    int pos_x, pos_y;
    box_to(unsigned x, unsigned y) : pos_x(x), pos_y(y) {}
    void operator () (canvas& out)
    { out.draw_box(pos_x - out.x(), pos_y - out.y()); }
};

struct text
{
    std::string str;
    text(const std::string& s) : str(s) {}
    text(char c) : str(1, c) {}
    void operator () (canvas& out)
    { out.draw_text(str); }
};

// új --------------------------------------------------------------
struct circle {
    int pos_x, pos_y, radius;
    circle(int x, int y, int r) : pos_x(x), pos_y(y), radius(r) {}
    void operator () (canvas& out) {
        out.draw_circle(pos_x, pos_y, radius);
    }
};

struct ellipse {
    int pos_x, pos_y, a, b;
    ellipse(int x, int y, int _a, int _b) : pos_x(x), pos_y(y), a(_a), b(_b) {}
    void operator () (canvas& out) {
        out.draw_ellipse(pos_x, pos_y, a, b);
    }
};

struct origin{
    short ox, oy;
    origin(short _ox, short _oy) : ox(_ox), oy(_oy) {}
    void operator () (canvas& out) {
        out.set_origin(ox, oy);
    }
};

struct rotate{
    double angle;
    rotate(double _angle) : angle(_angle) {}
    void operator () (canvas& out) {
        out.rotate(angle);
    }
};
// új --------------------------------------------------------------


/*
struct title
{
    std::string str;
    title(const std::string& s) : str(s) {}
    void operator () (canvas& out)
    { out.set_title(str); }
};
*/

struct font
{
    std::string font_name;
    int font_size;
    bool antialias;
    font(const std::string& s, int fs, bool a=true) : font_name(s), font_size(fs), antialias(a) {}
    void operator () (canvas& out)
    { out.load_font(font_name, font_size, antialias); }
};


/*********** Input device definition **********/

// Keycode constants
enum keycode_t {
    key_tab = SDLK_TAB,
    key_backspace = SDLK_BACKSPACE,
    key_enter = SDLK_RETURN,
    key_escape = SDLK_ESCAPE,
    key_space = SDLK_SPACE,
    key_up = SDL_SCANCODE_UP,
    key_down = SDL_SCANCODE_DOWN,
    key_right = SDL_SCANCODE_RIGHT,
    key_left = SDL_SCANCODE_LEFT,
    key_insert = SDL_SCANCODE_INSERT,
    key_delete = 65541,
    key_home = SDL_SCANCODE_HOME,
    key_end = SDL_SCANCODE_END,
    key_pgup = SDL_SCANCODE_PAGEUP,
    key_pgdn = SDL_SCANCODE_PAGEDOWN,
    key_lshift = SDL_SCANCODE_LSHIFT,
    key_rshift = SDL_SCANCODE_RSHIFT,
    key_lctrl = 65548,
    key_rctrl = SDL_SCANCODE_RCTRL,
    key_lalt = 65550,
    key_ralt = SDL_SCANCODE_RALT,
    key_lwin,
    key_rwin,
    key_menu = SDL_SCANCODE_MENU,
    key_numl = SDL_SCANCODE_NUMLOCKCLEAR,
    key_capsl = SDL_SCANCODE_CAPSLOCK,
    key_scrl = SDL_SCANCODE_SCROLLLOCK,
    key_f0 = 0x20000,
    key_f1 = SDL_SCANCODE_F1,
    key_f2 = SDL_SCANCODE_F2,
    key_f3 = SDL_SCANCODE_F3,
    key_f4 = SDL_SCANCODE_F4,
    key_f5 = SDL_SCANCODE_F5,
    key_f6 = SDL_SCANCODE_F6,
    key_f7 = SDL_SCANCODE_F7,
    key_f8 = SDL_SCANCODE_F8,
    key_f9 = SDL_SCANCODE_F9,
    key_f10 = SDL_SCANCODE_F10,
    key_f11 = SDL_SCANCODE_F11,
    key_f12 = SDL_SCANCODE_F12,
    key_f13 = SDL_SCANCODE_F13,
    key_f14 = SDL_SCANCODE_F14,
    key_f15 = SDL_SCANCODE_F15
};

enum button_t {
    btn_left = 1, btn_middle, btn_right, btn_wheelup, btn_wheeldown
};

enum event_type {
    ev_key = 1, ev_mouse, ev_timer
};

// Event descriptor
struct event
{
    int keycode;
    int pos_x, pos_y;
    int button;
    int time;
    int type;
	std::string keyname;
	std::string keyutf8;

};

// Class of input device (singleton)
class grinput
{
public:
    static grinput& instance();

    grinput& wait_event(event&);
    void timer(int wait);

    operator const void*() const
    { if (quit) return 0; else return this; }
    void textmode(bool on);
private:
    grinput() : quit(false) {textmode(true);}
    bool quit;
};

// Global accessor for the input device instance
extern grinput& gin;

// Event reader operator
inline grinput& operator >> (grinput& inp, event& ev)
{ return inp.wait_event(ev); }

std::vector<int> utf8_character_index(std::string str);
std::vector<std::string> utf8_character_split(std::string str) ;

}

#endif // GRAPHICS_HPP_INCLUDED
