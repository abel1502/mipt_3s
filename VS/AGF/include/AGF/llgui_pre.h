#ifndef AGF_LLGUI_PRE_H
#define AGF_LLGUI_PRE_H

#include <ACL/general.h>
#include <ACL/math/cmath.h>
#include <ACL/gui/color.h>
#include <ACL/gui/rect.h>
#include <ACL/gui/coords.h>
#include <ACL/unique_ptr.h>


namespace abel::gui {

using abel::math::Vector2i;
using abel::math::Vector2d;

class Window;
class Texture;

}

extern template
class abel::unique_ptr<abel::gui::Window>;

extern template
class abel::unique_ptr<abel::gui::Texture>;



#endif // AGF_LLGUI_PRE_H
