# How to use it?
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

