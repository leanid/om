# How to create ktx2 texture from png
1. build deps/prebuild/linux.../bin/ktx - binary
2. convert from png like:
```sh
./deps/prebuilt/linux-clang21-x86_64/bin/ktx create --format R8G8B8A8_SRGB --encode basis-lz --assign-tf srgb 02-vulkan/18-vk-gltf-ktx2/model/viking_room.png 02-vulkan/18-vk-gltf-ktx2/model/viking_room.ktx2
ktx create warning: No color primaries in PNG input file "02-vulkan/18-vk-gltf-ktx2/model/viking_room.png", defaulting to BT.709.
```

# How to validate see result of ktx2 texture
1. open in browser: `https://sandbox.babylonjs.com/`
2. drag and drop `02-vulkan/18-vk-gltf-ktx2/model/viking_room.ktx2`

# Statistics png vs ktx2 (basis-lz)
  -rw-rw-r-- 1 l-chayka l-chayka  98K Oct  6 11:09 viking_room.ktx2
  -rw-rw-r-- 1 l-chayka l-chayka 940K Oct  5 10:01 viking_room.png
  -rw-rw-r-- 1 l-chayka l-chayka 469K Oct  5 10:01 viking_room.obj
  -rw-rw-r-- 1 l-chayka l-chayka 230K Oct  5 10:01 viking_room.gltf
