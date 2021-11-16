#ifndef AGF_STYLE_H
#define AGF_STYLE_H

#include <ACL/general.h>
#include <AGF/llgui_pre.h>
#include <ACL/vector.h>
#include <string_view>


namespace abel::gui {


class Style {
protected:
    struct TileMap;

public:
    DECLARE_ERROR(error, abel::error)

    enum Element {
        #define STYLE_DSL_ELEM_(NAME, INDEX_NAME) \
            EL_##NAME,
        #include <AGF/style.dsl.h>

        EL_COUNT,  ///< Not a valid enum value!
    };

    enum ElementState {
        #define STYLE_DSL_ELEMSTATE_(NAME, INDEX_NAME) \
            ELS_##NAME,
        #include <AGF/style.dsl.h>

        ELS_COUNT,  ///< Not a valid enum value!
    };

    // Expands to Texture::DEFAULT_TEXT_FORMAT.
    // Use the former where possible, since it
    // is constexpr, unlike the definition given here
    static const unsigned DEFAULT_TEXT_FORMAT;


    Color textColor{};
    double textSize = 14;
    unsigned textFormat = DEFAULT_TEXT_FORMAT;
    double wndHeaderHeight = 30;
    double wndBorderWidth = 5;


    constexpr Style() noexcept = default;  // Warning: requires some tilemaps to be set manually

    constexpr bool isElemSysDrawn(Element elem) const noexcept {
        assert(elem < EL_COUNT);

        return sysDrawnElementsMask & (1 << elem);
    }

    constexpr void markElemSysDrawn(Element elem, bool value = true) noexcept {
        assert(elem < EL_COUNT);

        if (value)
            sysDrawnElementsMask |=  (1 << elem);
        else
            sysDrawnElementsMask &= ~(1 << elem);
    }

    constexpr void markAllSysDrawn(bool value = true) noexcept {
        sysDrawnElementsMask = value ? -1u : 0;
    }

    void loadTileMaps(const char *indexFileName);

    void drawElement(Texture &target, const Rect<double> &dest,
                     Element element, ElementState state);

    void animElement(Texture &target, const Rect<double> &dest, Element element,
                     ElementState state0, ElementState state1, double stage);

    inline void drawText(Texture &target, const Rect<double> &dest, const char *text) {
        drawText(target, dest, text, textFormat);
    }

    void drawText(Texture &target, const Rect<double> &dest, const char *text, unsigned format);

protected:
    struct TileMap {
        enum Corner {
            C_TOP_LEFT     = 0b00,
            C_TOP_RIGHT    = 0b01,
            C_BOTTOM_LEFT  = 0b10,
            C_BOTTOM_RIGHT = 0b11,
        };

        enum Side {
            S_TOP,
            S_BOTTOM,
            S_LEFT,
            S_RIGHT,
        };


        unique_ptr<Texture> texture = nullptr;
        Rect<double> outer{},
                     inner{};
        bool shouldFillInner = true;


        constexpr TileMap() noexcept = default;  // Means that this instance is unused

        TileMap(const char *srcFileName, const Rect<double> &outer_, const Rect<double> &inner_,
                bool shouldFillInner_ = true);

        void assemble(Texture &target, const Rect<double> &at);

        constexpr Rect<double> getCorner(bool x, bool y) const noexcept {
            return Rect<double>::se(outer.getVertex(x, y), inner.getVertex(x, y));
        }

        constexpr Rect<double> getTargetCorner(const Rect<double> &at, bool x, bool y) const noexcept {
            return Rect<double>::se(at.getVertex(x, y), getTargetBody(at).getVertex(x, y));
        }

        constexpr Rect<double> getCorner(Corner idx) const noexcept {
            assert(idx < 4);
            return getCorner(idx & 0b01, idx & 0b10);
        }

        constexpr Rect<double> getTargetCorner(const Rect<double> &at, Corner idx) const noexcept {
            assert(idx < 4);
            return getTargetCorner(at, idx & 0b01, idx & 0b10);
        }
        
        constexpr Rect<double> getSide(Side idx) const noexcept {
            switch (idx) {
            case S_TOP:
                return Rect<double>::se(inner.x0(), outer.y0(),
                                        inner.x1(), inner.y0());
            case S_BOTTOM:
                return Rect<double>::se(inner.x0(), inner.y1(),
                                        inner.x1(), outer.y1());
            case S_LEFT:
                return Rect<double>::se(outer.x0(), inner.y0(),
                                        inner.x0(), inner.y1());
            case S_RIGHT:
                return Rect<double>::se(inner.x1(), inner.y0(),
                                        outer.x1(), inner.y1());
            NODEFAULT
            }
        }

