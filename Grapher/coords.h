#ifndef COORDS_H
#define COORDS_H

#include "SDL/SDL.h"
#include <cassert>

#include "general.h"


/// T MUST BE A TRIVIAL BUILT-IN NUMERIC TYPE!!! (int, unsigned, float, double, etc.)
MOLECULAR_CLASS Rect<T> {
public:
    FACTORIES(Coords)

    bool ctor() {
        x(0);
        y(0);
        w(0);
        h(0);

        return false;
    }

    /// Width-Height
    void ctorWH(T new_x, T new_y, T new_w, T new_h) {
        x(new_x);
        y(new_y);
        w(new_w);
        h(new_h);
    }

    /// Start-end
    void ctorSE(T new_x0, T new_y0, T new_x1, T new_y1) {
        x0(new_x0);
        y0(new_y0);
        x1(new_x1);
        y1(new_y1);
    }

    void dtor() {
        ctor();
    }

    inline T  x() const { return x_; }
    inline T  y() const { return y_; }
    inline T  w() const { return w_; }
    inline T  h() const { return h_; }
    inline T x0() const { return x_; }
    inline T y0() const { return y_; }
    inline T x1() const { return x_ + w_; }
    inline T y1() const { return y_ + h_; }

    inline void  x(T val) { x_ = val; }
    inline void  y(T val) { y_ = val; }
    inline void  w(T val) { w_ = val; }
    inline void  h(T val) { h_ = val; }
    inline void x0(T val) { x_ = val; }
    inline void y0(T val) { y_ = val; }
    inline void x1(T val) { w_ = val - x_; }
    inline void y1(T val) { h_ = val - y_; }

    void toSDLRect(SDL_Rect *dest) const {
        assert(dest);

        dest->x = x();
        dest->y = y();
        dest->w = w();
        dest->h = h();
    }

protected:
    T x_, y_, w_, h_;

}


MOLECULAR_CLASS Coords {
public:
    FACTORIES(Coords)

    bool ctor();

    void ctor(const Coords *other);

    /// Rect
    void ctor(const Rect<int> *new_clipRect);
    void ctor(const Rect<int> *new_clipRect, int new_centerX, int new_centerY);
    void ctor(const Rect<int> *new_clipRect, int new_centerX, int new_centerY,
              double new_ppuH, double new_ppuV);

    /// Width-Height
    void ctorWH(int new_x, int new_y, int new_w, int new_h);
    void ctorWH(int new_x, int new_y, int new_w, int new_h,
                int new_centerX, int new_centerY);
    void ctorWH(int new_x, int new_y, int new_w, int new_h,
                int new_centerX, int new_centerY, double new_ppuH, double new_ppuV);

    /// Start-end
    void ctorSE(int new_x0, int new_y0, int new_x1, int new_y1);
    void ctorSE(int new_x0, int new_y0, int new_x1, int new_y1,
                int new_centerX, int new_centerY);
    void ctorSE(int new_x0, int new_y0, int new_x1, int new_y1,
                int new_centerX, int new_centerY, double new_ppuH, double new_ppuV);

    void dtor();

    inline int centerX() const { return center[0]; };
    inline int centerY() const { return center[1]; };
    inline double ppuH() const { return ppu[0];    };
    inline double ppuV() const { return ppu[1];    };

    inline void centerX(int val) { center[0] = val; };
    inline void centerY(int val) { center[1] = val; };
    inline void ppuH(double val) { ppu[0] = val; };
    inline void ppuV(double val) { ppu[1] = val; };

    Rect<int> clipRect;

protected:
    int center[2];
    double ppu[2];

};


#endif // COORDS_H
