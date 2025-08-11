cmake -G "Visual Studio 17 2022" -A x64 -B build -S .
Get-ChildItem -Recurse -Include *.h,*.cpp,*.c,*.hpp | ForEach-Object { clang-format -i $_.FullName }