        constexpr Rect<double> getTargetSide(const Rect<double> &at, Side idx) const noexcept {
            Rect<double> atBody = getTargetBody(at);

            switch (idx) {
            case S_TOP:
                return Rect<double>::se(atBody.x0(),     at.y0(),
                                        atBody.x1(), atBody.y0());
            case S_BOTTOM:
                return Rect<double>::se(atBody.x0(), atBody.y1(),
                                        atBody.x1(),     at.y1());
            case S_LEFT:
                return Rect<double>::se(    at.x0(), atBody.y0(),
                                        atBody.x0(), atBody.y1());
            case S_RIGHT:
                return Rect<double>::se(atBody.x1(), atBody.y0(),
                                            at.x1(), atBody.y1());
            NODEFAULT
            }
        }

        constexpr Rect<double> getTargetSideFirstTile(const Rect<double> &at, Side idx) const noexcept {
            constexpr Corner SIDE_TO_CORNER[4] = {C_TOP_LEFT, C_BOTTOM_LEFT, C_TOP_LEFT, C_TOP_RIGHT};

            Corner cornerIdx = SIDE_TO_CORNER[idx];

            return getSide(idx) + (getTargetCorner(at, cornerIdx).getPos() -
                                   getCorner          (cornerIdx).getPos());
        }

        constexpr Vector2d getTargetSideStep(Side idx) const noexcept {
            switch (idx) {
            case S_LEFT:
            case S_RIGHT:
                return Vector2d{0, getBody().getDiag().y()};

            case S_TOP:
            case S_BOTTOM:
                return Vector2d{getBody().getDiag().x(), 0};

            NODEFAULT
            }
        }

        constexpr const Rect<double> &getBody() const noexcept {
            return inner;
        }

        constexpr Rect<double> getTargetBody(const Rect<double> &at) const noexcept {
            return at.padded(getSidePad(S_LEFT), getSidePad(S_RIGHT ),
                             getSidePad(S_TOP ), getSidePad(S_BOTTOM));
        }

        constexpr double getSidePad(Side idx) const noexcept {
            switch (idx) {
            case S_TOP:
                return inner.y0() - outer.y0();

            case S_BOTTOM:
                return outer.y1() - inner.y1();

            case S_LEFT:
                return inner.x0() - outer.x0();

            case S_RIGHT:
                return outer.x1() - inner.x1();

            NODEFAULT
            }
        }

        constexpr bool isPresent() const noexcept {
            return texture;
        }

    };


    unsigned sysDrawnElementsMask = 0;
    TileMap tileMaps[EL_COUNT][ELS_COUNT] = {};


    constexpr TileMap &getTileMap(Element elem, ElementState state) {
        assert(elem  <  EL_COUNT);
        assert(state < ELS_COUNT);

        return tileMaps[elem][state];
    }

    constexpr const TileMap &getTileMap(Element elem, ElementState state) const {
        return const_cast<Style *>(this)->getTileMap(elem, state);
    }

    constexpr Element elemByName(const std::string_view &name) const {
        #define STYLE_DSL_ELEM_(NAME, INDEX_NAME)   \
            if (name == INDEX_NAME)                 \
                return EL_##NAME;
        #include <AGF/style.dsl.h>

        ERR("Unknown element: %s", name.data());
        throw error("Unknown element");
    }

    constexpr ElementState elemStateByName(const std::string_view &name) const {
        #define STYLE_DSL_ELEMSTATE_(NAME, INDEX_NAME)  \
            if (name == INDEX_NAME)                     \
                return ELS_##NAME;
        #include <AGF/style.dsl.h>

        ERR("Unknown element state: %s", name.data());
        throw error("Unknown element state");
    }

    void sysDrawElement(Texture &target, const Rect<double> &dest,
                        Element element, ElementState state);

};


class StyleManager {
public:
    using StyleHandle = unsigned;
    static constexpr StyleHandle ROOT_STYLE_HANDLE = 0;


    StyleManager();

    StyleManager(const char *defaultStyleFileName);

    void loadDefaultStyle(const char *fileName);

    inline Style &getStyle(StyleHandle handle) {
        demandValidHandle(handle);

        return styles[handle];
    }

    inline const Style &getStyle(StyleHandle handle) const {
        return const_cast<StyleManager *>(this)->getStyle(handle);
    }

    StyleHandle createStyle(StyleHandle parent = ROOT_STYLE_HANDLE) {
        demandValidHandle(parent);

        // styles.append(styles[parent]);
        styles.appendEmplace();  // TODO: Change to incorporate inhereting default values

        return (StyleHandle)(styles.getSize() - 1);
    }

    inline bool validateHandle(StyleHandle handle) const {
        return handle < styles.getSize();
    }

protected:
    vector<Style> styles{};

    inline void demandValidHandle(StyleHandle handle) const {
        if (!validateHandle(handle)) {
            throw error("Invalid handle");
        }
    }

};


}


#endif