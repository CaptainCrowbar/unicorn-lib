#include "crow/unit-test.hpp"
#include "unicorn/core.hpp"
#include <string>

using namespace std::literals;
using namespace Crow;
using namespace Unicorn;

TEST_MAIN;

namespace {

    void check_version_information() {

        auto v1 = unicorn_version(), v2 = unicode_version();
        std::cout << "... Unicorn version: " << v1 << "\n";
        std::cout << "... Unicode version: " << v2 << "\n";
        TEST_COMPARE(v1, >=, (Version{0,1,0}));
        TEST_COMPARE(v2, >=, (Version{8,0,0}));

    }

}

TEST_MODULE(unicorn, core) {

    check_version_information();

}
