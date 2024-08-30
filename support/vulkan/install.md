# How to install Vulkan SDK to Fedora 39 (nice and easy way)
```bash
dnf install vulkan-loader-devel vulkan-tools vulkan-headers vulkan-validation-layers-devel
```
# How to install Vulkan SDK on Windows (nice and easy way)
on Windows you can just add $env VULKAN_SDK=C:/VulkanSDK/1.3.290.0
if you already have VulkanSDK installed
or:
```cmd
.\vcpkg install vulkan 
```
now you can use Vulkan with cmake like:
```cmd
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```
# How to install Vulkan SDK on macOS (nice and easy way)
```bash
brew install vulkan-tools
```
