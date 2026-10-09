param(
  [string]$Compiler = "clang++"
)

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../../..")).Path
$binary = Join-Path $env:TEMP "objc3c_stdlib_service_storage_$PID.exe"
$sources = @(
  "native/objc3c/tests/stdlib_service_storage_test.cpp",
  "native/objc3c/src/runtime/stdlib/text_runtime.cpp",
  "native/objc3c/src/runtime/stdlib/text_service.cpp",
  "native/objc3c/src/runtime/stdlib/collections_runtime.cpp",
  "native/objc3c/src/runtime/stdlib/collections_service.cpp",
  "native/objc3c/src/runtime/stdlib/collections_runtime_state.cpp"
)

$arguments = @(
  "-std=c++20",
  "-Wall",
  "-Wextra",
  "-Werror",
  "-I",
  (Join-Path $repoRoot "native/objc3c/src")
) + @($sources | ForEach-Object { Join-Path $repoRoot $_ }) + @("-o", $binary)

try {
  Push-Location $repoRoot
  & $Compiler @arguments
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }

  & $binary
  exit $LASTEXITCODE
}
finally {
  Pop-Location
  if (Test-Path -LiteralPath $binary) {
    Remove-Item -LiteralPath $binary
  }
}
