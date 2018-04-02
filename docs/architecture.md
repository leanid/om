# OM Game Engine Architecture
## Basic overview
### Keywords

  - *User* - game creator, implementor of ```om::game``` interface developer.
  - *Developer* - Om game engine implementor
  - *Game* - library (.dll, .dylib, .so) with one exported function:
  ```std::unique_ptr<om::game*> create_game(om::engine& om);```
  - *Engine* - executable (om.exe, om) provides all services for *User* for 
  easylly game creation (graphics, input, sound, resource loading, job system)
  
### Restrictions for Om Developer

  - Use modern c++17 standard and style from: [CppCoreGuidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)
  - Write as simple as possible (KISS principle)
  - Every feature starting from it's test
  - Strong distingues *User* code and *Developer* code, make everything for
  easy *User* live
  - Add to *User* public api interface features *Developer* can garantee on all
  supported platforms
  - Add to *User* public api platform dependent interface with prefix 
  ```android_service& om::android_some_system_service()```
  - All calls from Engine to Game(```om::game```) in one thread 
  - Do not use code-format on third_paty libraries

### Restrictions for Om User

  - Use on::engine only in one thread same thread where 
  ```std::unique_ptr<om::game*> create_game(om::engine& om);``` called
  
  
## File System Structure

    /om
       /bin             <-- Public output for engine and tools binary
       /docs
       /include
           /om          <-- Public *User* api
       /src
           /om_src      <-- Private headers and sources
           /third_paty  <-- Private external code
               /include 
               /src
       /om_tests        <-- Test for every new feature for Om engine
            /01-test    <-- Every test has it's own mini project in separate directory

## Public Includes Structure

    /om
       /om.hxx            <-- all engine public interface
       /graphics.hxx      <-- window + graphics part (opengl es 2.0 wrapper)
       /input.hxx         <-- input (gamepads, keyboard, mouse, joystick)
       /sound.hxx         <-- openal wrapper
       /jobs.hxx          <-- job system (usefull to load all files on level loading)
       /resources.hxx     <-- hir level api to load (textures, sprites, audio...)
       /platform.hxx      <-- platform dependent api
       /system.hxx        <-- log, etc...
       /text_input.hxx    <-- utf8 text input api in any place on screen
   
   
## Pimpl - usage example for any Om resource type

For *User* every should be simple and easy to use. Let's take texture example.
*User* request texture from Om engine and see just simple wraper around
uint64_t. *User* can do anything with such object like with ```int``` or 
```std::string``` copy, compare, store, move, lock memory and unlock because 
we know exactly it will be on *main thread only*

    // User point of view on om::texture resource
    using handle = std::uint64_t;
    
    class texture
    {
    public:
        texture();
        texture(const texture&);
        //... more constructors
        uint32 get_width() const;
        uint32 get_height() const;
        std::string_view get_file_path() const;
    private:
        handle h = 0; // reinterpret as pointer or index in private buffer of engine
    };
    
    // Developer point of view on texture resource
    
    struct gles_texture
    {
       engine* e;
       std::string_view source_path;
       uint32 ref_counter;
       uint32 width;
       uint32 height;
       uint32 opengl_handle;
       // ... any data *Developer* need
    }   

## Resource System

First subsystem to implement in engine will be Resource System - async file 
loading and parse as textures, json, sprites, animations, sounds, music etc...

TODO...