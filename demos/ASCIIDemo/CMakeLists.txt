file(GLOB_RECURSE ASCII_DEMO_SOURCES "*.cpp")
file(GLOB_RECURSE ASCII_DEMO_HEADERS "*.hpp")

add_executable(ASCIIDemo ${ASCII_DEMO_SOURCES} ${ASCII_DEMO_HEADERS})

target_link_libraries(ASCIIDemo PRIVATE DemoCore)

target_include_directories(ASCIIDemo 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)