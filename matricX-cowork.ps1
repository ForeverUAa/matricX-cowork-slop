<#
.SYNOPSIS  Builds (if needed) and runs matricX-cowork, the computer-control tool for the M-Cowork client.
.EXAMPLE   .\matricX-cowork.ps1 -Describe
.EXAMPLE   .\matricX-cowork.ps1 -Build
#>
param(
  [ValidateSet('slow','normal','fast')][string]$Speed = 'normal',
  [ValidateSet('green','cyan','rainbow','off')][string]$Glow = 'green',
  [int]$IdleTimeout = 120,
  [switch]$NoCursor, [switch]$NoLock, [switch]$RestoreCursor, [switch]$Describe, [switch]$Build, [switch]$Help
)
$here = $PSScriptRoot
$exe  = Join-Path $here 'matricX-cowork.exe'
$src  = Join-Path $here 'matricX-cowork.cpp'
if ($Build -or -not (Test-Path $exe) -or ((Get-Item $src).LastWriteTime -gt (Get-Item $exe).LastWriteTime)) {
  $env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"
  Write-Host 'Building matricX-cowork...'
  g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -static $src -o $exe -luser32 -lgdi32 -lgdiplus -lwinmm
  if ($LASTEXITCODE -ne 0) { throw 'build failed' }
}
$a = @()
if ($Help) { $a += '--help' } elseif ($Describe) { $a += '--describe' } elseif ($RestoreCursor) { $a += '--restore-cursor' }
else {
  $a += '--speed',$Speed,'--glow',$Glow,'--idle-timeout',$IdleTimeout
  if ($NoCursor) { $a += '--no-cursor' }; if ($NoLock) { $a += '--no-lock' }
}
& $exe @a
