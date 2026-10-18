file(GLOB IKUN_LOG_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)
list(SORT IKUN_LOG_TEST_SOURCES)

set(IKUN_LOG_TEST_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test)


foreach(TEST_SOURCE IN LISTS IKUN_LOG_TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    add_executable("${TEST_NAME}" "${TEST_SOURCE}")

    target_link_libraries("${TEST_NAME}" PRIVATE ikun_log::ikun_log)

    target_compile_options("${TEST_NAME}" PRIVATE
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Debug>:-ggdb>
    )

    # ===== Release/RelWithDebInfo 共用选项 =====
    target_compile_options("${TEST_NAME}" PRIVATE
        $<$<CONFIG:Release>:-DNDEBUG>
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:Release>:-fno-omit-frame-pointer>
        
        $<$<CONFIG:RelWithDebInfo>:-DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:-O2>
        $<$<CONFIG:RelWithDebInfo>:-g>
        $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>
    )

    set_target_properties("${TEST_NAME}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${IKUN_LOG_TEST_OUTPUT_DIRECTORY}"
    )

    foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER "${CONFIG}" CONFIG_UPPER)
        set_target_properties("${TEST_NAME}" PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER}" "${IKUN_LOG_TEST_OUTPUT_DIRECTORY}"
        )
    endforeach()

    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
endforeach()
