#ifndef AGF_WIDGETS_STATIC_GROUP_H
#define AGF_WIDGETS_STATIC_GROUP_H

#include <AGF/widget.h>
#include <ACL/list.h>
#include <ACL/type_traits.h>
#include <ACL/template_trinkets.h>
#include <ACL/debug.h>
#include <tuple>


#define SGRP_DECLARE_BINDING_I(NAME, IDX)                               \
    inline const auto &NAME()      const { return child   <IDX>(); }    \
    inline       auto &NAME()            { return child   <IDX>(); }    \
    inline const auto &NAME##Ptr() const { return childPtr<IDX>(); }    \
    inline       auto &NAME##Ptr()       { return childPtr<IDX>(); }

#define SGRP_DECLARE_BINDING_T(NAME, TYPE) \
    SGRP_DECLARE_BINDING_I(NAME, Types::idx<TYPE>)


namespace abel::gui::widgets {


namespace _impl {
    template <typename ... Ts>
    struct are_all_widgets;

    template <>
    struct are_all_widgets<> : public std::true_type {};

    template <typename T, typename ... Ts>
    struct are_all_widgets<T, Ts...> : public std::bool_constant<
                                        std::is_base_of_v<Widget, T> &&
                                        are_all_widgets<Ts...>::value
                                      > {};

    template <typename ... Ts>
    constexpr bool are_all_widgets_v = are_all_widgets<Ts...>::value;
}


// TODO: Maybe inherit virtually
template <typename ... Ts>
class StaticGroup : public /*virtual*/ Widget {
public:
    static_assert(_impl::are_all_widgets_v<Ts...>);
    using Types = TypeArray<Ts...>;

    EVENT_HANDLER_USING(Widget)

    #define EVENTS_DSL_ITEM_(NAME) \
        EVENT_HANDLER_OVERRIDE(NAME)
    #include <AGF/events.dsl.h>

protected:
    std::tuple<unique_ptr<Ts>...> children{};

    template <typename ... As>
    StaticGroup(Widget *parent_, const Rect<double> &region_, As &&... args) :
        Widget(parent_, region_), children{std::forward<As>(args)...} {

        // TODO: Change children's parents...?
    }

    template <unsigned I>
    constexpr const unique_ptr<typename Types::item<I> > &childPtr() const noexcept {
        return std::get<I>(children);
    }

    template <unsigned I>
    constexpr unique_ptr<typename Types::item<I> > &childPtr() noexcept {
        return std::get<I>(children);
    }

    template <unsigned I>
    const typename Types::item<I> &child() const {
        return *std::get<I>(children);
    }

    template <unsigned I>
    typename Types::item<I> &child() {
        return *std::get<I>(children);
    }

    template <unsigned I = 0>
    bool areChildrenSet() const noexcept {
        if constexpr (I < Types::size) {
            return childPtr<I>() != nullptr && areChildrenSet<I + 1>();
        }

        return true;
    }

    template <typename T, unsigned I = 0>
    EventStatus _dispatchToChildren(const T &event) {
        static_assert(!std::is_same_v<T, RenderEvent>, "There should have been a separate mechanism "
                                                       "propagating render events in reverse");

        EventStatus status = dispatchToChild(child<I>(), event);

        if constexpr (I + 1 < Types::size) {
            if (status.shouldHandle(status.SIBL)) {
                return _dispatchToChildren<T, I + 1>(event);
            }
        }

        return status;
    }

    template <unsigned I = 0>
    EventStatus _dispatchToChildren(const RenderEvent &event) {
        if constexpr (I + 1 < Types::size) {
            EventStatus status = _dispatchToChildren<I + 1>(event);

            if (!status.shouldHandle(status.SIBL)) {
                return status;
            }
        }

        return dispatchToChild(child<I>(), event);
    }

    // TODO: Inline?
    template <typename T>
    EventStatus _processEvent(const T &event) {
        /*DBG("[%p] got %s",
            this,
            getTypeName<T>().data());

        if constexpr (std::is_same_v<T, MouseClickEvent>) {
            DBG("Mouse pos: %ld %lg", event.pos.x(), event.pos.y());
        }*/

        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE))
            return status.update();

        REQUIRE(areChildrenSet());

        return _dispatchToChildren(event).update();
    }

};


#define EVENTS_DSL_ITEM_(NAME)                      \
    template <typename ... Ts>                      \
    EVENT_HANDLER_IMPL(StaticGroup<Ts...>, NAME) {  \
        return _processEvent(event);                \
    }

#include <AGF/events.dsl.h>


}


#endif // AGF_WIDGETS_STATIC_GROUP_H


