$ErrorActionPreference = "Stop"

function Get-Objc3cNativeBuildFingerprint {
  param(
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$NinjaTool,
    [Parameter(Mandatory = $true)][string]$LlvmArTool,
    [Parameter(Mandatory = $true)][string]$LlvmRanlibTool,
    [Parameter(Mandatory = $true)][string]$LlvmLibTool,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDir,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDir,
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$SanitizerVariant,
    [Parameter(Mandatory = $true)][string]$SourceDateEpoch
  )

  function Get-ToolIdentity([string]$Path) {
    $resolved = if (Test-Path -LiteralPath $Path -PathType Leaf) { (Resolve-Path -LiteralPath $Path).Path } else { $Path }
    $digest = if (Test-Path -LiteralPath $Path -PathType Leaf) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() } else { "missing" }
    $version = ""
    if (Test-Path -LiteralPath $Path -PathType Leaf) { try { $version = ((& $Path --version 2>&1 | Select-Object -First 1) -join "").Trim() } catch { $version = "unavailable" } }
    return [ordered]@{ path = $resolved; sha256 = $digest; version = $version }
  }
  return [ordered]@{
    schema_version = 2
    generator = "Ninja"
    tool_identities = [ordered]@{ clangxx = (Get-ToolIdentity $Clangxx); cmake = (Get-ToolIdentity $CmakeTool); ninja = (Get-ToolIdentity $NinjaTool); llvm_ar = (Get-ToolIdentity $LlvmArTool); llvm_ranlib = (Get-ToolIdentity $LlvmRanlibTool); llvm_lib = (Get-ToolIdentity $LlvmLibTool); libclang = (Get-ToolIdentity $Libclang) }
    cmake = $CmakeTool
    ninja = $NinjaTool
    llvm_ar = $LlvmArTool
    llvm_ranlib = $LlvmRanlibTool
    llvm_lib = $LlvmLibTool
    clangxx = $Clangxx
    llvm_root = $LlvmRoot
    llvm_include_dir = $IncludeDir
    libclang = $Libclang
    build_dir = $BuildDir
    source_dir = $SourceDir
    runtime_output_dir = $RuntimeOutputDir
    library_output_dir = $LibraryOutputDir
    sanitizer_variant = $SanitizerVariant
    build_type = "RelWithDebInfo"
    direct_object_emission = $true
    warning_parity = $true
    reproducible_build = $true
    source_date_epoch = $SourceDateEpoch
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
    $actualValue = ConvertTo-Json -InputObject $actual[$key] -Compress -Depth 12
    $expectedValue = ConvertTo-Json -InputObject $ExpectedFingerprint[$key] -Compress -Depth 12
    if ($actualValue -cne $expectedValue) {
      return $false
    }
  }

  return $true
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeBuildFingerprint",
  "Test-Objc3cNativeBuildFingerprintMatch"
)
