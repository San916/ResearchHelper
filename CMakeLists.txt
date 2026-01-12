cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
project(ResearchHelper LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============== Reusable paths ==============
set(UNITY_DIR tests/unity)
set(TEST_DIR tests)
set(TEST_UTILS_DIR tests/testutils)
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

# ============== Setup unity ==============
add_library(unity STATIC ${UNITY_DIR}/src/unity.c)
target_include_directories(unity PUBLIC ${UNITY_DIR}/src)

# ============== Setup curl ==============
find_package(CURL CONFIG REQUIRED)

# ============== Set sources ==============
set(SOURCES
    ${SOURCE_DIR}/main.c
    ${SOURCE_DIR}/server/handlers.c
    ${SOURCE_DIR}/server/http_errors.c
    ${SOURCE_DIR}/server/http.c
    ${SOURCE_DIR}/server/router.c
    ${SOURCE_DIR}/server/server.c
    ${SOURCE_DIR}/backend/web_crawler.c
    ${SOURCE_DIR}/utils/utils.c
)

# ============== Set includes ==============
set(INCLUDES
    ${INCLUDE_DIR}/server
    ${INCLUDE_DIR}/backend
    ${INCLUDE_DIR}/utils
)

# ============== Main executable ==============
add_executable(App ${SOURCES})
target_include_directories(App PRIVATE ${INCLUDES})
target_link_libraries(App Ws2_32 CURL::libcurl)

# ============== Tests with unity ==============

add_compile_definitions(
    UNITY_OUTPUT_COLOR
)

# ========== Unit test utils ==========
add_executable(test_utils
    ${TEST_DIR}/unit/test_utils.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_utils PRIVATE 
    ${INCLUDES}
    ${UNITY_DIR}/src
)
target_link_libraries(test_utils unity)

# ========== Unit test http ==========
add_executable(test_http
    ${TEST_DIR}/unit/test_http.c
    ${TEST_UTILS_DIR}/src/test_http_utils.c
    ${SOURCE_DIR}/server/http.c
    ${SOURCE_DIR}/server/http_errors.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_http PRIVATE 
    ${INCLUDES}
    ${TEST_UTILS_DIR}/include
    ${UNITY_DIR}/src
)
target_link_libraries(test_http unity)

# ========== Unit test router ==========
add_executable(test_router
    ${TEST_DIR}/unit/test_router.c
    ${SOURCE_DIR}/server/router.c
    ${SOURCE_DIR}/server/http.c
    ${SOURCE_DIR}/server/http_errors.c
    ${SOURCE_DIR}/server/handlers.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_router PRIVATE 
    ${INCLUDES}
    ${UNITY_DIR}/src
)
target_link_libraries(test_router unity Ws2_32)

# ========== Unit test web crawler ==========
add_executable(test_web_crawler
    ${TEST_DIR}/unit/test_web_crawler.c
    ${TEST_UTILS_DIR}/src/test_http_utils.c
    ${SOURCE_DIR}/backend/web_crawler.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_web_crawler PRIVATE
    ${INCLUDES}
    ${UNITY_DIR}/src
)
target_link_libraries(test_web_crawler unity CURL::libcurl)

# ========== Unit test json ==========
add_executable(test_json
    ${TEST_DIR}/unit/test_json.c
    ${SOURCE_DIR}/utils/json.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_json PRIVATE 
    ${INCLUDES}
    ${UNITY_DIR}/src
)
target_link_libraries(test_json unity)

# ========== Add tests ==========
enable_testing()

add_test(NAME test_utils COMMAND test_utils)
add_test(NAME test_http COMMAND test_http)
add_test(NAME test_router COMMAND test_router)
add_test(NAME test_web_crawler COMMAND test_web_crawler)
add_test(NAME test_json COMMAND test_json)
