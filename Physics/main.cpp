#include "gui/gui.h"
#include <ACL/general.h>


/// Has to be employed because of gui frameworks' implications
MAIN_DECL {
    abel::verbosity = 2;

    MAIN_TRY(
        Window window{};
        Texture texture{window};

        MAIN_EVENT_LOOP(
            //
        , 0)

        return 0;
    )
}
