/**
 * \file src/midout.cpp
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include "midout.h"

// to avoid linking error for empty lib on some platforms
void midout_empty() {
}

#ifdef MIDOUT_PROFILING
#pragma message "midout profiling enabled"

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <mutex>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

namespace {
    class UsedTypes {
        std::unordered_set<const char*> m_types;
        std::mutex m_mtx;

        public:

            void add(const char *type) {
                std::lock_guard<std::mutex> guard{m_mtx};
                m_types.insert(type);
            }

            ~UsedTypes() {
                auto output_fname = getenv("MIDOUT_OUTPUT");
                char output_fname_storage[256];
                if (!output_fname) {
                    output_fname = output_fname_storage;
                    snprintf(output_fname, sizeof(output_fname_storage),
                            "midout_trace.%d", static_cast<int>(getpid()));
                }
                FILE *fout = fopen(output_fname, "w");
                if (!fout) {
                    fprintf(stderr,
                            "midout: failed to open output file %s: %s\n",
                            output_fname, strerror(errno));
                }
                fprintf(fout, "midout_trace v1\n");
                std::vector<std::string> sorted_types;
                sorted_types.reserve(m_types.size());
                for (auto &&i: m_types) {
                    sorted_types.emplace_back(i);
                }
                std::sort(sorted_types.begin(), sorted_types.end());
                for (auto &&i: sorted_types) {
                    fputs(i.c_str(), fout);
                    fputc('\n', fout);
                }
                fclose(fout);
                fprintf(stderr, "midout: %zu items written to %s\n",
                        sorted_types.size(), output_fname);
            }
    };
}

void midout::on_region_used(const std::type_info &type) {
    static UsedTypes used_types;
    used_types.add(type.name());
}

#endif  // MIDOUT_PROFILING

#ifdef MIDOUT_GENERATED
#pragma message "stripping by midout enabled"
#endif
