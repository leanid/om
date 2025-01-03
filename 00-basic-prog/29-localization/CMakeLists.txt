cmake_minimum_required(VERSION 3.30)

project(29-localization CXX)

find_package(
    Boost
    # 1.86.0
    # EXACT # Minimum or EXACT version e.g. 1.67.0
    # REQUIRED # Fail with error if Boost is not found
    COMPONENTS locale # Boost libraries by their canonical name
    # REQUIRED
    # e.g. "date_time" for "libboost_date_time"
    #[OPTIONAL_COMPONENTS <libs>...]
    # Optional Boost libraries by their canonical name)
    ) # e.g. "date_time" for "libboost_date_time"

if(NOT Boost_FOUND)
    message(STATUS "Boost and/or required components not found"
                   " skip this example")
    return()
endif()

add_executable(29-localization main.cxx)
target_link_libraries(29-localization PRIVATE Boost::locale)
target_compile_features(29-localization PRIVATE cxx_std_23)

find_program(XGETTEXT_EXECUTABLE xgettext)
if(NOT XGETTEXT_EXECUTABLE)
    message(STATUS "xgettext not found skip generation of .pot file")
    return()
endif()

message(STATUS "xgettext found: ${XGETTEXT_EXECUTABLE}")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/messages.pot
    COMMAND xgettext -o ${CMAKE_CURRENT_SOURCE_DIR}/messages.pot
            --add-comments="TRANSLATORS:"
            --keyword=translate
            --keyword=translate:1,2
            --keyword=translate:1,2,3 # --flag=translate:1:pass-c-format
            --directory=${CMAKE_CURRENT_SOURCE_DIR}
            # --join-existing
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx
    VERBATIM
    COMMENT "generating localization template [messages.pot] from source main.cxx"
)
add_custom_target(generate_messages ALL DEPENDS messages.pot)

add_dependencies(29-localization generate_messages)
