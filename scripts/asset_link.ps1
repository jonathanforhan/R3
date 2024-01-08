param(
    [string]$in_dir,
    [string]$out_dir
)

# Windows needs elevated permissions for symlinks
if (!(Test-Path($out_dir))) {
    Write-Host "linking assets"
    Start-Process powershell -WindowStyle Hidden -Verb RunAs -ArgumentList "python  $PSScriptRoot\asset_link.py $in_dir $out_dir"
}