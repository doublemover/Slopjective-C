Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Resolve-Objc3cNativeExecutionSmokeClangxx {
  param([string]$ConfiguredClangPath)

  if (-not [string]::IsNullOrWhiteSpace($ConfiguredClangPath)) {
    return $ConfiguredClangPath
  }

  foreach ($llvmRoot in @(Get-Objc3cNativeExecutionSmokeLlvmRootCandidates)) {
    $llvmClangxx = Join-Path $llvmRoot "bin\clang++.exe"
    if (Test-Path -LiteralPath $llvmClangxx -PathType Leaf) {
      return $llvmClangxx
    }
  }

  $pathClangxx = Get-Command "clang++" -ErrorAction SilentlyContinue
  if ($null -ne $pathClangxx -and -not [string]::IsNullOrWhiteSpace($pathClangxx.Source)) {
    return $pathClangxx.Source
  }

  return "clang++"
}

function Resolve-Objc3cNativeExecutionSmokeLlc {
  param([string]$ConfiguredLlcPath)

  if (-not [string]::IsNullOrWhiteSpace($ConfiguredLlcPath)) {
    return $ConfiguredLlcPath
  }

  foreach ($llvmRoot in @(Get-Objc3cNativeExecutionSmokeLlvmRootCandidates)) {
    $llvmLlc = Join-Path $llvmRoot "bin\llc.exe"
    if (Test-Path -LiteralPath $llvmLlc -PathType Leaf) {
      return $llvmLlc
    }
  }

  $pathLlc = Get-Command "llc" -ErrorAction SilentlyContinue
  if ($null -ne $pathLlc -and -not [string]::IsNullOrWhiteSpace($pathLlc.Source)) {
    return $pathLlc.Source
  }

  return "llc"
}

function Get-Objc3cNativeExecutionSmokeLlvmRootCandidates {
  $candidates = @()
  foreach ($envName in @("OBJC3C_LLVM_ROOT", "LLVM_ROOT")) {
    $configured = [System.Environment]::GetEnvironmentVariable($envName)
    if (-not [string]::IsNullOrWhiteSpace($configured)) {
      $candidates += $configured
    }
  }

  $version = if ($env:OBJC3C_CI_LLVM_VERSION) { $env:OBJC3C_CI_LLVM_VERSION } else { "22.1.6" }
  $userProfile = [System.Environment]::GetEnvironmentVariable("USERPROFILE")
  if (-not [string]::IsNullOrWhiteSpace($userProfile)) {
    $candidates += (Join-Path $userProfile ("Tools\LLVM\llvm-{0}-msvc" -f $version))
  }
  $candidates += "C:\Program Files\LLVM"

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

function Get-Objc3cNativeExecutionSmokeLinkDriverArgs {
  $args = @("-std=c++20")
  if ([System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT) {
    $args += @(
      "-fms-runtime-lib=dll",
      "-fuse-ld=lld",
      "-Xlinker",
      "/MANIFEST:EMBED",
      "-Xlinker",
      "/MANIFESTUAC:level='asInvoker' uiAccess='false'"
    )
  }
  return $args
}

function Resolve-Objc3cNativeExecutionSmokeConfig {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $positiveFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/positive"
  $negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/negative"
  $defaultRuntimeLibrary = Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib"
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
  $configuredRunId = $env:OBJC3C_NATIVE_EXECUTION_RUN_ID
  $runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
  $defaultNativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
  $nativeExe = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExe } else { $configuredNativeExe }
  $nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)
  $configuredClangPath = $env:OBJC3C_NATIVE_EXECUTION_CLANG_PATH
  $clangCommand = Resolve-Objc3cNativeExecutionSmokeClangxx -ConfiguredClangPath $configuredClangPath
  $linkDriverArgs = @(Get-Objc3cNativeExecutionSmokeLinkDriverArgs)
  $configuredLlcPath = $env:OBJC3C_NATIVE_EXECUTION_LLC_PATH
  $llcCommand = Resolve-Objc3cNativeExecutionSmokeLlc -ConfiguredLlcPath $configuredLlcPath
  $llcSourcePath = ""
  if (-not [string]::IsNullOrWhiteSpace($llcCommand)) {
    $llcSourcePath = $llcCommand
  }

  if (!(Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
    throw "execution smoke FAIL: runtime launch contract helper missing at $runtimeLaunchContractScript"
  }

  return [pscustomobject]@{
    repo_root = $repoRoot
    positive_fixture_dir = $positiveFixtureDir
    negative_fixture_dir = $negativeFixtureDir
    default_runtime_library = $defaultRuntimeLibrary
    build_script = $buildScript
    suite_root = $suiteRoot
    run_id = $runId
    run_dir = $runDir
    summary_path = $summaryPath
    runtime_launch_contract_script = $runtimeLaunchContractScript
    native_exe = $nativeExe
    native_exe_explicit = $nativeExeExplicit
    clang_command = $clangCommand
    link_driver_args = $linkDriverArgs
    llc_command = $llcCommand
    llc_source_path = $llcSourcePath
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeExecutionSmokeLinkDriverArgs",
  "Resolve-Objc3cNativeExecutionSmokeClangxx",
  "Resolve-Objc3cNativeExecutionSmokeConfig"
)
