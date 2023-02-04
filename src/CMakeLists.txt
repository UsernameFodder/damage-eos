set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        c2ea58c7f9bb2a1da2d3d7f5b462121ac6a07f16 # v2.3.1
)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG ab6c7375be9a8e71ee84c6f8537113f9f47daf99 # v3.2.1
)

FetchContent_MakeAvailable(json cli11 Catch2)

set(DAMAGE_SOURCES mathutil.cpp mechanics.cpp damage.cpp)
set(DAMAGECALC_NO_MAIN_SOURCES ${DAMAGE_SOURCES} idmap.cpp cfgparse.cpp)

add_library(damage ${DAMAGECALC_NO_MAIN_SOURCES})
target_link_libraries(damage PRIVATE nlohmann_json::nlohmann_json)

# wasm library, disabled by default and must be specified explicitly
# and built with emscripten
add_library(damage.wasm ${DAMAGECALC_NO_MAIN_SOURCES} wasm_bindings.cpp)
target_compile_options(damage.wasm PUBLIC "$<$<CONFIG:Debug>:-fexceptions>")
target_link_libraries(damage.wasm PRIVATE nlohmann_json::nlohmann_json PUBLIC "$<$<CONFIG:Debug>:-fexceptions>")
set_target_properties(damage.wasm PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

add_executable(damagecalc ${DAMAGECALC_NO_MAIN_SOURCES} damagecalc_cli.cpp)
target_link_libraries(damagecalc PRIVATE nlohmann_json::nlohmann_json PRIVATE CLI11::CLI11)

# Tests
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(damage_tests ${DAMAGE_SOURCES} damage_tests.cpp)
target_link_libraries(damage_tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(damage_tests)

add_executable(cfgparse_tests ${DAMAGECALC_NO_MAIN_SOURCES} cfgparse_tests.cpp)
target_link_libraries(cfgparse_tests PRIVATE nlohmann_json::nlohmann_json PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(cfgparse_tests)

add_executable(idmap_tests idmap.cpp idmap_tests.cpp)
target_link_libraries(idmap_tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(idmap_tests)

add_executable(mathutil_tests mathutil.cpp mathutil_tests.cpp)
target_link_libraries(mathutil_tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(mathutil_tests)

add_executable(mechanics_tests mathutil.cpp mechanics.cpp mechanics_tests.cpp)
target_link_libraries(mechanics_tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(mechanics_tests)
