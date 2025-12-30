cmake_minimum_required(VERSION 3.20)

project(ResearchHelper LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set sources
set(SOURCES
    src/main.c
    src/server/handlers.c
    src/server/http_errors.c
    src/server/http.c
    src/server/router.c
    src/server/server.c
    src/utils/utils.c
)

# Set includes
set(INCLUDES
    include/server
    include/utils
)

# Create executable
add_executable(App ${SOURCES})

# Get includes
target_include_directories(App PRIVATE ${INCLUDES})

# Libraries
target_link_libraries(App Ws2_32)