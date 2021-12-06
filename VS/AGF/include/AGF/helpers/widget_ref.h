#ifndef AGF_HELPERS_WINDOW_REF_H
#define AGF_HELPERS_WINDOW_REF_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/window.h>
#include <ACL/type_traits.h>
#include <ACL/cmp.h>
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


    static WidgetRefMgr &getInstance();

    static void onDeath(Widget &widget);

protected:
    friend class widget_ref_type;

    static unique_ptr<WidgetRefMgr> instance = nullptr;


    vector<widget_ref_type &> managedReferences{};


    WidgetRefMgr();

    void   registerRef(widget_ref_type &ref);
    void unregisterRef(widget_ref_type &ref);

    void hookToWidget(type &widget);

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

    WidgetRef(WidgetRef &&other);
    WidgetRef &operator=(WidgetRef &&other);


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

    void reset(type *newPtr = nullptr);

protected:
    friend class WidgetRefMgr<T>;

    type *target = nullptr;

};


}


#endif // AGF_HELPERS_WINDOW_REF_H
