#include <ACL/general.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ACL/signal.h>
#include <ACL/list.h>

//#define UNSUB_


#ifdef UNSUB_
bool callback() {
    DBG("Called back!");

    return true;
}
#else
void callback() {
    DBG("Called back!");
}
#endif


int main() {
    abel::verbosity = 2;

    #ifdef UNSUB_
    abel::Signal<bool ()> event{};
    #else
    abel::Signal<void ()> event{};
    #endif

    event += callback;
    event += callback;

    event();
    event();

    return 0;
}
