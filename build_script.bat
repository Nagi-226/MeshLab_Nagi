@echo off
REM 激活VS2026开发环境
call "E:\VS2026\Common7\Tools\VsDevCmd.bat" -arch=amd64

REM 设置Qt路径
set CMAKE_PREFIX_PATH=E:\Qt\5.15.2\msvc2019_64

REM 进入MeshLab源码目录
cd /d "E:\Github Project\MeshLab\MeshLab-src"

REM 清理旧的构建目录
if exist build_m2_test rmdir /s /q build_m2_test

REM 创建新的构建目录
mkdir build_m2_test
cd build_m2_test

REM 运行CMake配置
echo 正在配置CMake...
cmake .. -G Ninja -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%

if %errorlevel% equ 0 (
    echo CMake配置成功！正在构建插件...
    REM 构建插件
    cmake --build . --target filter_normal_enhance --config Release
) else (
    echo CMake配置失败！
    pause
)