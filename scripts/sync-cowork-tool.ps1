param([string]$Repo = 'C:\Users\moham\OneDrive\Documents\GitHub\matricX-cowork')
$dest = Join-Path (Split-Path $PSScriptRoot -Parent) 'tools\matricX-cowork'
New-Item -ItemType Directory -Force $dest | Out-Null
& (Join-Path $Repo 'matricX-cowork.ps1') -Build
Copy-Item (Join-Path $Repo 'matricX-cowork.exe') $dest -Force
Copy-Item (Join-Path $Repo 'akar-icons_cursor.png') $dest -Force
Write-Host "Synced matricX-cowork to $dest"
