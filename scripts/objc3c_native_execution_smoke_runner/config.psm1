Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Test-Objc3cNativeExecutionSmokeHostIsWindows {
  return [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::Windows
  )
}

function Test-Objc3cNativeExecutionSmokeHostIsDarwin {
  return [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::OSX
  )
}

function Test-Objc3cNativeExecutionSmokeHostIsLinux {
  return [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::Linux
  )
}

function Get-Objc3cNativeExecutionSmokeHostArchitecture {
  $architecture = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString().ToLowerInvariant()
  if ($architecture -in @("x64", "x86_64", "amd64")) {
    return "x64"
  }
  if ($architecture -in @("arm64", "aarch64")) {
    return "arm64"
  }
  return $architecture
}

function Get-Objc3cNativeExecutionSmokeHostPlatformId {
  $architecture = Get-Objc3cNativeExecutionSmokeHostArchitecture
  if (Test-Objc3cNativeExecutionSmokeHostIsWindows) {
    return "windows-$architecture"
  }
  if (Test-Objc3cNativeExecutionSmokeHostIsDarwin) {
    return "darwin-$architecture"
  }
  if (Test-Objc3cNativeExecutionSmokeHostIsLinux) {
    return "linux-$architecture"
  }
  return "unknown-$architecture"
}

function Get-Objc3cNativeExecutionSmokeTargetTriple {
  param([Parameter(Mandatory = $true)][string]$PlatformId)

  if ($PlatformId -eq "windows-x64") {
    return "x86_64-pc-windows-msvc"
  }
  if ($PlatformId -eq "linux-x64") {
    return "x86_64-unknown-linux-gnu"
  }
  if ($PlatformId -eq "darwin-arm64") {
    return "aarch64-apple-darwin"
  }
  if ($PlatformId -eq "darwin-x64") {
    return "x86_64-apple-darwin"
  }
  return $PlatformId
}

function Get-Objc3cNativeExecutionSmokeHostPromotionState {
  param([Parameter(Mandatory = $true)][string]$PlatformId)

  if ($PlatformId -eq "windows-x64") {
    return "supported-boundary"
  }
  if ($PlatformId -in @("linux-x64", "darwin-arm64")) {
    return "fail-closed-until-native-host-evidence"
  }
  return "unsupported-host"
}

function Get-Objc3cNativeExecutionSmokePlatformModel {
  $platformId = Get-Objc3cNativeExecutionSmokeHostPlatformId
  $isWindows = Test-Objc3cNativeExecutionSmokeHostIsWindows
  $isDarwin = Test-Objc3cNativeExecutionSmokeHostIsDarwin
  $objectFileExtension = if ($isWindows) { ".obj" } else { ".o" }
  $nativeExecutableName = if ($isWindows) { "objc3c-native.exe" } else { "objc3c-native" }
  $runtimeLibraryName = if ($isWindows) {
    "objc3_runtime.lib"
  } elseif ($isDarwin) {
    "libobjc3-runtime.dylib"
  } else {
    "libobjc3-runtime.so"
  }
  $runtimeLibraryKind = if ($isWindows) { "static-archive" } else { "shared-library" }
  $objectFormat = if ($isWindows) {
    "COFF"
  } elseif ($isDarwin) {
    "Mach-O"
  } else {
    "ELF"
  }
  $debugFormat = if ($isWindows) {
    "CodeView/PDB"
  } elseif ($isDarwin) {
    "DWARF/dSYM"
  } else {
    "DWARF"
  }
  $loaderPathPolicy = if ($isWindows) {
    "PATH-owned loader resolution for supported Windows package roots"
  } elseif ($isDarwin) {
    "@rpath, install_name, codesign, and package-root loader behavior must be proven before support"
  } else {
    "ELF rpath, RUNPATH, or package-root loader resolution must be proven before support"
  }
  $runtimeLoadEnvironmentVariable = if ($isWindows) {
    ""
  } elseif ($isDarwin) {
    "DYLD_LIBRARY_PATH"
  } else {
    "LD_LIBRARY_PATH"
  }
  $supportedPlatformIds = if ($platformId -eq "windows-x64") { @("windows-x64") } else { @() }

  return [pscustomobject]@{
    platform_id = $platformId
    supported_platform_ids = @($supportedPlatformIds)
    host_promotion_state = Get-Objc3cNativeExecutionSmokeHostPromotionState -PlatformId $platformId
    support_claim_published = $false
    target_triple = Get-Objc3cNativeExecutionSmokeTargetTriple -PlatformId $platformId
    native_executable_relative_path = "artifacts/bin/$nativeExecutableName"
    object_artifact = "module$objectFileExtension"
    object_file_extension = $objectFileExtension
    object_format = $objectFormat
    debug_format = $debugFormat
    runtime_library_relative_path = "artifacts/lib/$runtimeLibraryName"
    runtime_library_name = $runtimeLibraryName
    runtime_library_names = @($runtimeLibraryName)
    runtime_library_kind = $runtimeLibraryKind
    shared_runtime = (-not $isWindows)
    runtime_load_path_relative = if ($isWindows) { "" } else { "artifacts/lib" }
    runtime_load_environment_variable = $runtimeLoadEnvironmentVariable
    loader_path_policy = $loaderPathPolicy
  }
}

