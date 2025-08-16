

# Training project of the game engine for DirectX11.

1. Assembly via Cmake + MSVS. If you really want to assemble and debug under Visual Studio. You will need to install Windows 10 SDK and run follow:
```
cmake -G "Visual Studio 17 2022" -A x64 -B build -S .
```

2. To format via clang-format  individual library folders run this in folder:
```
Get-ChildItem -Recurse -Include *.h,*.cpp,*.c,*.hpp | ForEach-Object { clang-format -i $_.FullName }
```

