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
  param([string]$SanitizerVariant = "release")

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
  if ($SanitizerVariant -eq "address") {
    $args += @("-fsanitize=address", "-fno-omit-frame-pointer")
  } elseif ($SanitizerVariant -eq "undefined") {
    $args += @("-fsanitize=undefined", "-fno-sanitize-recover=undefined", "-fno-omit-frame-pointer")
  }
  return $args
}

function Get-Objc3cNativeExecutionSmokeSanitizerRuntimeLinkArgs {
  param(
    [string]$SanitizerVariant = "release",
    [string]$RuntimeDir = ""
  )

  if ($SanitizerVariant -eq "release") {
    return @()
  }
  if ([string]::IsNullOrWhiteSpace($RuntimeDir)) {
    throw "execution smoke FAIL: sanitizer runtime link directory missing for $SanitizerVariant"
  }

  if ([System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT) {
    return @("-Xlinker", "/LIBPATH:$RuntimeDir")
  }
  return @("-L", $RuntimeDir)
}

function Resolve-Objc3cNativeExecutionSmokeSanitizerVariant {
  $variant = $env:OBJC3C_NATIVE_EXECUTION_SANITIZER_VARIANT
  if ([string]::IsNullOrWhiteSpace($variant)) {
    return "release"
  }
  if ($variant -notin @("release", "address", "undefined")) {
    throw "execution smoke FAIL: OBJC3C_NATIVE_EXECUTION_SANITIZER_VARIANT must be release, address, or undefined"
  }
  return $variant
}

function Resolve-Objc3cNativeExecutionSmokeSanitizerRuntimeDir {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$SanitizerVariant
  )

  if ($SanitizerVariant -eq "release") {
    return ""
  }
  $leaf = if ($SanitizerVariant -eq "address") { "address" } else { "undefined" }
  $runtimeDir = Join-Path $RepoRoot (Join-Path "artifacts/runtime/sanitizer" $leaf)
  if (!(Test-Path -LiteralPath $runtimeDir -PathType Container)) {
    throw "execution smoke FAIL: sanitizer runtime directory missing for ${SanitizerVariant}: $runtimeDir"
  }
  return $runtimeDir
}

function Set-Objc3cNativeExecutionSmokeSanitizerEnvironment {
  param(
    [Parameter(Mandatory = $true)][string]$SanitizerVariant,
    [string]$RuntimeDir = ""
  )

  if ($SanitizerVariant -eq "release") {
    return [ordered]@{}
  }
  if ([string]::IsNullOrWhiteSpace($RuntimeDir)) {
    throw "execution smoke FAIL: sanitizer runtime directory missing from environment setup"
  }

  $env:PATH = $RuntimeDir + [System.IO.Path]::PathSeparator + $env:PATH
  if ($SanitizerVariant -eq "address") {
    if ([string]::IsNullOrWhiteSpace($env:ASAN_OPTIONS)) {
      $env:ASAN_OPTIONS = "detect_leaks=0:halt_on_error=1:symbolize=1"
    }
    return [ordered]@{
      PATH_PREPEND = $RuntimeDir
      ASAN_OPTIONS = $env:ASAN_OPTIONS
    }
  }

  if ([string]::IsNullOrWhiteSpace($env:UBSAN_OPTIONS)) {
    $env:UBSAN_OPTIONS = "halt_on_error=1:print_stacktrace=1"
  }
  return [ordered]@{
    PATH_PREPEND = $RuntimeDir
    UBSAN_OPTIONS = $env:UBSAN_OPTIONS
  }
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
  $sanitizerVariant = Resolve-Objc3cNativeExecutionSmokeSanitizerVariant
  $sanitizerRuntimeDir = Resolve-Objc3cNativeExecutionSmokeSanitizerRuntimeDir `
    -RepoRoot $repoRoot `
    -SanitizerVariant $sanitizerVariant
  $sanitizerEnvironment = Set-Objc3cNativeExecutionSmokeSanitizerEnvironment `
    -SanitizerVariant $sanitizerVariant `
    -RuntimeDir $sanitizerRuntimeDir
  $configuredClangPath = $env:OBJC3C_NATIVE_EXECUTION_CLANG_PATH
  $clangCommand = Resolve-Objc3cNativeExecutionSmokeClangxx -ConfiguredClangPath $configuredClangPath
  $linkDriverArgs = @(
    @(Get-Objc3cNativeExecutionSmokeLinkDriverArgs -SanitizerVariant $sanitizerVariant) +
    @(Get-Objc3cNativeExecutionSmokeSanitizerRuntimeLinkArgs `
      -SanitizerVariant $sanitizerVariant `
      -RuntimeDir $sanitizerRuntimeDir)
  )
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
    sanitizer_variant = $sanitizerVariant
    sanitizer_runtime_dir = $sanitizerRuntimeDir
    sanitizer_environment = $sanitizerEnvironment
    clang_command = $clangCommand
    link_driver_args = $linkDriverArgs
    llc_command = $llcCommand
    llc_source_path = $llcSourcePath
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeExecutionSmokeLinkDriverArgs",
  "Get-Objc3cNativeExecutionSmokeSanitizerRuntimeLinkArgs",
  "Resolve-Objc3cNativeExecutionSmokeSanitizerVariant",
  "Resolve-Objc3cNativeExecutionSmokeClangxx",
  "Resolve-Objc3cNativeExecutionSmokeConfig"
)
