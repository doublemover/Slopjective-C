$ErrorActionPreference = "Stop"

$nativeCmakeRoot = Join-Path $PSScriptRoot "objc3c_native_cmake"
$nativeCmakeModules = @(
  "logging.psm1",
  "toolchain.psm1",
  "paths.psm1",
  "fingerprint.psm1",
  "configure.psm1",
  "build.psm1"
)

foreach ($nativeCmakeModule in $nativeCmakeModules) {
  $nativeCmakeModulePath = Join-Path $nativeCmakeRoot $nativeCmakeModule
  if (!(Test-Path -LiteralPath $nativeCmakeModulePath -PathType Leaf)) {
    throw "native CMake support module missing: $nativeCmakeModulePath"
  }

  . $nativeCmakeModulePath
}

Export-ModuleMember -Function @(
  "Resolve-Objc3cNativeToolchain",
  "Get-Objc3cNativeCMakeBuildPaths",
  "Get-Objc3cNativeBuildFingerprint",
  "Test-Objc3cNativeBuildFingerprintMatch",
  "Invoke-Objc3cNativeCMakeConfigure",
  "Invoke-Objc3cNativeCMakeBuild"
)
