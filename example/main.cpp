#include "midout.h"
#include <cstdio>
#include <string>

MIDOUT_DECL(Opr);

enum class Opr {
    ADD, SUB
};

namespace calc {
    template<Opr opr>
    int kern_impl(int a, int b);

    template<>
    __attribute__((noinline))
    int kern_impl<Opr::ADD>(int a, int b) {
        return a + b;
    }

    template<>
    __attribute__((noinline))
    int kern_impl<Opr::SUB>(int a, int b) {
        return a - b;
    }

    template<Opr opr>
    int kern(int a, int b) {
        MIDOUT_BEGIN(Opr, midout_iv(opr)) {
            return kern_impl<opr>(a, b);
        } MIDOUT_END();
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <num0> <num1> <+/->\n"
                "    to compute sum/difference of two numbers\n",
                argv[0]);
        return -1;
    }
    int a = std::stoi(argv[1]),
        b = std::stoi(argv[2]),
        c;

    switch (argv[3][0]) {
        case '+':
            c = calc::kern<Opr::ADD>(a, b);
            break;
        case '-':
            c = calc::kern<Opr::SUB>(a, b);
            break;
        default:
            fprintf(stderr, "bad opr\n");
            return 2;
    }

    printf("result=%d\n", c);
}

