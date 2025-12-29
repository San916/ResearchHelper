cmake_minimum_required(VERSION 3.20)

project(ResearchHelper C)

# Set sources
set(SOURCES
    src/main.c
    src/server/server.c
)

# Set includes
set(INCLUDES
    include/server
)

# Create executable
add_executable(App ${SOURCES})

# Get includes
target_include_directories(App PRIVATE ${INCLUDES})

# Libraries
target_link_libraries(App Ws2_32)