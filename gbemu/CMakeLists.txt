set(MAIN_SOURCES
    main.cpp
)

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")

add_executable(gbemu ${HEADERS} ${MAIN_SOURCES})

target_link_libraries(gbemu emu)

target_include_directories(gbemu PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

message(STATUS "SDL Libraries: ${SDL2_LIBRARIES} - ${SDL2_LIBRARY}")
message(STATUS "SDL TTF Libraries: ${SDL2_TTF_LIBRARIES} - ${SDL2_TTF_LIBRARY}")


install(TARGETS gbemu
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)


# Copy font next to executable
add_custom_command(
    TARGET gbemu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/NotoSansMono-Medium.ttf
        $<TARGET_FILE_DIR:gbemu>
    VERBATIM
)


# Windows DLLs
if(WIN32)

    if(${CMAKE_SIZEOF_VOID_P} MATCHES 8)

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2/lib/x64/SDL2.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf/lib/x64/SDL2_ttf.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf/lib/x64/zlib1.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf/lib/x64/libfreetype-6.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

    else()

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2/lib/x86/SDL2.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf/lib/x86/SDL2_ttf.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf/lib/x86/zlib1.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

        add_custom_command(
            TARGET gbemu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf/lib/x86/libfreetype-6.dll
                $<TARGET_FILE_DIR:gbemu>
            VERBATIM
        )

    endif()

endif()
