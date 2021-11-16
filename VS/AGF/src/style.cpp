#include <AGF/llgui.h>
#include <AGF/style.h>
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

void Style::TileMap::assemble(Texture &target, const Rect<double> &at) {
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
const unsigned Style::DEFAULT_TEXT_FORMAT = Texture::DEFAULT_TEXT_FORMAT;

#pragma region Parsing
static bool skipSpace(std::istream &stream) {
    int lastChar = 0;

    while (stream.good() && isspace(lastChar = stream.get())) {}
    
    if (!isspace(lastChar) && lastChar > 0)
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
            readDelim(stream, ','))
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
            indexFile.get();
            continue;
        }

        if (readToken(indexFile, elemName,  BUF_LIMIT) ||
            readToken(indexFile, elemState, BUF_LIMIT)) {
            ERROR_;
        }

        if (std::string_view("+") == elemState) {
            markElemSysDrawn(elemByName(elemName));
        } else if (std::string_view("-") == elemState) {
            markElemSysDrawn(elemByName(elemName), false);
        } else {
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
                        Element element, ElementState state) {
    if (isElemSysDrawn(element)) {
        return sysDrawElement(target, dest, element, state);
    }

    TileMap &generator = getTileMap(element, state);

    if (!generator.isPresent()) {
        ERR("No tilemap defined for %u %u", element, state);  // TODO: String names!

        throw error("Tilemap not found");
    }

    generator.assemble(target, dest);
}

void Style::sysDrawElement(Texture &target, const Rect<double> &dest,
                           Element element, ElementState state) {
    unsigned sysState = 0;
    unsigned wndHeaderButton = 0;

    switch (element) {
    case EL_WND_HEADER: {
        switch (state) {
        case ELS_NORMAL:
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
        target.drawRect(dest.padded(wndBorderWidth, wndBorderWidth, 0, wndBorderWidth), Color::WHITE);
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

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_BUTTON>(),
                                 BP_PUSHBUTTON, sysState);
    } break;

    case EL_CLOSE_BTN: {
        wndHeaderButton = WP_CLOSEBUTTON;
        goto caseHeaderButton;
    } break;

    case EL_MINIMIZE_BTN: {
        wndHeaderButton = WP_MINBUTTON;
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

        NODEFAULT
        }

        target.drawThemedControl(dest, Window::getInstance().getTheme<Window::WT_WINDOW>(),
                                 wndHeaderButton, sysState);
    } break;

    NODEFAULT
    }
}
#pragma endregion Style


#pragma region StyleManager
StyleManager::StyleManager() {
    styles.appendEmplace();
}

StyleManager::StyleManager(const char *defaultStyleFileName) :
    StyleManager() {
    loadDefaultStyle(defaultStyleFileName);
}

void StyleManager::loadDefaultStyle(const char *fileName) {
    getStyle(ROOT_STYLE_HANDLE).loadTileMaps(fileName);
}
#pragma endregion StyleManager


}
