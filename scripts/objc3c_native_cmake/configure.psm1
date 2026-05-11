$ErrorActionPreference = "Stop"

function Get-Objc3cNativeCMakeConfigureNeeded {
  param(
    [Parameter(Mandatory = $true)]
    [string]$BuildDir,
    [Parameter(Mandatory = $true)]
    [string]$FingerprintPath,
    [Parameter(Mandatory = $true)]
    [System.Collections.IDictionary]$Fingerprint,
    [Parameter(Mandatory = $true)]
    [bool]$ForceReconfigure
  )

  $cachePath = Join-Path $BuildDir "CMakeCache.txt"
  return $ForceReconfigure -or !(Test-Path -LiteralPath $cachePath -PathType Leaf) -or !(Test-Objc3cNativeBuildFingerprintMatch -ExpectedFingerprint $Fingerprint -FingerprintPath $FingerprintPath)
}

function Write-Objc3cNativeCMakeConfigureReason {
  param(
    [Parameter(Mandatory = $true)]
    [string]$BuildDir,
    [Parameter(Mandatory = $true)]
    [bool]$ForceReconfigure
  )

  $cachePath = Join-Path $BuildDir "CMakeCache.txt"
  if ($ForceReconfigure) {
    Write-Objc3cNativeBuildStep "cmake_configure=force-reconfigure"
  } elseif (Test-Path -LiteralPath $cachePath -PathType Leaf) {
    Write-Objc3cNativeBuildStep "cmake_configure=refresh-fingerprint"
  } else {
    Write-Objc3cNativeBuildStep "cmake_configure=cold"
  }
}

function Invoke-Objc3cNativeCMakeConfigure {
  param(
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$NinjaTool,
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDir,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDir,
    [Parameter(Mandatory = $true)][string]$FingerprintPath,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Fingerprint,
    [Parameter(Mandatory = $true)][bool]$ForceReconfigure
  )

  $needsConfigure = Get-Objc3cNativeCMakeConfigureNeeded `
    -BuildDir $BuildDir `
    -FingerprintPath $FingerprintPath `
    -Fingerprint $Fingerprint `
    -ForceReconfigure $ForceReconfigure

  if ($needsConfigure) {
    Write-Objc3cNativeCMakeConfigureReason -BuildDir $BuildDir -ForceReconfigure $ForceReconfigure
    & $CmakeTool `
      -S $SourceDir `
      -B $BuildDir `
      -G Ninja `
      "-DCMAKE_MAKE_PROGRAM=$NinjaTool" `
      "-DCMAKE_CXX_COMPILER=$Clangxx" `
      "-DCMAKE_BUILD_TYPE=Release" `
      "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON" `
      "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=ON" `
      "-DOBJC3C_ENABLE_WARNING_PARITY=ON" `
      "-DOBJC3C_LLVM_ROOT=$LlvmRoot" `
      "-DOBJC3C_LLVM_INCLUDE_DIR=$IncludeDir" `
      "-DOBJC3C_LIBCLANG_LIBRARY=$Libclang" `
      "-DOBJC3C_RUNTIME_OUTPUT_DIR=$RuntimeOutputDir" `
      "-DOBJC3C_LIBRARY_OUTPUT_DIR=$LibraryOutputDir"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $Fingerprint | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $FingerprintPath -Encoding utf8
  } else {
    Write-Objc3cNativeBuildStep "cmake_configure=reuse"
  }
}
