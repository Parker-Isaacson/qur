param(
    [string]$Action
)

if ($Action -eq "clean") {
    Remove-Item "file"
} elseif (-not $Action) {
    g++ -std=c++17 qur.cpp utils/transpile.cpp -o qur
    ./qur --transpile file.qur --out file
} else {
    Write-Host "Unknown parameter: $Action"
    Write-Host "Usage: make.ps1 [clean]"
}