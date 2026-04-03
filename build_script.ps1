# 激活VS开发环境的脚本
Push-Location "$env:VS2026\Common7\Tools"
.\Launch-VsDevShell.ps1 -Arch amd64 -SkipAutomaticLocation
Pop-Location

# 设置Qt路径
$env:CMAKE_PREFIX_PATH = "E:\Qt\5.15.2\msvc2019_64"

# 进入MeshLab源码目录
Set-Location "E:\Github Project\MeshLab\MeshLab-src"

# 创建构建目录
if (Test-Path "build_m2_test") {
    Remove-Item -Recurse -Force build_m2_test
}
mkdir build_m2_test
Set-Location build_m2_test

# 运行CMake配置
Write-Host "正在配置CMake..." -ForegroundColor Green
cmake .. -G Ninja -DCMAKE_PREFIX_PATH="$env:CMAKE_PREFIX_PATH"

if ($LASTEXITCODE -eq 0) {
    Write-Host "CMake配置成功！正在构建插件..." -ForegroundColor Green
    # 构建插件
    cmake --build . --target filter_normal_enhance --config Release
} else {
    Write-Host "CMake配置失败！" -ForegroundColor Red
}