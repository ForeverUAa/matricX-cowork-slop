# Drives matricX-cowork.exe over its JSON-lines protocol (safe commands only).
$root = Split-Path -Parent $PSScriptRoot
$exe = Join-Path $root 'tools\matricX-cowork\matricX-cowork.exe'
if (-not (Test-Path $exe)) {
  & (Join-Path $PSScriptRoot 'build.ps1')
}
$psi = New-Object Diagnostics.ProcessStartInfo
$psi.FileName = $exe; $psi.Arguments = '--no-lock --no-cursor --glow off --idle-timeout 30'
$psi.RedirectStandardInput = $true; $psi.RedirectStandardOutput = $true; $psi.UseShellExecute = $false
$p = [Diagnostics.Process]::Start($psi)
$p.StandardOutput.ReadLine()
foreach ($j in '{"id":1,"cmd":"screen_info"}','{"id":2,"cmd":"mouse_move","x":600,"y":400}','{"id":3,"cmd":"screenshot"}','{"id":4,"cmd":"end_session"}') {
  $p.StandardInput.WriteLine($j); $p.StandardInput.Flush(); $p.StandardOutput.ReadLine()
}
$p.StandardOutput.ReadLine(); $p.WaitForExit(5000) | Out-Null
