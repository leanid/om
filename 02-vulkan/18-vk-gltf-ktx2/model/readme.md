# How to create ktx2 texture from png
1. build deps/prebuild/linux.../bin/ktx - binary
2. convert from png like (add `--generate-mipmap` to embed the full mip
   chain - the renderer expects mip levels in the file):
```sh
./deps/prebuilt/linux-clang23-x86_64/bin/ktx create --format R8G8B8A8_SRGB --encode basis-lz --assign-tf srgb --generate-mipmap 02-vulkan/18-vk-gltf-ktx2/model/viking_room.png 02-vulkan/18-vk-gltf-ktx2/model/viking_room.ktx2
ktx create warning: No color primaries in PNG input file "02-vulkan/18-vk-gltf-ktx2/model/viking_room.png", defaulting to BT.709.
```
3. --qlevel <1,255> - arg to change quality, default 128
4. --clevel <0,6>   - arg to change compression, default 1
5. `viking_room.6.255.ktx2` = `--clevel 6 --qlevel 255` (max quality) +
   `--generate-mipmap` -> 11 mip levels, ETC1S, PSNR vs png ~34 dB
   (ETC1S is lossy even at max settings; UASTC `--encode uastc` gives
   ~50 dB at ~3.5x size, also runtime-transcodable)

# How to validate see result of ktx2 texture
1. open in browser: `https://sandbox.babylonjs.com/`
2. drag and drop `02-vulkan/18-vk-gltf-ktx2/model/viking_room.ktx2`

# Statistics png vs ktx2 (basis-lz)
```sh
  -rw-rw-r-- 1 l-chayka l-chayka 141K Oct  6 12:21 viking_room.255.ktx2
  -rw-rw-r-- 1 l-chayka l-chayka 145K Oct  6 12:28 viking_room.6.255.ktx2
  -rw-rw-r-- 1 l-chayka l-chayka 230K Oct  5 10:01 viking_room.gltf
  -rw-rw-r-- 1 l-chayka l-chayka  98K Oct  6 11:09 viking_room.ktx2
  -rw-rw-r-- 1 l-chayka l-chayka 469K Oct  5 10:01 viking_room.obj
  -rw-rw-r-- 1 l-chayka l-chayka 940K Oct  5 10:01 viking_room.png
```

# If you check out file like:
```sh
$ ./deps/prebuilt/linux-clang23-x86_64/bin/ktx info ./02-vulkan/18-vk-gltf-ktx2/model/viking_room.6.255.ktx2
Validation successful

Header

identifier: «KTX 20»\r\n\x1A\n
vkFormat: VK_FORMAT_UNDEFINED
typeSize: 1
pixelWidth: 1024
pixelHeight: 1024
```
Be advised FORMAT_UNDEFINED - mean - need transcode to real gpu compression format.
