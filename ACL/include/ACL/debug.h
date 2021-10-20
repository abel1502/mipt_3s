#ifndef ACL_DEBUG_H
#define ACL_DEBUG_H

#include <ACL/general.h>
#include <cstdlib>


namespace abel {


class OpTracer {
public:
    OpTracer() {
        msg("ctor");

        doCtor();
    }

    OpTracer(const OpTracer &other) {
        msg("copy ctor", other);

        doCtor();
    }

    OpTracer &operator=(const OpTracer &other) {
        msg("copy asgn", other);

        doAsgn();

        return *this;
    }

    OpTracer(OpTracer &&other) {
        msg("move ctor", other);

        doCtor();
    }

    OpTracer &operator=(OpTracer &&other) {
        msg("move asgn", other);

        doAsgn();

        return *this;
    }

    ~OpTracer() {
        msg("dtor");

        doDtor();
    }


    void doCtor() {
        if (balance != 0)
            bad();

        balance++;
    }

    void doDtor() {
        if (balance != 1)
            bad();

        balance--;
    }

    void doAsgn() {
        if (balance != 1)
            bad();
    }

    void msg(const char *src) {
        DBG("OpTracer [%p].%d %s\n", this, balance, src);
    }

    void msg(const char *src, const OpTracer &other) {
        DBG("OpTracer [%p].%d %s [%p].%d\n", this, balance, src, &other, other.balance);
    }

    void bad() {
        ERR("OpTracer bad usage!!");
        abort();
    }

protected:
    int balance = 0;

};


}


#endif // ACL_DEBUG_H
