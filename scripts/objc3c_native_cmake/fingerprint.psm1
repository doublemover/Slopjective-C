$ErrorActionPreference = "Stop"

function Get-Objc3cNativeBuildFingerprint {
  param(
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$NinjaTool,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDir,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDir,
    [Parameter(Mandatory = $true)][string]$SourceDir
  )

  return [ordered]@{
    schema_version = 1
    generator = "Ninja"
    cmake = $CmakeTool
    ninja = $NinjaTool
    clangxx = $Clangxx
    llvm_root = $LlvmRoot
    llvm_include_dir = $IncludeDir
    libclang = $Libclang
    build_dir = $BuildDir
    source_dir = $SourceDir
    runtime_output_dir = $RuntimeOutputDir
    library_output_dir = $LibraryOutputDir
    build_type = "Release"
    direct_object_emission = $true
    warning_parity = $true
  }
}

function Test-Objc3cNativeBuildFingerprintMatch {
  param(
    [Parameter(Mandatory = $true)]
    [System.Collections.IDictionary]$ExpectedFingerprint,
    [Parameter(Mandatory = $true)]
    [string]$FingerprintPath
  )

  if (!(Test-Path -LiteralPath $FingerprintPath -PathType Leaf)) {
    return $false
  }

  try {
    $actual = Get-Content -LiteralPath $FingerprintPath -Raw | ConvertFrom-Json -AsHashtable
  } catch {
    return $false
  }

  foreach ($key in $ExpectedFingerprint.Keys) {
    if (!$actual.ContainsKey($key)) {
      return $false
    }
    if ([string]$actual[$key] -ne [string]$ExpectedFingerprint[$key]) {
      return $false
    }
  }

  return $true
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeBuildFingerprint",
  "Test-Objc3cNativeBuildFingerprintMatch"
)
