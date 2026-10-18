# Builds M-Cowork into build\m_cowork_ui.exe (incremental). Use -Clean for a full rebuild.
param([switch]$Clean)
$ErrorActionPreference = 'Stop'

$root  = Split-Path -Parent $PSScriptRoot
$src   = Join-Path $root 'src'
$build = Join-Path $root 'build'
$obj   = Join-Path $build 'obj'
$exe   = Join-Path $build 'm_cowork_ui.exe'

$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
if ($Clean -and (Test-Path $build)) { Remove-Item $build -Recurse -Force }
New-Item -ItemType Directory -Force $obj | Out-Null

$cflags = @((& pkg-config --cflags Qt6Widgets Qt6Svg Qt6Network) -split '\s+' | Where-Object { $_ })
if ($LASTEXITCODE -ne 0) { throw 'Qt 6 Widgets/Svg/Network not found. Run scripts\install-packages.ps1 first.' }
$libs = @((& pkg-config --libs Qt6Widgets Qt6Svg Qt6Network) -split '\s+' | Where-Object { $_ })

# Run moc on headers with Q_OBJECT
$moc = "C:\msys64\ucrt64\bin\moc.exe"
if (Test-Path $moc) {
    foreach ($h in Get-ChildItem $src -Recurse -Filter *.h) {
        $content = Get-Content $h.FullName -Raw
        if ($content -match 'Q_OBJECT') {
            $mocCpp = Join-Path $obj ('moc_' + $h.BaseName + '.cpp')
            $stale = (-not (Test-Path $mocCpp)) -or ((Get-Item $mocCpp).LastWriteTime -lt $h.LastWriteTime)
            if ($stale) {
                Write-Host "moc      $($h.Name)"
                & $moc $h.FullName -o $mocCpp -I $src
            }
            $o = Join-Path $obj ('moc_' + $h.BaseName + '.o')
            $objects += $o
            if ((-not (Test-Path $o)) -or ((Get-Item $o).LastWriteTime -lt (Get-Item $mocCpp).LastWriteTime)) {
                & g++ -std=c++17 -O1 -Wall -I $src @cflags -c $mocCpp -o $o
            }
        }
    }
}

$newestHeader = (Get-ChildItem $src -Recurse -Filter *.h | Measure-Object LastWriteTime -Maximum).Maximum
$objects = @()
foreach ($cpp in Get-ChildItem $src -Recurse -Filter *.cpp) {
    $rel = $cpp.FullName.Substring($src.Length + 1)
    $o = Join-Path $obj (($rel -replace '[\\/]', '_') + '.o')
    $objects += $o
    $stale = (-not (Test-Path $o)) -or
             ((Get-Item $o).LastWriteTime -lt $cpp.LastWriteTime) -or
             ((Get-Item $o).LastWriteTime -lt $newestHeader)
    if ($stale) {
        Write-Host "compile  $rel"
        & g++ -std=c++17 -O1 -Wall -I $src @cflags -c $cpp.FullName -o $o
        if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $rel" }
    }
}

Write-Host 'link     m_cowork_ui.exe'
& g++ @objects -o $exe @libs -mwindows
if ($LASTEXITCODE -ne 0) { throw 'Linking failed.' }
Write-Host "built    $exe"

$toolSrc = Join-Path $root 'tools\matricX-cowork\matricX-cowork.cpp'
$toolExe = Join-Path $root 'tools\matricX-cowork\matricX-cowork.exe'
if (Test-Path $toolSrc) {
    $toolStale = (-not (Test-Path $toolExe)) -or ((Get-Item $toolSrc).LastWriteTime -gt (Get-Item $toolExe).LastWriteTime)
    if ($toolStale) {
        Write-Host 'compile  tools\matricX-cowork\matricX-cowork.cpp'
        & g++ -std=c++17 -O2 -Wall -Wextra -static $toolSrc -o $toolExe -luser32 -lgdi32 -lgdiplus -lwinmm
        if ($LASTEXITCODE -ne 0) { throw 'matricX-cowork tool compilation failed.' }
        Write-Host "built    $toolExe"
    }
}