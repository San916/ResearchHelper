cmake_minimum_required(VERSION 3.20)

project(ResearchHelper LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============== Reusable paths ==============
set(UNITY_DIR tests/unity)
set(TESTS_DIR tests)
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

# ============== Setup unity ==============
add_library(unity STATIC ${UNITY_DIR}/src/unity.c)
target_include_directories(unity PUBLIC ${UNITY_DIR}/src)

# ============== Set sources ==============
set(SOURCES
    ${SOURCE_DIR}/main.c
    ${SOURCE_DIR}/server/handlers.c
    ${SOURCE_DIR}/server/http_errors.c
    ${SOURCE_DIR}/server/http.c
    ${SOURCE_DIR}/server/router.c
    ${SOURCE_DIR}/server/server.c
    ${SOURCE_DIR}/utils/utils.c
)

# ============== Set includes ==============
set(INCLUDES
    ${INCLUDE_DIR}/server
    ${INCLUDE_DIR}/utils
)

# ============== Main executable ==============
add_executable(App ${SOURCES})
target_include_directories(App PRIVATE ${INCLUDES})
target_link_libraries(App Ws2_32)

# ============== Tests with unity ==============

# ========== Unit test utils ==========
add_executable(test_utils
    ${TESTS_DIR}/unit/test_utils.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_utils PRIVATE 
    ${INCLUDES}
    ${UNITY_DIR}/src
)
target_link_libraries(test_utils unity)

# ========== Unit test http ==========
add_executable(test_http
    ${TESTS_DIR}/unit/test_http.c
    ${SOURCE_DIR}/server/http.c
    ${SOURCE_DIR}/server/http_errors.c
    ${SOURCE_DIR}/utils/utils.c
)
target_include_directories(test_http PRIVATE 
    ${INCLUDES}
    ${UNITY_DIR}/src
)
target_link_libraries(test_http unity)

# ========== Unit test router ==========
add_executable(test_router
    ${TESTS_DIR}/unit/test_router.c
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

# ========== Add tests ==========
enable_testing()
add_test(NAME test_utils COMMAND test_utils)
add_test(NAME test_http COMMAND test_http)
add_test(NAME test_router COMMAND test_router)