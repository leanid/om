# How to install Vulkan SDK to Fedora 39 (nice and easy way)
```bash
dnf install vulkan-loader-devel vulkan-tools vulkan-headers vulkan-validation-layers-devel
```
# How to install Vulkan SDK on Windows (nice and easy way)
```cmd
.\vcpkg install vulkan 
```
now you can use Vulkan with cmake like:
```cmd
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```
