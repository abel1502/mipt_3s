#include <AGF/llgui.h>
#include <AGF/helpers/widget_ref.h>


namespace abel::gui {


namespace _impl {

unique_ptr<WidgetRefMgr> WidgetRefMgr::instance = nullptr;


WidgetRefMgr::WidgetRefMgr() {}

void WidgetRefMgr::registerRef(WidgetRef *ref) {
    assert(ref);

    if (ref->isDead()) {
        ERR("Dead ref registration attempted. Skipping");

        return;
    }

    managedReferences.insert(ref);
    hookToWidget(**ref);
}

void WidgetRefMgr::unregisterRef(WidgetRef *ref) {
    managedReferences.erase(ref);

    // TODO: Perhaps unhook from non-referenced widgets?
}

void WidgetRefMgr::refTargetChanged(WidgetRef *ref, Widget *oldTarget, Widget *newTarget) {
    // auto pos = managedReferences.find(ref);

    // if (pos == managedReferences.end()) {
    //     ERR("Got target change notice from an unmanaged reference");
    //     return;
    // }

    // TODO: Maybe unhook from oldTarget, if not null

    if (newTarget) {
        hookToWidget(*newTarget);
    }
}

void WidgetRefMgr::refMoved(WidgetRef *from, WidgetRef *to) {
    managedReferences.erase(from);
    managedReferences.insert(to);
}

void WidgetRefMgr::hookToWidget(Widget &widget) {
    if (isHookedToWidget(widget)) {
        return;
    }

    widget.sigDeath += onDeath;
}

void WidgetRefMgr::unhookFromWidget(Widget &widget) {
    widget.sigDeath -= onDeath;
}

bool WidgetRefMgr::isHookedToWidget(const Widget &widget) const noexcept {
    return widget.sigDeath.hasCallback(onDeath);
}

void WidgetRefMgr::onDeath(Widget &widget) {
    for (WidgetRef *ref : getInstance().managedReferences) {
        assert(ref);

        if (ref->get() == &widget) {
            ref->died();
        }
    }


}

}  // namespace _impl


WidgetRef::WidgetRef(Widget *target_) :
    target{target_} {

    getManager().registerRef(this);
}

void WidgetRef::reset() {
    getManager().unregisterRef(this);

    target = nullptr;
}

void WidgetRef::reset(Widget *newTarget) {
    getManager().refTargetChanged(this, target, newTarget);

    target = newTarget;
}

void WidgetRef::move(WidgetRef &&other) {
    assert(!target);

    getManager().refMoved(&other, this);

    std::swap(target, other.target);
}


template
class WidgetRefTo<Widget>;

template
class WidgetRefTo<const Widget>;


}

