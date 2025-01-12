cmake_minimum_required(VERSION 3.31)

project(18-stacktrace CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_ENABLE_EXPORTS OFF)

include(CheckIncludeFileCXX)
check_include_file_cxx("<stacktrace>" has_cxx_23_stacktrace_include)

if(has_cxx_23_stacktrace_include)
    add_executable(main_std_stacktrace main_std_stacktrace.cxx)
    # cmake-format: off
    target_link_libraries(
    main_std_stacktrace
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:
        $<IF:$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},14.0.0>,
            stdc++exp,
            $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},12.1.1>:
                stdc++_libbacktrace>
            >
        >)
     # cmake-format: on
else()
    message(STATUS "skip <stacktrace> example no such header file")
endif(has_cxx_23_stacktrace_include)

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
