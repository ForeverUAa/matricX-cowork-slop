# Runs M-Cowork (builds first if the exe is missing).
$root = Split-Path -Parent $PSScriptRoot
$exe  = Join-Path $root 'build\m_cowork_ui.exe'
if (-not (Test-Path $exe)) { & (Join-Path $PSScriptRoot 'build.ps1') }
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
$env:QT_PLUGIN_PATH = 'C:\msys64\ucrt64\share\qt6\plugins'
Start-Process -FilePath $exe -WorkingDirectory $root