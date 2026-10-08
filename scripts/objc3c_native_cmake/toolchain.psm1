$ErrorActionPreference = "Stop"

function Resolve-Objc3cNativeCommandPath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$CommandName
  )

  $command = Get-Command $CommandName -ErrorAction SilentlyContinue
  if ($null -ne $command -and (Test-Path -LiteralPath $command.Source -PathType Leaf)) {
    return $command.Source
  }

  return $null
}

function Get-Objc3cNativeToolExecutableName {
  param([Parameter(Mandatory = $true)][string]$CommandName)

  if (Test-Objc3cNativeHostIsWindows) {
    return $CommandName + ".exe"
  }
  return $CommandName
}

function Join-Objc3cNativeLlvmToolPath {
  param(
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$CommandName
  )

  return Join-Path (Join-Path $LlvmRoot "bin") (Get-Objc3cNativeToolExecutableName -CommandName $CommandName)
}

function Get-Objc3cNativeLlvmRootCandidates {
  $candidates = @()
  foreach ($envName in @("OBJC3C_LLVM_ROOT", "LLVM_ROOT")) {
    $configured = [System.Environment]::GetEnvironmentVariable($envName)
    if (-not [string]::IsNullOrWhiteSpace($configured)) {
      $candidates += $configured
    }
  }

  if (Test-Objc3cNativeHostIsWindows) {
    $version = if ($env:OBJC3C_CI_LLVM_VERSION) { $env:OBJC3C_CI_LLVM_VERSION } else { "22.1.6" }
    $userProfile = [System.Environment]::GetEnvironmentVariable("USERPROFILE")
    if (-not [string]::IsNullOrWhiteSpace($userProfile)) {
      $candidates += (Join-Path $userProfile ("Tools\LLVM\llvm-{0}-msvc" -f $version))
    }
    $candidates += "C:\Program Files\LLVM"
  } elseif (Test-Objc3cNativeHostIsDarwin) {
    $candidates += "/opt/homebrew/opt/llvm"
    $candidates += "/usr/local/opt/llvm"
  } else {
    $candidates += "/usr/lib/llvm-22"
    $candidates += "/usr/lib/llvm-21"
    $candidates += "/usr/lib/llvm-20"
    $candidates += "/usr/lib/llvm-19"
    $candidates += "/usr/lib/llvm-18"
    $candidates += "/usr/lib/llvm"
    $candidates += "/usr/local/llvm"
  }

  $resolvedClangxx = Resolve-Objc3cNativeCommandPath -CommandName "clang++"
  if ($null -ne $resolvedClangxx) {
    $candidates += (Split-Path -Parent (Split-Path -Parent $resolvedClangxx))
  }

  $seen = @{}
  $ordered = @()
  foreach ($candidate in $candidates) {
    if ([string]::IsNullOrWhiteSpace($candidate)) {
      continue
    }
    $key = $candidate.ToLowerInvariant()
    if ($seen.ContainsKey($key)) {
      continue
    }
    $seen[$key] = $true
    $ordered += $candidate
  }
  return $ordered
}

function Resolve-Objc3cNativeLlvmRoot {
  foreach ($candidate in @(Get-Objc3cNativeLlvmRootCandidates)) {
    $clangxx = Join-Objc3cNativeLlvmToolPath -LlvmRoot $candidate -CommandName "clang++"
    $llc = Join-Objc3cNativeLlvmToolPath -LlvmRoot $candidate -CommandName "llc"
    $llvmConfig = Join-Objc3cNativeLlvmToolPath -LlvmRoot $candidate -CommandName "llvm-config"
    if (
      (Test-Path -LiteralPath $clangxx -PathType Leaf) -and
      (Test-Path -LiteralPath $llc -PathType Leaf) -and
      (Test-Path -LiteralPath $llvmConfig -PathType Leaf)
    ) {
      return $candidate
    }
  }

  foreach ($candidate in @(Get-Objc3cNativeLlvmRootCandidates)) {
    $clangxx = Join-Objc3cNativeLlvmToolPath -LlvmRoot $candidate -CommandName "clang++"
    if (Test-Path -LiteralPath $clangxx -PathType Leaf) {
      return $candidate
    }
  }

  if (Test-Objc3cNativeHostIsWindows) {
    return "C:\Program Files\LLVM"
  }
  return "/usr"
}

function Get-Objc3cNativeLibclangCandidates {
  param([Parameter(Mandatory = $true)][string]$LlvmRoot)

  $candidates = @()
  if (Test-Objc3cNativeHostIsWindows) {
    $candidates += (Join-Path (Join-Path $LlvmRoot "lib") "libclang.lib")
    $candidates += (Join-Path (Join-Path $LlvmRoot "lib") "clang.lib")
    return $candidates
  }

  $libDir = Join-Path $LlvmRoot "lib"
  if (Test-Objc3cNativeHostIsDarwin) {
    $candidates += (Join-Path $libDir "libclang.dylib")
  } else {
    $candidates += (Join-Path $libDir "libclang.so")
  }
  if (Test-Path -LiteralPath $libDir -PathType Container) {
    $candidates += @(
      Get-ChildItem -LiteralPath $libDir -File -Filter "libclang.*" -ErrorAction SilentlyContinue |
        Sort-Object -Property Name |
        ForEach-Object { $_.FullName }
    )
  }
  return @($candidates | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Select-Object -Unique)
}

