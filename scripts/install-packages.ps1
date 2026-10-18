# Installs the packages listed in packages\msys2-packages.txt into MSYS2 (UCRT64).
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$list = Get-Content (Join-Path $root 'packages\msys2-packages.txt') |
    ForEach-Object { $_.Trim() } | Where-Object { $_ -and -not $_.StartsWith('#') }
$env:MSYSTEM = 'UCRT64'
$env:CHERE_INVOKING = '1'
& 'C:\msys64\usr\bin\bash.exe' -lc ('pacman -S --needed --noconfirm ' + ($list -join ' '))
if ($LASTEXITCODE -ne 0) { throw 'pacman failed.' }