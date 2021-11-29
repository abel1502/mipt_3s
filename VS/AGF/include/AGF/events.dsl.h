/// #define EVENTS_DSL_ITEM_(NAME)

#ifndef EVENTS_DSL_ITEM_
#error "This header should only be used as DSL"
#endif

EVENTS_DSL_ITEM_(Render)
EVENTS_DSL_ITEM_(Move)
//EVENTS_DSL_ITEM_(Resize)
EVENTS_DSL_ITEM_(FocusUpdate)
EVENTS_DSL_ITEM_(Start)
EVENTS_DSL_ITEM_(Exit)
EVENTS_DSL_ITEM_(MouseClick)
EVENTS_DSL_ITEM_(MouseMove)
EVENTS_DSL_ITEM_(KeyPress)
EVENTS_DSL_ITEM_(KeyboardInput)
EVENTS_DSL_ITEM_(User)

#undef EVENTS_DSL_ITEM_
