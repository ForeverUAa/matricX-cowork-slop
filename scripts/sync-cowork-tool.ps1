# Builds matricX-cowork into tools\matricX-cowork\matricX-cowork.exe
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$dest = Join-Path $root 'tools\matricX-cowork'
$src  = Join-Path $dest 'matricX-cowork.cpp'
$exe  = Join-Path $dest 'matricX-cowork.exe'

if (-not (Test-Path $src)) {
    throw "matricX-cowork.cpp not found at $src"
}

$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
Write-Host "Building matricX-cowork in $dest..."
& g++ -std=c++17 -O2 -Wall -Wextra -static $src -o $exe -luser32 -lgdi32 -lgdiplus -lwinmm
if ($LASTEXITCODE -ne 0) { throw "Build failed for matricX-cowork" }
Write-Host "matricX-cowork built at $exe"
