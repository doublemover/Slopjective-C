Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Resolve-Objc3cNativeExecutionSmokeClangxx {
  param([string]$ConfiguredClangPath)

  if (-not [string]::IsNullOrWhiteSpace($ConfiguredClangPath)) {
    return $ConfiguredClangPath
  }

  $llvmRoot = $env:LLVM_ROOT
  if (-not [string]::IsNullOrWhiteSpace($llvmRoot)) {
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
  $llcCommand = $configuredLlcPath
  $llcSourcePath = ""
  if ([string]::IsNullOrWhiteSpace($llcCommand)) {
    $llcCandidate = Get-Command llc -ErrorAction SilentlyContinue
    if ($null -ne $llcCandidate -and -not [string]::IsNullOrWhiteSpace($llcCandidate.Source)) {
      $llcCommand = $llcCandidate.Source
    } else {
      $llcCommand = "llc"
    }
  }
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
