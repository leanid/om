cmake_minimum_required(VERSION 3.9)
project(03-sdl-loop-to-engine)

add_executable(${PROJECT_NAME} main.cxx engine.cxx engine.hxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_package(sdl2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    # find out what libraries are needed for staticaly linking with libSDL.a
    # using mingw64 cross-compiler
    
    #$> $ /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --static-libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main 
    #-lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 
    #-lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid 
    #-static-libgcc
    
    target_link_libraries(${PROJECT_NAME} 
               -lmingw32 
               -lSDL2main 
               "${SDL2_LIB}" # full path to libSDL2.a force to staticaly link with it
               -mwindows
               -Wl,--no-undefined
               -lm
               -ldinput8
               -ldxguid
               -ldxerr8
               -luser32 
               -lgdi32
               -lwinmm
               -limm32
               -lole32
               -loleaut32
               -lshell32
               -lversion
               -luuid
               -static-libgcc
               )
elseif(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
