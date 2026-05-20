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

function Resolve-Objc3cNativeToolchain {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot
  )

  $llvmRoot = if ($env:LLVM_ROOT) { $env:LLVM_ROOT } else { "C:\Program Files\LLVM" }
  $clangxx = Join-Path $llvmRoot "bin\clang++.exe"
  $llvmArTool = Join-Path $llvmRoot "bin\llvm-ar.exe"
  $llvmRanlibTool = Join-Path $llvmRoot "bin\llvm-ranlib.exe"
  $llvmLibTool = Join-Path $llvmRoot "bin\llvm-lib.exe"

  if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
    $resolvedClangxx = Resolve-Objc3cNativeCommandPath -CommandName "clang++"
    if ($null -ne $resolvedClangxx) {
      $clangxx = $resolvedClangxx
      $clangBinDir = Split-Path -Parent $clangxx
      $llvmRoot = Split-Path -Parent $clangBinDir
      $llvmArTool = Join-Path $llvmRoot "bin\llvm-ar.exe"
      $llvmRanlibTool = Join-Path $llvmRoot "bin\llvm-ranlib.exe"
      $llvmLibTool = Join-Path $llvmRoot "bin\llvm-lib.exe"
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
    }
  }

  $cmakeTool = Resolve-Objc3cNativeCommandPath -CommandName "cmake"
  $ninjaTool = Resolve-Objc3cNativeCommandPath -CommandName "ninja"
  $libclangCandidates = @(
    (Join-Path $llvmRoot "lib\libclang.lib"),
    (Join-Path $llvmRoot "lib\clang.lib")
  )
  $libclang = Resolve-Objc3cNativeLibclangPath -Candidates $libclangCandidates
  $includeDir = Join-Path $llvmRoot "include"
  $nativeSourceRoot = Join-Path $RepoRoot "native/objc3c/src"

  Assert-Objc3cNativeToolchainPath -Path $clangxx -PathType Leaf -Message ("clang++ not found. set LLVM_ROOT or ensure clang++ is on PATH (attempted: " + $clangxx + ")")
  Assert-Objc3cNativeToolchainPath -Path $llvmArTool -PathType Leaf -Message ("llvm-ar not found. set LLVM_ROOT or ensure llvm-ar is on PATH (attempted: " + $llvmArTool + ")")
  Assert-Objc3cNativeToolchainPath -Path $llvmRanlibTool -PathType Leaf -Message ("llvm-ranlib not found. set LLVM_ROOT or ensure llvm-ranlib is on PATH (attempted: " + $llvmRanlibTool + ")")
  Assert-Objc3cNativeToolchainPath -Path $llvmLibTool -PathType Leaf -Message ("llvm-lib not found. set LLVM_ROOT or ensure llvm-lib is on PATH (attempted: " + $llvmLibTool + ")")
  if ($null -eq $libclang) {
    $attempted = [string]::Join(", ", $libclangCandidates)
    throw ("LLVM import library not found. set LLVM_ROOT to a full LLVM install (attempted: " + $attempted + ")")
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
  "Resolve-Objc3cNativeCommandPath",
  "Resolve-Objc3cNativeToolchain",
  "Resolve-Objc3cNativeLibclangPath",
  "Assert-Objc3cNativeToolchainPath"
)
