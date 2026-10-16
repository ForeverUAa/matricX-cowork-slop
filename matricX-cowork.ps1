<#
.SYNOPSIS
    Launcher script for MatricX Cowork (matricX-cowork.exe).

.DESCRIPTION
    Launches the compiled C++ mouse wander utility, passing through any
    custom CLI arguments (e.g. -Duration, -Speed, -Pattern, -Glow, etc.).
    If the executable is not built, it can automatically compile it.

.EXAMPLE
    .\matricX-cowork.ps1
    Runs matricX-cowork.exe with default settings.

.EXAMPLE
    .\matricX-cowork.ps1 -Duration 120 -Glow cyan -Speed fast
    Runs for 120 seconds with cyan border glow and fast movement.

.EXAMPLE
    .\matricX-cowork.ps1 -Duration infinite -Glow rainbow
    Runs until ESC is pressed with rainbow pulsing glow.
#>

[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$Duration,

    [ValidateSet('slow', 'normal', 'fast')]
    [string]$Speed,

    [ValidateSet('wander', 'work', 'edge')]
    [string]$Pattern,

    [ValidateSet('green', 'cyan', 'rainbow', 'off')]
    [string]$Glow,

    [switch]$NoCursor,
    [switch]$NoLock,
    [switch]$RestoreCursor,
    [switch]$Build,
    [switch]$Help
)

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ExePath   = Join-Path $ScriptDir "matricX-cowork.exe"
$CppPath   = Join-Path $ScriptDir "matricX-cowork.cpp"

if ($Help) {
    if (Test-Path $ExePath) {
        & $ExePath --help
    } else {
        Get-Help $MyInvocation.MyCommand.Path -Full
    }
    return
}

# Auto-build if requested or if executable does not exist
if ($Build -or (-not (Test-Path $ExePath))) {
    Write-Host "Building matricX-cowork.exe..." -ForegroundColor Cyan
    $gpp = Get-Command "g++" -ErrorAction SilentlyContinue
    if (-not $gpp) {
        Write-Error "g++ (MinGW-w64) not found in PATH. Please install or compile matricX-cowork.cpp manually."
        return
    }

    $buildCmd = "g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -static `"$CppPath`" -o `"$ExePath`" -luser32 -lgdi32 -lgdiplus"
    Invoke-Expression $buildCmd

    if (-not (Test-Path $ExePath)) {
        Write-Error "Build failed."
        return
    }
    Write-Host "Build successful!`n" -ForegroundColor Green
    if ($Build -and -not $PSBoundParameters.ContainsKey('Duration') -and -not $PSBoundParameters.ContainsKey('Glow')) {
        return
    }
}

# Forward parameters to matricX-cowork.exe
$argList = @()

if ($PSBoundParameters.ContainsKey('Duration')) {
    $argList += "--duration"
    $argList += $Duration
}

if ($PSBoundParameters.ContainsKey('Speed')) {
    $argList += "--speed"
    $argList += $Speed
}

if ($PSBoundParameters.ContainsKey('Pattern')) {
    $argList += "--pattern"
    $argList += $Pattern
}

if ($PSBoundParameters.ContainsKey('Glow')) {
    $argList += "--glow"
    $argList += $Glow
}

if ($NoCursor) {
    $argList += "--no-cursor"
}

if ($NoLock) {
    $argList += "--no-lock"
}

if ($RestoreCursor) {
    $argList += "--restore-cursor"
}

# Forward any additional unparsed arguments
if ($args) {
    $argList += $args
}

# Run the executable directly
& $ExePath @argList
