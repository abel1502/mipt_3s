#include <ACL/general.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ACL/event.h>


void callback() {
    DBG("Called back!");
}


int main() {
    abel::verbosity = 2;

    abel::Event<void()> event{};

    event.subscribe(callback);

    event.invoke();

    return 0;
}
