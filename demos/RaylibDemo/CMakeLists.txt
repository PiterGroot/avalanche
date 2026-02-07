
file(GLOB_RECURSE RAYLIB_DEMO_SOURCES "code/*.cpp")
file(GLOB_RECURSE RAYLIB_DEMO_HEADERS "code/*.hpp" "code/*.h")
file(GLOB IMGUI_SOURCES "imgui/*.cpp")
file(GLOB RLIMGUI_SOURCES "rlImGui/*.cpp")
file(GLOB PNG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.png")

list(APPEND RAYLIB_DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

add_executable(RaylibDemo 
    ${RAYLIB_DEMO_SOURCES} 
    ${RAYLIB_DEMO_HEADERS}
    ${IMGUI_SOURCES}
    ${RLIMGUI_SOURCES}
)

target_link_libraries(RaylibDemo PRIVATE DemoCore)

target_include_directories(RaylibDemo 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/code
        ${CMAKE_CURRENT_SOURCE_DIR}/raylib/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
        ${CMAKE_CURRENT_SOURCE_DIR}/rlImGui
)

target_link_libraries(RaylibDemo PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/raylib/lib/raylib.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/raylib/lib/raylibdll.lib
)

add_custom_command(TARGET RaylibDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:RaylibDemo>/assets"
)

add_custom_command(TARGET RaylibDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/fa-solid-900.ttf"
        "$<TARGET_FILE_DIR:RaylibDemo>/fa-solid-900.ttf"
)

foreach(PNG_FILE ${PNG_FILES})
    add_custom_command(TARGET RaylibDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PNG_FILE}"
            "$<TARGET_FILE_DIR:RaylibDemo>/assets/"
    )
endforeach()

if(WIN32)
    target_link_libraries(RaylibDemo PRIVATE winmm gdi32 user32)
endif()