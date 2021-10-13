#include "gui/gui.h"
#include "gui/coords.h"
#include "borders.h"


void Border::render(Texture &texture, const Coords &coords) const {
    texture.drawLineInfAlong(coords.virt2screen(getPoint()), coords.scale_v2s(getLine()));
}