function Get-Objc3cNativeExecutionSmokeDefaultRuntimeLibraryRelativePath {
  return (Get-Objc3cNativeExecutionSmokePlatformModel).runtime_library_relative_path
}

function Set-Objc3cNativeExecutionSmokeRuntimeEnvironment {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][object]$PlatformModel
  )

  if (-not [bool]$PlatformModel.shared_runtime) {
    return [ordered]@{}
  }
  if ([string]::IsNullOrWhiteSpace($PlatformModel.runtime_load_path_relative) -or [string]::IsNullOrWhiteSpace($PlatformModel.runtime_load_environment_variable)) {
    return [ordered]@{}
  }

  $runtimeLoadPath = Join-Path $RepoRoot $PlatformModel.runtime_load_path_relative
  $environmentVariableName = [string]$PlatformModel.runtime_load_environment_variable
  $currentValue = [System.Environment]::GetEnvironmentVariable($environmentVariableName)
  $newValue = if ([string]::IsNullOrWhiteSpace($currentValue)) {
    $runtimeLoadPath
  } else {
    $runtimeLoadPath + [System.IO.Path]::PathSeparator + $currentValue
  }
  [System.Environment]::SetEnvironmentVariable($environmentVariableName, $newValue)

  $environment = [ordered]@{}
  $environment["${environmentVariableName}_PREPEND"] = $runtimeLoadPath
  $environment[$environmentVariableName] = $newValue
  return $environment
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
  $platformModel = Get-Objc3cNativeExecutionSmokePlatformModel
  $positiveFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/positive"
  $negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/negative"
  $defaultRuntimeLibrary = Join-Path $repoRoot $platformModel.runtime_library_relative_path
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
  $configuredRunId = $env:OBJC3C_NATIVE_EXECUTION_RUN_ID
  $runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
  $defaultNativeExe = Join-Path $repoRoot $platformModel.native_executable_relative_path
  $configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
  $nativeExe = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExe } else { $configuredNativeExe }
  $nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)
  $runtimeEnvironment = Set-Objc3cNativeExecutionSmokeRuntimeEnvironment `
    -RepoRoot $repoRoot `
    -PlatformModel $platformModel
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
    platform_model = $platformModel
    target_platform_id = $platformModel.platform_id
    supported_platform_ids = @($platformModel.supported_platform_ids)
    host_promotion_state = $platformModel.host_promotion_state
    support_claim_published = $false
    target_triple = $platformModel.target_triple
    object_artifact = $platformModel.object_artifact
    object_file_extension = $platformModel.object_file_extension
    object_format = $platformModel.object_format
    debug_format = $platformModel.debug_format
    runtime_library_kind = $platformModel.runtime_library_kind
    runtime_library_names = @($platformModel.runtime_library_names)
    runtime_library_relative_path = $platformModel.runtime_library_relative_path
    shared_runtime = [bool]$platformModel.shared_runtime
    runtime_load_path_relative = $platformModel.runtime_load_path_relative
    runtime_load_environment_variable = $platformModel.runtime_load_environment_variable
    runtime_environment = $runtimeEnvironment
    loader_path_policy = $platformModel.loader_path_policy
    positive_fixture_dir = $positiveFixtureDir
    negative_fixture_dir = $negativeFixtureDir
    default_runtime_library = $defaultRuntimeLibrary
    default_runtime_library_relative_path = $platformModel.runtime_library_relative_path
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
  "Get-Objc3cNativeExecutionSmokeDefaultRuntimeLibraryRelativePath",
  "Get-Objc3cNativeExecutionSmokeLinkDriverArgs",
  "Get-Objc3cNativeExecutionSmokePlatformModel",
  "Get-Objc3cNativeExecutionSmokeSanitizerRuntimeLinkArgs",
  "Resolve-Objc3cNativeExecutionSmokeSanitizerVariant",
  "Resolve-Objc3cNativeExecutionSmokeClangxx",
  "Resolve-Objc3cNativeExecutionSmokeConfig"
)
