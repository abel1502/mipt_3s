#include <AGF/llgui.h>
#include <AGF/style.h>
#include <AGF/helpers/animation.h>
#include <ACL/type_traits.h>
#include <cctype>
#include <fstream>


namespace abel::gui {


#pragma region Style::TileMap
Style::TileMap::TileMap(const char *srcFileName, const Rect<double> &outer_,
                        const Rect<double> &inner_, bool shouldFillInner_) :
    texture{new Texture(srcFileName)}, outer{outer_}, inner{inner_}, shouldFillInner{shouldFillInner_} {

    REQUIRE(!inner.isEmpty());
    REQUIRE(!outer.isEmpty());
    REQUIRE(texture->getRect().contains(outer));
    REQUIRE(outer.contains(inner));
}

void Style::TileMap::assemble(Texture &target, const Rect<double> &at) const {
    // The corners are transferred as-is
    for (unsigned i = 0; i < 4; ++i) {
        Corner corner = (Corner)i;

        target.embedPart(getTargetCorner(at, corner), *texture, getCorner(corner));
    }

    // The sides are stretched
    // TODO: Maybe tile instead of stretching?
    for (unsigned i = 0; i < 4; ++i) {
        Side side = (Side)i;

        target.embedPart(getTargetSide(at, side), *texture, getSide(side));
    }

    // The inner part is stretched (if at all transferred)
    if (shouldFillInner) {
        target.embedPart(getTargetBody(at), *texture, getBody());
    }
}
#pragma endregion Style::TileMap


#pragma region Style
#pragma region Parsing
static bool skipSpace(std::istream &stream) {
    int lastChar = 0;

    while (stream.good() && isspace(lastChar = stream.get())) {}

    if (!isspace(lastChar) && lastChar > 0)
        stream.unget();

    return !stream.good();
}

static bool skipLine(std::istream &stream) {
    int lastChar = 0;

    while (stream.good() && (lastChar = stream.get()) != '\n') {}

    if (lastChar != '\n' && lastChar > 0)
        stream.unget();

    return !stream.good();
}

static bool readDelim(std::istream &stream, char delim) {
    if (skipSpace(stream))
        return true;

    if (stream.get() != delim) {
        stream.unget();
        return true;
    }

    return !stream.good();
}

static bool readToken(std::istream &stream, char *dest, unsigned limit) {
    if (readDelim(stream, '"'))
        return true;

    stream.getline(dest, limit, '"');

    return !stream.good();
}

template <typename T>
static bool readToken(std::istream &stream, std::enable_if_t<std::is_arithmetic_v<T>, T> &dest) {
    stream >> dest;

    return !stream.good();
}

template <typename T>
static bool readToken(std::istream &stream, Rect<T> &dest) {
    T tmp[4] = {};

    if (readDelim(stream, '['))
        return true;

    for (unsigned i = 0; i < 4; ++i) {
        if (readToken<T>(stream, tmp[i]) ||
            (i + 1 < 4 && readDelim(stream, ',')))
            return true;
    }

    if (readDelim(stream, ']'))
        return true;

    dest = Rect<T>::wh(tmp[0], tmp[1], tmp[2], tmp[3]);

    return !stream.good();
}
#pragma endregion Parsing

void Style::loadTileMaps(const char *indexFileName) {
    static constexpr unsigned
        BUF_SIZE = 512,
        BUF_LIMIT = BUF_SIZE - 1;

    std::ifstream indexFile(indexFileName);
    char elemName [BUF_SIZE] = "",
         elemState[BUF_SIZE] = "",
         fileName [BUF_SIZE] = "";

    Rect<double> outer{},
                 inner{};

    #define ERROR_  MACROFUNC(                      \
        throw error("Corrupt tilemap index file");  \
    )

    while (indexFile) {
        if (skipSpace(indexFile)) {
            if (indexFile.eof())
                break;

            ERROR_;
        }

        if (indexFile.peek() == '#') {
            skipLine(indexFile);
            continue;
        }

        if (readToken(indexFile, elemName,  BUF_LIMIT) ||
            readToken(indexFile, elemState, BUF_LIMIT)) {
            ERROR_;
        }

        if (std::string_view("*") == elemState) {
            markElemSysDrawn(elemByName(elemName));
        } else {
            markElemSysDrawn(elemByName(elemName), false);

            if (readToken(indexFile, fileName,  BUF_LIMIT) ||
                readToken(indexFile, outer)                ||
                readToken(indexFile, inner)) {
                ERROR_;
            }

            Element elem = elemByName(elemName);
            ElementState state = elemStateByName(elemState);

            getTileMap(elem, state) = std::move(TileMap{fileName, outer, inner});
        }

        if (readDelim(indexFile, ';')) {
            ERROR_;
        }
    }

    #undef ERROR_

    REQUIRE(indexFile.eof());
}

void Style::drawElement(Texture &target, const Rect<double> &dest,
                        Element element, ElementState state) const {
    if (isElemSysDrawn(element)) {
        return sysDrawElement(target, dest, element, state);
    }

    const TileMap &generator = getTileMap(element, state);

    if (!generator.isPresent()) {
        ERR("No tilemap defined for %u %u", element, state);  // TODO: String names!

        throw error("Tilemap not found");
    }

    generator.assemble(target, dest);
}

