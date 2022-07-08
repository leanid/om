cmake_minimum_required(VERSION 3.20...3.23)

project(19-boost-file-system CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_ENABLE_EXPORTS OFF)

include(FetchContent)

FetchContent_Declare(
  BoostAssert
  GIT_REPOSITORY https://github.com/boostorg/assert.git
  GIT_TAG        f1a6f9ffd1a0161fc18fad36829033c570cf5ac3  # v1.79.0
)
FetchContent_Declare(
  BoostConfig
  GIT_REPOSITORY https://github.com/boostorg/config.git
  GIT_TAG        08dced51e9a688ec3e9c1b7c5f90c99f2b1cdca7  # v1.79.0
)
FetchContent_Declare(
  BoostContainerHash
  GIT_REPOSITORY https://github.com/boostorg/container_hash.git
  GIT_TAG        355603c0c2a56013d7e44f3a0952004f4f280b39  # v1.79.0
)
FetchContent_Declare(
  BoostCore
  GIT_REPOSITORY https://github.com/boostorg/core.git
  GIT_TAG        dbefea96316465c501bcb853642cc855b03a462b  # v1.79.0
)
FetchContent_Declare(
  BoostDetail
  GIT_REPOSITORY https://github.com/boostorg/detail.git
  GIT_TAG        8c603a1894ec1d8abc7a0a538297923b0ac98b33  # v1.79.0
)
FetchContent_Declare(
  BoostInteger
  GIT_REPOSITORY https://github.com/boostorg/integer.git
  GIT_TAG        49195acf608190f6a1cf43bce3b472e6fadce0a2  # v1.79.0
)
FetchContent_Declare(
  BoostStaticAssert
  GIT_REPOSITORY https://github.com/boostorg/static_assert.git
  GIT_TAG        ba72d3340f3dc6e773868107f35902292f84b07e  # v1.79.0
)
FetchContent_Declare(
  BoostThrowException
  GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
  GIT_TAG        68d092f4d7aafcc7dfbcc5f408d78bcb9e5289ec  # v1.79.0
)
FetchContent_Declare(
  BoostTypeTraits
  GIT_REPOSITORY https://github.com/boostorg/type_traits.git
  GIT_TAG        d2a4a6bf0a3900e11faaf6904b95183115bac54d  # v1.79.0
)
FetchContent_Declare(
  BoostPreprocessor
  GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
  GIT_TAG        d4e82d7d3f2e2adbe280966ac51ce8f9372f5a44  # v1.79.0
)
FetchContent_Declare(
  BoostIterator
  GIT_REPOSITORY https://github.com/boostorg/iterator.git
  GIT_TAG        d175ba245026adf22d7cea7d992079162f5110ea  # v1.79.0
)
FetchContent_Declare(
  BoostIo
  GIT_REPOSITORY https://github.com/boostorg/io.git
  GIT_TAG        c68387509bf96681643833ca4e2003a78256e09a  # v1.79.0
)
FetchContent_Declare(
  BoostConceptCheck
  GIT_REPOSITORY https://github.com/boostorg/concept_check.git
  GIT_TAG        e34c735a1a6902de0d3e20ea58cfd8f101702458  # v1.79.0
)
FetchContent_Declare(
  BoostConversion
  GIT_REPOSITORY https://github.com/boostorg/conversion.git
  GIT_TAG        4ae8b4d49547126264f0b80c528fcfd26cdf5e7d  # v1.79.0
)
FetchContent_Declare(
  BoostFunctionTypes
  GIT_REPOSITORY https://github.com/boostorg/function_types.git
  GIT_TAG        895335874d67987ada0d8bf6ca1725e70642ed49  # v1.79.0
)
FetchContent_Declare(
  BoostFusion
  GIT_REPOSITORY https://github.com/boostorg/fusion.git
  GIT_TAG        500e4c120ff2f2414ec225a2a42eb1a665c79956  # v1.79.0
)
FetchContent_Declare(
  BoostMpl
  GIT_REPOSITORY https://github.com/boostorg/mpl.git
  GIT_TAG        5f79900dcc35823de3fe5f403b0eed6dbced31e1  # v1.79.0
)
FetchContent_Declare(
  BoostOptional
  GIT_REPOSITORY https://github.com/boostorg/optional.git
  GIT_TAG        5a444eb84b67a5035e3577476234189c57ca55c1  # v1.79.0
)
FetchContent_Declare(
  BoostSmartPtr
  GIT_REPOSITORY https://github.com/boostorg/smart_ptr.git
  GIT_TAG        f2cc84a23c64b8a73c9b72b34799d0854d7e0787  # v1.79.0
)
FetchContent_Declare(
  BoostUtility
  GIT_REPOSITORY https://github.com/boostorg/utility.git
  GIT_TAG        69f6588d7cf7b392ab8e7eac8eb92cdd1a90d77a  # v1.79.0
)
FetchContent_Declare(
  BoostTypeof
  GIT_REPOSITORY https://github.com/boostorg/typeof.git
  GIT_TAG        46c7a05f826fc020ee88210ea2a5cd9278b930ab  # v1.79.0
)
FetchContent_Declare(
  BoostTuple
  GIT_REPOSITORY https://github.com/boostorg/tuple.git
  GIT_TAG        500e4fa0a2845b96c0dd919e7485e0f216438a01  # v1.79.0
)
FetchContent_Declare(
  BoostPredef
  GIT_REPOSITORY https://github.com/boostorg/predef.git
  GIT_TAG        392e4e767469e3469c9390f0d9cca16724dc3fc8  # v1.79.0
)
FetchContent_Declare(
  BoostMove
  GIT_REPOSITORY https://github.com/boostorg/move.git
  GIT_TAG        208df9f3c338674697614c125b4d98d9cc446173  # v1.79.0
)
FetchContent_Declare(
  BoostAtomic
  GIT_REPOSITORY https://github.com/boostorg/atomic.git
  GIT_TAG        a17267547071e0dd60c81945bcb6bf0162a5db07  # v1.79.0
)
FetchContent_Declare(
  BoostVariant2
  GIT_REPOSITORY https://github.com/boostorg/variant2.git
  GIT_TAG        d34658f4c8480c3a71fb435121f68e25ca166eab  # v1.79.0
)
FetchContent_Declare(
  BoostAlign
  GIT_REPOSITORY https://github.com/boostorg/align.git
  GIT_TAG        bef21416a3bbd46f3f5de4f9254bf4c46ec752fe  # v1.79.0
)
FetchContent_Declare(
  BoostMp11
  GIT_REPOSITORY https://github.com/boostorg/mp11.git
  GIT_TAG        7bc4e1ae9b36ec8ee635c3629b59ec525bbe82b9  # v1.79.0
)
FetchContent_Declare(
  BoostWinapi
  GIT_REPOSITORY https://github.com/boostorg/winapi.git
  GIT_TAG        3fa8b0ace1774536905b559fc4d681b8c0971b02  # v1.79.0
)
FetchContent_Declare(
  BoostSystem
  GIT_REPOSITORY https://github.com/boostorg/system.git
  GIT_TAG        e197c5e80373736cabc319b2d4fa0657e7821e87  # v1.79.0
)
FetchContent_Declare(
  BoostFileSystem
  GIT_REPOSITORY https://github.com/boostorg/filesystem.git
  GIT_TAG        123fe5faa635a521a32bcffc5c6f5ebf9f03e581  # v1.79.0
)

FetchContent_MakeAvailable(BoostAssert BoostConfig BoostInteger BoostStaticAssert BoostThrowException
  BoostTypeTraits BoostPreprocessor BoostIo BoostIterator BoostConceptCheck BoostConversion BoostFunctionTypes
  BoostFusion BoostMpl BoostOptional BoostSmartPtr BoostUtility BoostTypeof BoostTuple BoostPredef BoostMove
  BoostAlign BoostMp11 BoostWinapi
  BoostContainerHash BoostCore BoostDetail BoostAtomic BoostVariant2 BoostSystem BoostFileSystem)

add_executable(main_boost main.cxx)
target_link_libraries(main_boost PRIVATE Boost::filesystem)
