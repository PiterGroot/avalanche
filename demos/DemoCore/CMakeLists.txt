file(GLOB_RECURSE DEMOCORE_SOURCES "DemoCore/src/*.cpp")
file(GLOB_RECURSE DEMOCORE_HEADERS "DemoCore/include/*.hpp")

add_library(DemoCore STATIC ${DEMOCORE_SOURCES} ${DEMOCORE_HEADERS})

target_link_libraries(DemoCore PUBLIC avalanche)

target_include_directories(DemoCore 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)