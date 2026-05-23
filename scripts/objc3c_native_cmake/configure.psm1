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

function Convert-Objc3cNativeCMakeCachePathForComparison {
  param(
    [Parameter(Mandatory = $true)]
    [AllowEmptyString()]
    [string]$Path
  )

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }

  try {
    return ([System.IO.Path]::GetFullPath($Path).TrimEnd('\', '/') -replace '\\', '/').ToLowerInvariant()
  } catch {
    return (($Path.TrimEnd('\', '/') -replace '\\', '/').ToLowerInvariant())
  }
}

function Get-Objc3cNativeCMakeCacheValue {
  param(
    [Parameter(Mandatory = $true)][string]$CachePath,
    [Parameter(Mandatory = $true)][string]$Key
  )

  if (!(Test-Path -LiteralPath $CachePath -PathType Leaf)) {
    return $null
  }

  foreach ($line in (Get-Content -LiteralPath $CachePath)) {
    if ($line.StartsWith($Key + ":", [System.StringComparison]::Ordinal)) {
      $separator = $line.IndexOf("=")
      if ($separator -ge 0) {
        return $line.Substring($separator + 1)
      }
    }
  }

  return $null
}

function Test-Objc3cNativeCMakeCacheToolchainMatch {
  param(
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang
  )

  $cachePath = Join-Path $BuildDir "CMakeCache.txt"
  if (!(Test-Path -LiteralPath $cachePath -PathType Leaf)) {
    return $true
  }

  $expected = @{
    CMAKE_CXX_COMPILER = $Clangxx
    OBJC3C_LLVM_ROOT = $LlvmRoot
    OBJC3C_LLVM_INCLUDE_DIR = $IncludeDir
    OBJC3C_LIBCLANG_LIBRARY = $Libclang
  }

  foreach ($key in $expected.Keys) {
    $actualValue = Get-Objc3cNativeCMakeCacheValue -CachePath $cachePath -Key $key
    if ((Convert-Objc3cNativeCMakeCachePathForComparisonSafe -Path $actualValue) -ne (Convert-Objc3cNativeCMakeCachePathForComparison -Path $expected[$key])) {
      return $false
    }
  }

  return $true
}

function Convert-Objc3cNativeCMakeCachePathForComparisonSafe {
  param(
    [AllowNull()]
    [string]$Path
  )

  if ($null -eq $Path) {
    return ""
  }

  return Convert-Objc3cNativeCMakeCachePathForComparison -Path $Path
}

function Reset-Objc3cNativeCMakeCacheIfToolchainDrifted {
  param(
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$FingerprintPath,
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang
  )

  if (Test-Objc3cNativeCMakeCacheToolchainMatch `
      -BuildDir $BuildDir `
      -Clangxx $Clangxx `
      -LlvmRoot $LlvmRoot `
      -IncludeDir $IncludeDir `
      -Libclang $Libclang) {
    return
  }

  Write-Objc3cNativeBuildStep "cmake_configure=toolchain-cache-mismatch"
  Remove-Item -LiteralPath (Join-Path $BuildDir "CMakeCache.txt") -Force -ErrorAction SilentlyContinue
  Remove-Item -LiteralPath (Join-Path $BuildDir "CMakeFiles") -Recurse -Force -ErrorAction SilentlyContinue
  Remove-Item -LiteralPath $FingerprintPath -Force -ErrorAction SilentlyContinue
}

function Invoke-Objc3cNativeCMakeConfigure {
  param(
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$NinjaTool,
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$LlvmArTool,
    [Parameter(Mandatory = $true)][string]$LlvmRanlibTool,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDir,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDir,
    [Parameter(Mandatory = $true)][string]$FingerprintPath,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Fingerprint,
    [Parameter(Mandatory = $true)][bool]$ForceReconfigure
  )

  Reset-Objc3cNativeCMakeCacheIfToolchainDrifted `
    -BuildDir $BuildDir `
    -FingerprintPath $FingerprintPath `
    -Clangxx $Clangxx `
    -LlvmRoot $LlvmRoot `
    -IncludeDir $IncludeDir `
    -Libclang $Libclang

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
      "-DCMAKE_AR=$LlvmArTool" `
      "-DCMAKE_RANLIB=$LlvmRanlibTool" `
      "-DCMAKE_BUILD_TYPE=Release" `
      "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON" `
      "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=ON" `
      "-DOBJC3C_ENABLE_WARNING_PARITY=ON" `
      "-DOBJC3C_ENABLE_REPRODUCIBLE_BUILD=ON" `
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

Export-ModuleMember -Function @(
  "Get-Objc3cNativeCMakeConfigureNeeded",
  "Write-Objc3cNativeCMakeConfigureReason",
  "Invoke-Objc3cNativeCMakeConfigure"
)
