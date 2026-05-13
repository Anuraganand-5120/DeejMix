$ErrorActionPreference = "Stop"

Write-Host "Setting up Qt environment..." -ForegroundColor Cyan
$env:PATH = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.0\mingw_64\bin;" + $env:PATH

Write-Host "Configuring Release Build..." -ForegroundColor Cyan
cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64" -DCMAKE_C_COMPILER="C:/Qt/Tools/mingw1310_64/bin/gcc.exe" -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe"

Write-Host "Compiling DeejMix..." -ForegroundColor Cyan
cmake --build build_release --parallel 4

Write-Host "Creating deployment folder..." -ForegroundColor Cyan
$deployDir = ".\DeejMix_Release"
if (Test-Path $deployDir) {
    Remove-Item -Recurse -Force $deployDir
}
New-Item -ItemType Directory -Path $deployDir | Out-Null

Write-Host "Copying executable..." -ForegroundColor Cyan
Copy-Item ".\build_release\DeejMix.exe" -Destination "$deployDir\"

Write-Host "Running windeployqt to bundle dependencies..." -ForegroundColor Cyan
# windeployqt automatically copies all the required Qt DLLs and plugins
windeployqt.exe --no-translations --no-opengl-sw "$deployDir\DeejMix.exe"

Write-Host "Done! The folder '$deployDir' is ready to be shared." -ForegroundColor Green
