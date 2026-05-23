param(
  [string]$Version = "",
  [string]$InstallParent = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Version)) {
  $Version = if ($env:OBJC3C_CI_LLVM_VERSION) { $env:OBJC3C_CI_LLVM_VERSION } else { "22.1.6" }
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
if ([string]::IsNullOrWhiteSpace($InstallParent)) {
  $InstallParent = if ($env:RUNNER_TEMP) {
    $env:RUNNER_TEMP
  } else {
    Join-Path $repoRoot "tmp/toolchains"
  }
}
$resolvedInstallParent = [System.IO.Path]::GetFullPath($InstallParent)
New-Item -ItemType Directory -Force -Path $resolvedInstallParent | Out-Null

function Assert-Objc3cPathUnderRoot {
  param(
    [Parameter(Mandatory = $true)][string]$Root,
    [Parameter(Mandatory = $true)][string]$Target
  )

  $resolvedRoot = [System.IO.Path]::GetFullPath($Root).TrimEnd('\', '/')
  $resolvedTarget = [System.IO.Path]::GetFullPath($Target)
  if (!$resolvedTarget.StartsWith($resolvedRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "refusing to operate outside LLVM install parent: $resolvedTarget"
  }
}

function Test-Objc3cRequiredLlvmTools {
  param([Parameter(Mandatory = $true)][string]$LlvmRoot)

  $bin = Join-Path $LlvmRoot "bin"
  foreach ($tool in @(
    "clang.exe",
    "clang++.exe",
    "llc.exe",
    "llvm-ar.exe",
    "llvm-ranlib.exe",
    "llvm-lib.exe",
    "llvm-readobj.exe",
    "llvm-config.exe"
  )) {
    if (!(Test-Path -LiteralPath (Join-Path $bin $tool) -PathType Leaf)) {
      return $false
    }
  }
  return (Test-Path -LiteralPath (Join-Path $LlvmRoot "lib/libclang.lib") -PathType Leaf)
}

$archiveStem = "clang+llvm-$Version-x86_64-pc-windows-msvc"
$installRoot = Join-Path $resolvedInstallParent "llvm-$Version-msvc"
$archive = Join-Path $resolvedInstallParent "$archiveStem.tar.xz"
$url = "https://github.com/llvm/llvm-project/releases/download/llvmorg-$Version/$archiveStem.tar.xz"

if (!(Test-Objc3cRequiredLlvmTools -LlvmRoot $installRoot)) {
  if (!(Test-Path -LiteralPath $archive -PathType Leaf)) {
    Invoke-WebRequest -Uri $url -OutFile $archive
  }

  $stagingRoot = Join-Path $resolvedInstallParent "llvm-$Version-msvc-staging"
  Assert-Objc3cPathUnderRoot -Root $resolvedInstallParent -Target $stagingRoot
  Assert-Objc3cPathUnderRoot -Root $resolvedInstallParent -Target $installRoot
  if (Test-Path -LiteralPath $stagingRoot) {
    Remove-Item -LiteralPath $stagingRoot -Recurse -Force
  }
  New-Item -ItemType Directory -Force -Path $stagingRoot | Out-Null

  & tar -xf $archive -C $stagingRoot
  $extractedRoot = Join-Path $stagingRoot $archiveStem
  if (!(Test-Objc3cRequiredLlvmTools -LlvmRoot $extractedRoot)) {
    throw "downloaded LLVM archive does not contain the required Objective-C 3.0 toolchain surface"
  }

  if (Test-Path -LiteralPath $installRoot) {
    Remove-Item -LiteralPath $installRoot -Recurse -Force
  }
  Move-Item -LiteralPath $extractedRoot -Destination $installRoot
  Remove-Item -LiteralPath $stagingRoot -Recurse -Force
}

if (!(Test-Objc3cRequiredLlvmTools -LlvmRoot $installRoot)) {
  throw "LLVM install is incomplete after installation: $installRoot"
}

$binRoot = Join-Path $installRoot "bin"
$clangxx = Join-Path $binRoot "clang++.exe"
$llc = Join-Path $binRoot "llc.exe"
$llvmReadobj = Join-Path $binRoot "llvm-readobj.exe"

$env:LLVM_ROOT = $installRoot
$env:Path = "$binRoot;$env:Path"

if (-not [string]::IsNullOrWhiteSpace($env:GITHUB_PATH)) {
  Add-Content -Path $env:GITHUB_PATH -Value $binRoot
}
if (-not [string]::IsNullOrWhiteSpace($env:GITHUB_ENV)) {
  Add-Content -Path $env:GITHUB_ENV -Value "LLVM_ROOT=$installRoot"
  Add-Content -Path $env:GITHUB_ENV -Value "OBJC3C_NATIVE_EXECUTION_CLANG_PATH=$clangxx"
  Add-Content -Path $env:GITHUB_ENV -Value "OBJC3C_NATIVE_EXECUTION_LLC_PATH=$llc"
  Add-Content -Path $env:GITHUB_ENV -Value "OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH=$llvmReadobj"
}

"LLVM_ROOT=$installRoot"
"& `"$clangxx`" --version"
& $clangxx --version | Select-Object -First 1
"& `"$llc`" --version"
& $llc --version | Select-Object -First 1
"& `"$binRoot\llvm-config.exe`" --version"
& (Join-Path $binRoot "llvm-config.exe") --version
