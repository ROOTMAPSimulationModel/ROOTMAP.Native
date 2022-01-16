#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "core/common/RmVersion.cpp"
#include "app/common/FilesystemUtilities.cpp"
#include "app/common/RmGetOpts.h"
#include "app/common/RmGetOpts.cpp"
#include <string>
#include <regex>

using namespace rootmap;

TEST_CASE("Has a valid semantic version", "[RmGetOpts]") {
    auto sut = RmGetOpts::getInstance();
    std::string version = sut.getVersion().ToStdString();

    // David Fichtmueller's semver validation regex from https://github.com/semver/semver/issues/232

    std::regex regex("^(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\\+([0-9a-zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$");
    auto match = std::regex_match(version, regex);
    if (!match)
    {
        FAIL("Version " << version << " does not pass a semantic versioning validation regex.");
    }
    REQUIRE(match);
}
