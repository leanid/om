# How to use it?
## Install c++ package manager `vcpkg`
on Fedora just 
```bash
sudo dnf install vcpkg
```
Next read manual and install vcpkg repository 
```bash
dnf repoquery -l vcpkg
```
find documentation in output:
```bash
/etc/profile.d/vcpkg.sh
/usr/bin/vcpkg
/usr/lib/.build-id
/usr/lib/.build-id/5f
/usr/lib/.build-id/5f/880f3f58398ace7dc63853bd44c675d0c52bde
/usr/lib/.build-id/b5
/usr/lib/.build-id/b5/47ef3689d6be56e9b4f9510e921180df1f4c02
/usr/share/doc/vcpkg
/usr/share/doc/vcpkg/README.fedora
/usr/share/doc/vcpkg/README.md
/usr/share/licenses/vcpkg
/usr/share/licenses/vcpkg/LICENSE.txt
/usr/share/licenses/vcpkg/NOTICE.txt
```
```bash
cat /usr/share/doc/vcpkg/README.fedora
```
and finally see:
```bash
git clone https://github.com/microsoft/vcpkg $VCPKG_ROOT
```
## First create vcpkg.json + vcpkg-configuration.json files

``` cmd
vcpkg new --application
vcpkg add port boost-program-options
vcpkg add port openssl
```
## Second add packages with dependencies like boost[icu]
``` cmd
vcpkg add port boost-locale[icu]
```
this will create:

``` json
{
  "dependencies": [
    {
      "name": "boost-locale",
      "features": [
        "icu"
      ]
    },
    "boost-program-options",
    "openssl"
  ]
}
```