function Resolve-Objc3cNativeToolchain {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot
  )

  $llvmRoot = Resolve-Objc3cNativeLlvmRoot
  $clangxx = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "clang++"
  $llvmArTool = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-ar"
  $llvmRanlibTool = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-ranlib"
  $llvmLibTool = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-lib"

  if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
    $resolvedClangxx = Resolve-Objc3cNativeCommandPath -CommandName "clang++"
    if ($null -ne $resolvedClangxx) {
      $clangxx = $resolvedClangxx
      $clangBinDir = Split-Path -Parent $clangxx
      $llvmRoot = Split-Path -Parent $clangBinDir
      $llvmArTool = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-ar"
      $llvmRanlibTool = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-ranlib"
      $llvmLibTool = Join-Objc3cNativeLlvmToolPath -LlvmRoot $llvmRoot -CommandName "llvm-lib"
    }
  }

  if (!(Test-Path -LiteralPath $llvmArTool -PathType Leaf)) {
    $resolvedLlvmAr = Resolve-Objc3cNativeCommandPath -CommandName "llvm-ar"
    if ($null -ne $resolvedLlvmAr) {
      $llvmArTool = $resolvedLlvmAr
    }
  }

  if (!(Test-Path -LiteralPath $llvmRanlibTool -PathType Leaf)) {
    $resolvedLlvmRanlib = Resolve-Objc3cNativeCommandPath -CommandName "llvm-ranlib"
    if ($null -ne $resolvedLlvmRanlib) {
      $llvmRanlibTool = $resolvedLlvmRanlib
    }
  }

  if (!(Test-Path -LiteralPath $llvmLibTool -PathType Leaf)) {
    $resolvedLlvmLib = Resolve-Objc3cNativeCommandPath -CommandName "llvm-lib"
    if ($null -ne $resolvedLlvmLib) {
      $llvmLibTool = $resolvedLlvmLib
    } elseif (-not (Test-Objc3cNativeHostIsWindows)) {
      $llvmLibTool = ""
    }
  }

  $cmakeTool = Resolve-Objc3cNativeCommandPath -CommandName "cmake"
  $ninjaTool = Resolve-Objc3cNativeCommandPath -CommandName "ninja"
  $libclangCandidates = @(Get-Objc3cNativeLibclangCandidates -LlvmRoot $llvmRoot)
  $libclang = Resolve-Objc3cNativeLibclangPath -Candidates $libclangCandidates
  $includeDir = Join-Path $llvmRoot "include"
  $nativeSourceRoot = Join-Path $RepoRoot "native/objc3c/src"

  Assert-Objc3cNativeToolchainPath -Path $clangxx -PathType Leaf -Message ("clang++ not found. set LLVM_ROOT or ensure clang++ is on PATH (attempted: " + $clangxx + ")")
  Assert-Objc3cNativeToolchainPath -Path $llvmArTool -PathType Leaf -Message ("llvm-ar not found. set LLVM_ROOT or ensure llvm-ar is on PATH (attempted: " + $llvmArTool + ")")
  Assert-Objc3cNativeToolchainPath -Path $llvmRanlibTool -PathType Leaf -Message ("llvm-ranlib not found. set LLVM_ROOT or ensure llvm-ranlib is on PATH (attempted: " + $llvmRanlibTool + ")")
  if (Test-Objc3cNativeHostIsWindows) {
    Assert-Objc3cNativeToolchainPath -Path $llvmLibTool -PathType Leaf -Message ("llvm-lib not found. set LLVM_ROOT or ensure llvm-lib is on PATH (attempted: " + $llvmLibTool + ")")
  }
  if ($null -eq $libclang) {
    $attempted = [string]::Join(", ", $libclangCandidates)
    throw ("libclang library not found. set LLVM_ROOT to a full LLVM install or install libclang development files (attempted: " + $attempted + ")")
  }
  Assert-Objc3cNativeToolchainPath -Path $includeDir -PathType Container -Message "LLVM include dir not found at $includeDir"
  Assert-Objc3cNativeToolchainPath -Path $nativeSourceRoot -PathType Container -Message "native source root not found at $nativeSourceRoot"
  if ($null -eq $cmakeTool) { throw "cmake not found. ensure cmake is on PATH" }
  if ($null -eq $ninjaTool) { throw "ninja not found. ensure ninja is on PATH" }

  return [pscustomobject]@{
    LlvmRoot = $llvmRoot
    Clangxx = $clangxx
    LlvmArTool = $llvmArTool
    LlvmRanlibTool = $llvmRanlibTool
    LlvmLibTool = $llvmLibTool
    CmakeTool = $cmakeTool
    NinjaTool = $ninjaTool
    Libclang = $libclang
    IncludeDir = $includeDir
    NativeSourceRoot = $nativeSourceRoot
  }
}

function Resolve-Objc3cNativeLibclangPath {
  param(
    [Parameter(Mandatory = $true)]
    [string[]]$Candidates
  )

  foreach ($candidate in $Candidates) {
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
      return $candidate
    }
  }

  return $null
}

function Assert-Objc3cNativeToolchainPath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path,
    [Parameter(Mandatory = $true)]
    [Microsoft.PowerShell.Commands.TestPathType]$PathType,
    [Parameter(Mandatory = $true)]
    [string]$Message
  )

  if (!(Test-Path -LiteralPath $Path -PathType $PathType)) {
    throw $Message
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeToolExecutableName",
  "Join-Objc3cNativeLlvmToolPath",
  "Get-Objc3cNativeLlvmRootCandidates",
  "Resolve-Objc3cNativeLlvmRoot",
  "Resolve-Objc3cNativeCommandPath",
  "Resolve-Objc3cNativeToolchain",
  "Get-Objc3cNativeLibclangCandidates",
  "Resolve-Objc3cNativeLibclangPath",
  "Assert-Objc3cNativeToolchainPath"
)
