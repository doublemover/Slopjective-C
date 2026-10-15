param([string]$Compiler = "clang++")

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../../..")).Path
$binary = Join-Path $env:TEMP "objc3c_generic_collection_service_$PID.exe"
$sources = @(
  "native/objc3c/tests/generic_collection_service_test.cpp",
  "native/objc3c/src/runtime/stdlib/generic_collection_service.cpp",
  "native/objc3c/src/runtime/services/error_service.cpp"
)
$arguments = @("-std=c++20", "-Wall", "-Wextra", "-Werror", "-pthread",
  "-I", (Join-Path $repoRoot "native/objc3c/src")) +
  @($sources | ForEach-Object { Join-Path $repoRoot $_ }) + @("-o", $binary)

try {
  Push-Location $repoRoot
  & $Compiler @arguments
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  & $binary
  exit $LASTEXITCODE
}
finally {
  Pop-Location
  if (Test-Path -LiteralPath $binary) { Remove-Item -LiteralPath $binary }
}
