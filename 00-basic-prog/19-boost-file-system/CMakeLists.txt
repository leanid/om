cmake_minimum_required(VERSION 3.20...3.23)

project(19-boost-file-system CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_ENABLE_EXPORTS OFF)

include(FetchContent)

fetchcontent_declare(
    BoostAssert GIT_REPOSITORY https://github.com/boostorg/assert.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostConfig GIT_REPOSITORY https://github.com/boostorg/config.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostContainerHash
    GIT_REPOSITORY https://github.com/boostorg/container_hash.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostCore GIT_REPOSITORY https://github.com/boostorg/core.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostDetail GIT_REPOSITORY https://github.com/boostorg/detail.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostInteger GIT_REPOSITORY https://github.com/boostorg/integer.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostStaticAssert
    GIT_REPOSITORY https://github.com/boostorg/static_assert.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostThrowException
    GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostTypeTraits GIT_REPOSITORY https://github.com/boostorg/type_traits.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostPreprocessor
    GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostIterator GIT_REPOSITORY https://github.com/boostorg/iterator.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(BoostIo GIT_REPOSITORY https://github.com/boostorg/io.git
                     GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostConceptCheck
    GIT_REPOSITORY https://github.com/boostorg/concept_check.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostConversion GIT_REPOSITORY https://github.com/boostorg/conversion.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostFunctionTypes
    GIT_REPOSITORY https://github.com/boostorg/function_types.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostFusion GIT_REPOSITORY https://github.com/boostorg/fusion.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(BoostMpl GIT_REPOSITORY https://github.com/boostorg/mpl.git
                     GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostOptional GIT_REPOSITORY https://github.com/boostorg/optional.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostSmartPtr GIT_REPOSITORY https://github.com/boostorg/smart_ptr.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostUtility GIT_REPOSITORY https://github.com/boostorg/utility.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostTypeof GIT_REPOSITORY https://github.com/boostorg/typeof.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostTuple GIT_REPOSITORY https://github.com/boostorg/tuple.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostPredef GIT_REPOSITORY https://github.com/boostorg/predef.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostMove GIT_REPOSITORY https://github.com/boostorg/move.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostAtomic GIT_REPOSITORY https://github.com/boostorg/atomic.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostVariant2 GIT_REPOSITORY https://github.com/boostorg/variant2.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostAlign GIT_REPOSITORY https://github.com/boostorg/align.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostMp11 GIT_REPOSITORY https://github.com/boostorg/mp11.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostWinapi GIT_REPOSITORY https://github.com/boostorg/winapi.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostSystem GIT_REPOSITORY https://github.com/boostorg/system.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)
fetchcontent_declare(
    BoostFileSystem GIT_REPOSITORY https://github.com/boostorg/filesystem.git
    GIT_TAG boost-1.79.0 GIT_SHALLOW ON GIT_PROGRESS ON)

fetchcontent_makeavailable(
    BoostAssert
    BoostConfig
    BoostInteger
    BoostStaticAssert
    BoostThrowException
    BoostTypeTraits
    BoostPreprocessor
    BoostIo
    BoostIterator
    BoostConceptCheck
    BoostConversion
    BoostFunctionTypes
    BoostFusion
    BoostMpl
    BoostOptional
    BoostSmartPtr
    BoostUtility
    BoostTypeof
    BoostTuple
    BoostPredef
    BoostMove
    BoostAlign
    BoostMp11
    BoostWinapi
    BoostContainerHash
    BoostCore
    BoostDetail
    BoostAtomic
    BoostVariant2
    BoostSystem
    BoostFileSystem)

add_executable(main_boost main.cxx)
target_link_libraries(main_boost PRIVATE Boost::filesystem)
