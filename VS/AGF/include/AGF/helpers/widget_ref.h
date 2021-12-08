#ifndef AGF_HELPERS_WINDOW_REF_H
#define AGF_HELPERS_WINDOW_REF_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/window.h>
#include <ACL/type_traits.h>
#include <ACL/unique_ptr.h>
#include <ACL/vector.h>
#include <set>


namespace abel::gui {


class WidgetRef;


namespace _impl {


class WidgetRefMgr {
public:
    static inline WidgetRefMgr &getInstance() {
        if (!instance) {
            instance = new WidgetRefMgr();
        }

        return *instance;
    }

protected:
    friend class WidgetRef;

    static unique_ptr<WidgetRefMgr> instance;


    std::set<WidgetRef *> managedReferences{};


    static void onDeath(Widget &widget);


    WidgetRefMgr();

    bool isHookedToWidget(const Widget &widget) const noexcept;

    void     hookToWidget(Widget &widget);
    void unhookFromWidget(Widget &widget);

    // =======================================
    // Exposed only to WidgetRef:
    void   registerRef(WidgetRef *ref);
    void unregisterRef(WidgetRef *ref);

    void refMoved(WidgetRef *from,
                  WidgetRef *to);

    void refTargetChanged(WidgetRef *ref,
                          Widget *oldTarget,
                          Widget *newTarget);
    // =======================================

};

}  // namespace _impl


class WidgetRef {
public:
    DECLARE_ERROR(error, abel::error)


    WidgetRef(Widget *target_ = nullptr);

    inline WidgetRef(Widget &target_) :
        WidgetRef(&target_) {}

    inline WidgetRef(const WidgetRef &other) :
        WidgetRef(other.get()) {}

    inline WidgetRef &operator=(const WidgetRef &other) {
        reset(other.get());

        return *this;
    }

    inline WidgetRef(WidgetRef &&other) {
        move(std::move(other));
    }

    inline WidgetRef &operator=(WidgetRef &&other) {
        reset();

        move(std::move(other));

        return *this;
    }

    inline ~WidgetRef() {
        reset();
    }


    constexpr bool isAlive() const noexcept {
        return target;
    }

    constexpr bool isDead() const noexcept {
        return !isAlive();
    }

    constexpr operator bool() const noexcept {
        return isAlive();
    }

    constexpr Widget &operator*() const {
        if (isDead())
            throw error("dead reference dereferencing");

        return *target;
    }

    constexpr Widget *operator->() const noexcept {
        return target;
    }

    constexpr Widget *get() const noexcept {
        return target;
    }

    void reset();

    void reset(Widget *newTarget);

protected:
    friend class _impl::WidgetRefMgr;

    Widget *target = nullptr;


    void move(WidgetRef &&other);

    inline static _impl::WidgetRefMgr &getManager() {
        return _impl::WidgetRefMgr::getInstance();
    }

    // =======================================
    // Exposed only to WidgetRefMgr:
    inline void died() {
        target = nullptr;

        // No sync-back to the manager,
        // because it was the one to invoke us
    }
    // =======================================

};


template <typename T>
class WidgetRefTo : public WidgetRef {
    static_assert(std::is_base_of_v<Widget, T>);
public:
    using type = T;
    using nonconst_type = std::remove_const_t<type>;


    inline WidgetRefTo(type *target_) :
        WidgetRef(const_cast<nonconst_type *>(target_)) {}

    inline WidgetRefTo(type &target_) :
        WidgetRefTo(&target_) {}

    template <typename U>
    inline WidgetRefTo(const WidgetRefTo<U> &other) :
        WidgetRefTo(other.get()) {

        static_assert(std::is_convertible_v<U *, type *>);
    }

    template <typename U>
    WidgetRefTo &operator=(const WidgetRefTo<U> &other) {
        static_assert(std::is_convertible_v<U *, type *>);

        reset(other.get());

        return *this;
    }

    template <typename U>
    inline WidgetRefTo(WidgetRefTo<U> &&other) :
        WidgetRef(std::move(other)) {
        static_assert(std::is_convertible_v<U *, type *>);
    }

    template <typename U>
    WidgetRefTo &operator=(WidgetRefTo<U> &&other) {
        static_assert(std::is_convertible_v<U *, type *>);

        WidgetRef::operator=(std::move(other));

        return *this;
    }



    constexpr type &operator*() const noexcept {
        return dynamic_cast<type &>(WidgetRef::operator*());
    }

    constexpr type *operator->() const noexcept {
        return dynamic_cast<type *>(WidgetRef::operator->());
    }

    constexpr type *get() const noexcept {
        return dynamic_cast<type *>(WidgetRef::get());
    }

    inline void reset(type *newTarget = nullptr) {
        return WidgetRef::reset(const_cast<nonconst_type *>(newTarget));
    }

protected:
    //

};


template <>
class WidgetRefTo<Widget> : public WidgetRef {};

template <typename T>
using ConstWidgetRefTo = WidgetRefTo<const T>;

using ConstWidgetRef = ConstWidgetRefTo<Widget>;


extern template
class WidgetRefTo<Widget>;

extern template
class WidgetRefTo<const Widget>;


}

#endif // AGF_HELPERS_WINDOW_REF_H
