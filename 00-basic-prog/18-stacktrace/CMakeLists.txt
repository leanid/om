cmake_minimum_required(VERSION 3.20...3.22)

project(18-stacktrace CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_ENABLE_EXPORTS OFF)

add_executable(main_std_stacktrace main_std_stacktrace.cxx)
target_link_libraries(
    main_std_stacktrace
    PRIVATE
        $<$<AND:$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},12.1.1>,$<CXX_COMPILER_ID:GNU>>:stdc++_libbacktrace>
    )

# set(CMAKE_CXX_VISIBILITY_PRESET)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)
# set(CMAKE_ENABLE_EXPORTS ON)

# find_package(Boost REQUIRED)
# add_executable(main_boost_stacktrace main_boost_stacktrace.cxx)
# target_link_libraries(main_boost_stacktrace PRIVATE $<$<PLATFORM_ID:Linux>:dl>)
# target_compile_definitions(main_boost_stacktrace
# PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
# target_compile_definitions(main_boost_stacktrace
#                            PRIVATE BOOST_STACKTRACE_USE_ADDR2LINE)
# target_link_libraries(main_boost_stacktrace PRIVATE Boost::stacktrace)
