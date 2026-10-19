<#
.SYNOPSIS
    Packages M-Cowork for Windows into a standalone zip in dist/
#>
param()
$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$dist = Join-Path $root 'dist'
$pkgName = "m-cowork-windows-x86_64"
$stage = Join-Path $dist $pkgName

Write-Host "==> Building targets on Windows..."
& (Join-Path $root 'scripts\build.ps1')

Write-Host "==> Staging distribution files..."
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory -Force (Join-Path $stage 'bin') | Out-Null
New-Item -ItemType Directory -Force (Join-Path $stage 'tools\matricX-cowork') | Out-Null

Copy-Item (Join-Path $root 'build\m_cowork_ui.exe') (Join-Path $stage 'bin\') -Force
Copy-Item (Join-Path $root 'tools\matricX-cowork\matricX-cowork.exe') (Join-Path $stage 'tools\matricX-cowork\') -Force
if (Test-Path (Join-Path $root 'tools\matricX-cowork\akar-icons_cursor.png')) {
    Copy-Item (Join-Path $root 'tools\matricX-cowork\akar-icons_cursor.png') (Join-Path $stage 'tools\matricX-cowork\') -Force
}
Copy-Item (Join-Path $root 'assets') $stage -Recurse -Force

# Optional: Run windeployqt if installed in MSYS2 UCRT64
$windeployqt = "C:\msys64\ucrt64\bin\windeployqt.exe"
if (Test-Path $windeployqt) {
    Write-Host "==> Deploying Qt runtime DLLs via windeployqt..."
    & $windeployqt --release --no-translations (Join-Path $stage 'bin\m_cowork_ui.exe')
}

# Create zip package
$zipPath = Join-Path $dist "$pkgName.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Compress-Archive -Path "$stage\*" -DestinationPath $zipPath
Write-Host "==> Packaged Windows distribution ready: $zipPath"
