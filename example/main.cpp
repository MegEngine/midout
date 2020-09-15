/**
 * \file example/main.cpp
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include "midout.h"
#include <cstdio>
#include <string>

MIDOUT_DECL(Opr);

enum class Opr {
    ADD, SUB
};

namespace calc {
    template<Opr opr, int iv>
    struct kern_impl;

    template<int iv>
    struct kern_impl<Opr::ADD, iv> {
        __attribute__((noinline))
        static int apply(int a, int b) {
            return a + b + iv;
        }
    };

    template<int iv>
    struct kern_impl<Opr::SUB, iv> {
        __attribute__((noinline))
        static int apply(int a, int b) {
            return a - b + iv;
        }
    };

    template<Opr opr, int iv>
    int kern(int a, int b) {
        MIDOUT_BEGIN(Opr, midout_iv(opr), iv) {
            return kern_impl<opr, iv>::apply(a, b);
        } MIDOUT_END();
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <num0> <num1> <+/-/p/m>\n"
                "    to compute sum/difference of two numbers\n",
                argv[0]);
        return -1;
    }
    int a = std::stoi(argv[1]),
        b = std::stoi(argv[2]),
        c;

    switch (argv[3][0]) {
        case '+':
            c = calc::kern<Opr::ADD, 0>(a, b);
            break;
        case '-':
            c = calc::kern<Opr::SUB, 0>(a, b);
            break;
        case 'p':
            c = calc::kern<Opr::ADD, 1>(a, b);
            break;
        case 'm':
            c = calc::kern<Opr::SUB, 1>(a, b);
            break;
        default:
            fprintf(stderr, "bad opr\n");
            return 2;
    }

    printf("result=%d\n", c);
}

