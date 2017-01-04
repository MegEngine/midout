#include "midout.h"

#ifdef MIDOUT_PROFILING
#include <vector>
#include <unordered_set>
#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <unistd.h>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

std::string demangle(const char* name) {
    int status;

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    if (status) {
        fprintf(stderr, "midout: failed to demangle\n");
        abort();
    }

    return res.get();
}

#else
#error "unsupported compiler: can not demangle type name"
#endif

namespace {
    class UsedTypes {
        std::unordered_set<const char*> m_types;

        public:

            void add(const char *type) {
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
                std::vector<std::string> sorted_types;
                sorted_types.reserve(m_types.size());
                for (auto &&i: m_types) {
                    sorted_types.emplace_back(demangle(i));
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
    UsedTypes g_used_types;
}

void midout::on_region_used(const std::type_info &type) {
    g_used_types.add(type.name());
}

#endif  // MIDOUT_PROFILING