Animation Style::animElement(const Rect<double> &region, Element element,
                             ElementState state0, ElementState state1, double duration) const {
    auto tex0 = unique_ptr<Texture>::createEmplace(region);
    auto tex1 = unique_ptr<Texture>::createEmplace(region);

    tex0->setFillColor(Color::WHITE, 0);
    tex0->clear();

    tex1->setFillColor(Color::WHITE, 0);
    tex1->clear();

    drawElement(*tex0, region, element, state0);
    drawElement(*tex1, region, element, state1);

    return Animation(std::move(tex0), std::move(tex1), duration, state0, state1);
}

void Style::sysDrawElement(Texture &target, const Rect<double> &dest,
                           Element element, ElementState state) const {
    unsigned sysState = 0;
    unsigned sysElem  = 0;
    unsigned scrollbarArrowDir = 0;
    unsigned scrollbarGripperElem = 0;

    switch (element) {
    case EL_WND_HEADER: {
        switch (state) {
        case ELS_NORMAL:
        case ELS_HOVERED:
        case ELS_PRESSED:
            sysState = CS_ACTIVE;
            break;

        case ELS_INACTIVE:
            sysState = CS_INACTIVE;
            break;

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_WINDOW>(),
                                 WP_CAPTION, sysState);
    } break;

    case EL_WND_FRAME: {
        target.drawThemedControl(Rect<double>::se(dest.x0(), dest.y0(), dest.x0() + wndBorderWidth, dest.y1()),
                                 Window::getInstance().getTheme<Window::WT_WINDOW>(), WP_FRAMELEFT, 0);
        target.drawThemedControl(Rect<double>::se(dest.x1() - wndBorderWidth, dest.y0(), dest.x1(), dest.y1()),
                                 Window::getInstance().getTheme<Window::WT_WINDOW>(), WP_FRAMERIGHT, 0);
        target.drawThemedControl(Rect<double>::se(dest.x0(), dest.y1() - wndBorderWidth, dest.x1(), dest.y1()),
                                 Window::getInstance().getTheme<Window::WT_WINDOW>(), WP_FRAMEBOTTOM, 0);
        target.setFillColor(Color::WHITE);
        target.drawRect(dest.padded(wndBorderWidth, wndBorderWidth, 0, wndBorderWidth));
    } break;

    case EL_BUTTON: {
        switch (state) {
        case ELS_NORMAL:
            sysState = PBS_NORMAL;
            break;

        case ELS_HOVERED:
            sysState = PBS_HOT;
            break;

        case ELS_PRESSED:
            sysState = PBS_PRESSED;
            break;

        case ELS_INACTIVE:
            sysState = PBS_DISABLED;

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_BUTTON>(),
                                 BP_PUSHBUTTON, sysState);
    } break;

    case EL_CLOSE_BTN: {
        sysElem = WP_CLOSEBUTTON;
        goto caseHeaderButton;
    } break;

    case EL_MINIMIZE_BTN: {
        sysElem = WP_MINBUTTON;
        goto caseHeaderButton;
    } break;

    caseHeaderButton: {
        switch (state) {
        case ELS_NORMAL:
            sysState = CBS_NORMAL;
            break;

        case ELS_HOVERED:
            sysState = CBS_HOT;
            break;

        case ELS_PRESSED:
            sysState = CBS_PUSHED;
            break;

        case ELS_INACTIVE:
            sysState = CBS_DISABLED;
            break;

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_WINDOW>(),
                                 sysElem, sysState);
    } break;

    case EL_SLIDER_THUMB: {
        sysElem = TKP_THUMB;
        goto caseThumb;
    } break;

    case EL_SLIDER_VTHUMB: {
        sysElem = TKP_THUMBRIGHT;
        goto caseThumb;
    } break;

    case EL_SLIDER_HTHUMB: {
        sysElem = TKP_THUMBTOP;
        goto caseThumb;
    } break;

    caseThumb: {
        switch (state) {
        case ELS_NORMAL:
            static_assert(TUS_NORMAL ==   TUS_NORMAL &&
                          TUS_NORMAL ==  TUTS_NORMAL &&
                          TUS_NORMAL == TUVRS_NORMAL);
            sysState = TUS_NORMAL;
            break;

        case ELS_HOVERED:
            static_assert(TUS_HOT ==   TUS_HOT &&
                          TUS_HOT ==  TUTS_HOT &&
                          TUS_HOT == TUVRS_HOT);
            sysState = TUS_HOT;
            break;

        case ELS_PRESSED:
            static_assert(TUS_PRESSED ==   TUS_PRESSED &&
                          TUS_PRESSED ==  TUTS_PRESSED &&
                          TUS_PRESSED == TUVRS_PRESSED);
            sysState = TUS_PRESSED;
            break;

        case ELS_INACTIVE:
            static_assert(TUS_DISABLED ==   TUS_DISABLED &&
                          TUS_DISABLED ==  TUTS_DISABLED &&
                          TUS_DISABLED == TUVRS_DISABLED);
            sysState = TUS_DISABLED;
            break;

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_TRACKBAR>(),
                                 sysElem, sysState);
    } break;

    case EL_SLIDER_HBODY: {
        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_TRACKBAR>(),
                                 TKP_TRACK, TRS_NORMAL);
    } break;

    case EL_SLIDER_VBODY: {
        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_TRACKBAR>(),
                                 TKP_TRACKVERT, TRS_NORMAL);
    } break;

    case EL_SLIDER_BODY_2D: {
        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_TRACKBAR>(),
                                 TKP_TRACK, TRS_NORMAL);
    } break;

    case EL_SCROLLBAR_ARR_UP: {
        scrollbarArrowDir = ABS_UPNORMAL;
        goto caseScrollbarArrow;
    } break;

    case EL_SCROLLBAR_ARR_DOWN: {
        scrollbarArrowDir = ABS_DOWNNORMAL;
        goto caseScrollbarArrow;
    } break;

    case EL_SCROLLBAR_ARR_LEFT: {
        scrollbarArrowDir = ABS_LEFTNORMAL;
        goto caseScrollbarArrow;
    } break;

    case EL_SCROLLBAR_ARR_RIGHT: {
        scrollbarArrowDir = ABS_RIGHTNORMAL;
        goto caseScrollbarArrow;
    } break;

    caseScrollbarArrow: {
        static_assert(ABS_UPNORMAL ==
                      ABS_DOWNNORMAL - 4 ==
                      ABS_LEFTNORMAL - 8 ==
                      ABS_RIGHTNORMAL - 12, "Assumption about slider arrows was wrong");

        switch (state) {
        case ELS_NORMAL:
            sysState = ABS_UPNORMAL;
            break;

        case ELS_HOVERED:
            sysState = ABS_UPHOT;
            break;

        case ELS_PRESSED:
            sysState = ABS_UPPRESSED;
            break;

        case ELS_INACTIVE:
            sysState = ABS_UPDISABLED;
            break;

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
                                 SBP_ARROWBTN, scrollbarArrowDir + sysState - ABS_UPNORMAL);
    } break;

    case EL_SCROLLBAR_HTHUMB: {
        sysElem = SBP_THUMBBTNHORZ;
        scrollbarGripperElem = SBP_GRIPPERHORZ;
        goto caseScrollbarThumb;
    } break;

    case EL_SCROLLBAR_VTHUMB: {
        sysElem = SBP_THUMBBTNVERT;
        scrollbarGripperElem = SBP_GRIPPERVERT;
        goto caseScrollbarThumb;
    } break;

    caseScrollbarThumb: {
        switch (state) {
        case ELS_NORMAL:
            sysState = SCRBS_NORMAL;
            break;

        case ELS_HOVERED:
            sysState = SCRBS_HOT;
            break;

        case ELS_PRESSED:
            sysState = SCRBS_PRESSED;
            break;

        case ELS_INACTIVE:
            sysState = SCRBS_DISABLED;
            break;

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
                                 sysElem, sysState);

        const Vector2d center = dest.getCenter();
        const double size = std::min(dest.w(), dest.h());
        target.drawThemedControl(Rect<double>::se(center - Vector2d{size / 2},
                                                  center + Vector2d{size / 2}),
                                 Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
                                 scrollbarGripperElem, sysState);
    } break;

    case EL_SCROLLBAR_HTRACK: {
        // Rect<double> lower = dest;
        // Rect<double> upper = dest;
        // lower.h (lower.h () / 2);
        // upper.y0(lower.y1());
        //
        // target.drawThemedControl(lower, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
        //                          SBP_LOWERTRACKHORZ, SCRBS_NORMAL);
        // target.drawThemedControl(upper, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
        //                          SBP_UPPERTRACKHORZ, SCRBS_NORMAL);

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
                                 SBP_UPPERTRACKHORZ, SCRBS_NORMAL);
    } break;

    case EL_SCROLLBAR_VTRACK: {
        // Rect<double> lower = dest;
        // Rect<double> upper = dest;
        // lower.w (lower.w () / 2);
        // upper.x0(lower.x1());
        //
        // target.drawThemedControl(lower, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
        //                          SBP_LOWERTRACKVERT, SCRBS_NORMAL);
        // target.drawThemedControl(upper, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
        //                          SBP_UPPERTRACKVERT, SCRBS_NORMAL);

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_SCROLLBAR>(),
                                 SBP_UPPERTRACKVERT, SCRBS_NORMAL);
    } break;

    NODEFAULT
    }
}
#pragma endregion Style


#pragma region StyleManager
StyleManager::StyleManager() :
    styles(1) {}

StyleManager::StyleManager(const char *defaultStyleFileName) :
    StyleManager() {
    loadDefaultStyle(defaultStyleFileName);
}

void StyleManager::loadDefaultStyle(const char *fileName) {
    getStyle(ROOT_STYLE_HANDLE).loadTileMaps(fileName);
}
#pragma endregion StyleManager


}
