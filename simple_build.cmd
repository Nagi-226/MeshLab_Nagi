@echo off
cd /d "E:\Github Project\MeshLab\MeshLab-src\build_m2_test"
"C:\Program Files\CMake\bin\cmake.exe" --build . --target filter_normal_enhance --config Release
if %errorlevel% equ 0 (
    echo 构建成功！
    dir src\meshlabplugins\filter_normal_enhance\Release\*.dll
) else (
    echo 构建失败！
)
pause