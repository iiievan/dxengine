

# Training project of the game engine for DirectX11 and CMAKE. 
# MSVC 19.36.32532.0
# HLSL:Windows Kits/10/bin/10.0.26100.0/x64/fxc.exe

1. Assembly via Cmake + MSVS. If you really want to assemble and debug under Visual Studio. You will need to install Windows 10 SDK and run follow:
```
cmake -G "Visual Studio 17 2022" -A x64 -B build -S .
```

2. To format via clang-format  individual library folders run this in folder:
```
Get-ChildItem -Recurse -Include *.h,*.cpp,*.c,*.hpp | ForEach-Object { clang-format -i $_.FullName }
```
3. To build assimp library
```
git clone https://github.com/assimp/assimp.git
cd assimp
git tag 
```
you see something like that
```
...
v5.2.0
v5.2.1
v5.2.2
v5.2.3
v5.2.4
v5.2.5
v5.3.0
v5.3.1
v5.4.0
v5.4.1
v5.4.2
v5.4.3
v6.0.0
v6.0.1
...
```
press 'q' and switch to version:
```
git checkout v5.2.5
```
and build it
```
cmake -B build -DASSIMP_BUILD_SHARED_LIBS=ON -DASSIMP_BUILD_STATIC_LIBS=ON -DASSIMP_DOUBLE_PRECISION=OFF -DASSIMP_NO_EXPORT=OFF -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON -DASSIMP_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target install
```
In folders build/lib and build/bin you need assimp-vc143-mt.lib and assimp-vc143-mt.dll.
Copy all contents from /include/assimp folder of repository
to you project assimp/include/assimp and assimp-vc143-mt.lib,assimp-vc143-mt.dll to assimp/lib  
In folder build/include/assimp you need file config.h to replace original file "assimp/include/assimp/config.h.in"  
in your project. 

