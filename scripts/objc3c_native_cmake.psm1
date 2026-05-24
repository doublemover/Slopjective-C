$ErrorActionPreference = "Stop"

$nativeCmakeRoot = Join-Path $PSScriptRoot "objc3c_native_cmake"
$nativeCmakeModules = @(
  "logging.psm1",
  "paths.psm1",
  "toolchain.psm1",
  "fingerprint.psm1",
  "configure.psm1",
  "build.psm1"
)

foreach ($nativeCmakeModule in $nativeCmakeModules) {
  $nativeCmakeModulePath = Join-Path $nativeCmakeRoot $nativeCmakeModule
  if (!(Test-Path -LiteralPath $nativeCmakeModulePath -PathType Leaf)) {
    throw "native CMake support module missing: $nativeCmakeModulePath"
  }

  Import-Module $nativeCmakeModulePath -Force -DisableNameChecking -Global
}

Export-ModuleMember -Function @(
  "Test-Objc3cNativeHostIsWindows",
  "Test-Objc3cNativeHostIsDarwin",
  "Test-Objc3cNativeHostIsLinux",
  "Get-Objc3cNativeHostArchitecture",
  "Get-Objc3cNativeHostPlatformId",
  "Get-Objc3cNativeExecutableExtension",
  "Get-Objc3cNativeExecutableFileName",
  "Get-Objc3cNativeRuntimeLibraryFileName",
  "Get-Objc3cNativeRuntimeLibraryKind",
  "Get-Objc3cNativeObjectFormat",
  "Get-Objc3cNativeDebugFormat",
  "Get-Objc3cNativeTargetTriple",
  "Get-Objc3cNativePackageArtifactRelativePaths",
  "Get-Objc3cNativeToolExecutableName",
  "Join-Objc3cNativeLlvmToolPath",
  "Resolve-Objc3cNativeToolchain",
  "Get-Objc3cNativeCMakeBuildPaths",
  "Get-Objc3cNativeBuildFingerprint",
  "Test-Objc3cNativeBuildFingerprintMatch",
  "Invoke-Objc3cNativeCMakeConfigure",
  "Invoke-Objc3cNativeCMakeBuild"
)
