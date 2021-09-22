#include "gui/gui.h"
#include <ACL/general.h>
#include "raycaster.h"


/// Has to be employed because of gui frameworks' implications
MAIN_DECL {
    abel::verbosity = 2;

    MAIN_TRY(
        Window window{};
        Texture texture{window};
        Scene scene{};

        MAIN_EVENT_LOOP(
            scene.tick(DELTA_T);

            scene.render(texture.getBuf(), texture.width(), texture.height());
            texture.update();

            window.render(texture);
            window.update();

            DBG("Tick.");
        , 10)

        return 0;
    )
}
