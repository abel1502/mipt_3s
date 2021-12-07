#ifndef AGF_HELPERS_WINDOW_REF_H
#define AGF_HELPERS_WINDOW_REF_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/window.h>
#include <ACL/type_traits.h>
#include <ACL/unique_ptr.h>
#include <ACL/vector.h>


namespace abel::gui {


template <typename T>
class WidgetRef;

template <typename T>
class WidgetRefMgr;


namespace _impl {


template <typename T>
class WidgetRefMgr {
    static_assert(std::is_base_of_v<Widget, T>);
public:
    using type = T;
    using widget_ref_type = WidgetRef<type>;


    static WidgetRefMgr &getInstance() {
        if (!instance) {
            instance = new WidgetRefMgr();
        }

        return *instance;
    }

protected:
    friend class widget_ref_type;

    static unique_ptr<WidgetRefMgr> instance = nullptr;


    vector<widget_ref_type &> managedReferences{};


    static void onDeath(Widget &widget);


    WidgetRefMgr() {};

    bool isHookedToWidget(const type &widget) const noexcept;

    void     hookToWidget(type &widget);
    void unhookFromWidget(type &widget);


    // ======================================
    // Exposed only to WidgetRef<T>:
    void   registerRef(widget_ref_type &ref);
    void unregisterRef(widget_ref_type &ref);
    // ======================================

};


}


template <typename T>
class WidgetRef {
    static_assert(std::is_base_of_v<Widget, T>);
public:
    DECLARE_ERROR(error, abel::error)

    using type = T;


    WidgetRef(type *target_);

    inline WidgetRef(type &target_) :
        WidgetRef(&target) {}

    WidgetRef(const WidgetRef &other);
    WidgetRef &operator=(const WidgetRef &other);

    WidgetRef(WidgetRef &&other) noexcept;
    WidgetRef &operator=(WidgetRef &&other) noexcept;

    ~WidgetRef();


    constexpr bool isAlive() const noexcept {
        return target;
    }

    constexpr bool isDead() const noexcept {
        return !isAlive();
    }

    constexpr operator bool() const noexcept {
        return isAlive();
    }

    constexpr type &operator*() const noexcept {
        if (isDead())
            throw error("dead reference dereferencing");

        return *target;
    }

    constexpr type *operator->() const noexcept {
        return target;
    }

    void reset(type *newTarget = nullptr);

    constexpr unsigned getHash() const {
        return hash;
    }

protected:
    friend class WidgetRefMgr<T>;

    static unsigned curHash;

    type *target = nullptr;
    const unsigned hash = curHash++;


    inline static WidgetRefMgr<T> &getManager() {
        return WidgetRefMgr<T>::getInstance();
    }

};


namespace _impl {

template <typename T>
void WidgetRefMgr<T>::registerRef(widget_ref_type &ref) {
    if (ref.isDead()) {
        ERR("Dead ref registration attempted. Skipping");

        return;
    }

    managedReferences.append(ref);
    hookToWidget(*ref);
}

template <typename T>
void WidgetRefMgr<T>::unregisterRef(widget_ref_type &ref) {
    managedReferences.filterUnordered(
        [hash = ref.getHash()](const widget_ref_type &other) {
            return other.getHash() == hash;
        }
    );

    // TODO: Perhaps unhook from non-referenced widgets?
}

template <typename T>
void WidgetRefMgr<T>::hookToWidget(type &widget) {
    if (isHookedToWidget(widget)) {
        return;
    }

    widget.sigDeath += onDeath;
}

template <typename T>
void WidgetRefMgr<T>::unhookFromWidget(type &widget) {
    widget.sigDeath -= onDeath;
}

}  // namespace _impl


template <typename T>
unsigned WidgetRef<T>::curHash = 0x1000;


}

#endif // AGF_HELPERS_WINDOW_REF_H
