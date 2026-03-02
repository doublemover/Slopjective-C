$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$defaultOutDir = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native"

function Show-UsageAndExit {
  Write-Error "usage: objc3c_native_compile.ps1 <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--use-cache]"
  exit 2
}

function Parse-WrapperArguments {
  param([string[]]$RawArgs)

  if ($RawArgs.Count -lt 1) {
    Show-UsageAndExit
  }

  $useCache = $false
  $compileArgs = New-Object System.Collections.Generic.List[string]
  $outDir = $null
  $wrapperFlagCounts = @{
    "--use-cache" = 0
    "--out-dir" = 0
  }

  for ($i = 0; $i -lt $RawArgs.Count; $i++) {
    $token = $RawArgs[$i]
    if ($token -eq "--use-cache") {
      $wrapperFlagCounts["--use-cache"] = [int]$wrapperFlagCounts["--use-cache"] + 1
      if ([int]$wrapperFlagCounts["--use-cache"] -gt 1) {
        Write-Error "--use-cache can be provided at most once"
        exit 2
      }
      $useCache = $true
      continue
    }
    if ($token.StartsWith("--use-cache=", [System.StringComparison]::OrdinalIgnoreCase)) {
      $wrapperFlagCounts["--use-cache"] = [int]$wrapperFlagCounts["--use-cache"] + 1
      if ([int]$wrapperFlagCounts["--use-cache"] -gt 1) {
        Write-Error "--use-cache can be provided at most once"
        exit 2
      }
      $rawBoolean = $token.Substring("--use-cache=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $rawBoolean) {
        $useCache = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $rawBoolean) {
        $useCache = $false
        continue
      }
      Write-Error "invalid --use-cache value '$rawBoolean' (expected true/false style token)"
      exit 2
    }

    if ($token -eq "--out-dir") {
      $wrapperFlagCounts["--out-dir"] = [int]$wrapperFlagCounts["--out-dir"] + 1
      if ([int]$wrapperFlagCounts["--out-dir"] -gt 1) {
        Write-Error "--out-dir can be provided at most once"
        exit 2
      }
      if (($i + 1) -ge $RawArgs.Count) {
        Show-UsageAndExit
      }
      $i++
      $value = $RawArgs[$i]
      $compileArgs.Add("--out-dir")
      $compileArgs.Add($value)
      $outDir = $value
      continue
    }

    if ($token.StartsWith("--out-dir=", [System.StringComparison]::Ordinal)) {
      $wrapperFlagCounts["--out-dir"] = [int]$wrapperFlagCounts["--out-dir"] + 1
      if ([int]$wrapperFlagCounts["--out-dir"] -gt 1) {
        Write-Error "--out-dir can be provided at most once"
        exit 2
      }
      $value = $token.Substring("--out-dir=".Length)
      if ([string]::IsNullOrWhiteSpace($value)) {
        Show-UsageAndExit
      }
      $compileArgs.Add("--out-dir")
      $compileArgs.Add($value)
      $outDir = $value
      continue
    }

    $compileArgs.Add($token)
  }

  if ($compileArgs.Count -lt 1) {
    Show-UsageAndExit
  }

  if ([string]::IsNullOrWhiteSpace($outDir)) {
    $outDir = $defaultOutDir
    $compileArgs.Add("--out-dir")
    $compileArgs.Add($outDir)
  }

  return [pscustomobject]@{
    use_cache = $useCache
    compile_args = $compileArgs.ToArray()
    out_dir = $outDir
  }
}

function Get-ArgsWithoutOutDir {
  param([string[]]$CompileArgs)

  $result = New-Object System.Collections.Generic.List[string]
  for ($i = 0; $i -lt $CompileArgs.Count; $i++) {
    $token = $CompileArgs[$i]
    if ($token -eq "--out-dir") {
      if (($i + 1) -ge $CompileArgs.Count) {
        break
      }
      $i++
      continue
    }
    $result.Add($token)
  }
  return $result.ToArray()
}

function Get-Sha256HexFromBytes {
  param([byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Get-FileSha256Hex {
  param([string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Get-OptionalFileHash {
  param([string]$Path)

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }
  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    return ""
  }
  return Get-FileSha256Hex -Path $Path
}

function Resolve-RepoBoundPath {
  param(
    [string]$RepoRoot,
    [string]$RelativeOrAbsolutePath,
    [string]$Label
  )

  if ([string]::IsNullOrWhiteSpace($RelativeOrAbsolutePath)) {
    Write-Error "$Label path is empty"
    exit 2
  }

  $candidatePath = $RelativeOrAbsolutePath
  if (-not [System.IO.Path]::IsPathRooted($candidatePath)) {
    $normalizedRelative = $candidatePath.Replace('\', '/')
    foreach ($segment in $normalizedRelative.Split('/')) {
      if ($segment -eq "..") {
        Write-Error "$Label path must not contain '..' relative segments: $RelativeOrAbsolutePath"
        exit 2
      }
    }
    $candidatePath = Join-Path $RepoRoot $candidatePath
  }

  $resolvedRoot = [System.IO.Path]::GetFullPath($RepoRoot).TrimEnd('\', '/')
  $resolvedCandidate = [System.IO.Path]::GetFullPath($candidatePath)
  $rootPrefix = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  if (($resolvedCandidate -ne $resolvedRoot) -and
      (-not $resolvedCandidate.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase))) {
    Write-Error "$Label path escapes repository root: $RelativeOrAbsolutePath"
    exit 2
  }

  return $resolvedCandidate
}

function Get-CacheKey {
  param(
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$CompilerSourcePath,
    [string]$WrapperScriptPath
  )

  if ([string]::IsNullOrWhiteSpace($InputPath)) {
    return $null
  }
  if (!(Test-Path -LiteralPath $InputPath -PathType Leaf)) {
    return $null
  }

  $inputHash = Get-FileSha256Hex -Path $InputPath
  $compilerSourceHash = Get-OptionalFileHash -Path $CompilerSourcePath
  $wrapperScriptHash = Get-OptionalFileHash -Path $WrapperScriptPath
  $payload = [ordered]@{
    version = 2
    input_sha256 = $inputHash
    compiler_source_sha256 = $compilerSourceHash
    wrapper_script_sha256 = $wrapperScriptHash
    args = $ArgsWithoutOutDir
  }
  $payloadJson = $payload | ConvertTo-Json -Compress -Depth 6
  $payloadBytes = [System.Text.Encoding]::UTF8.GetBytes($payloadJson)
  return Get-Sha256HexFromBytes -Bytes $payloadBytes
}

function Copy-DirectoryContents {
  param(
    [string]$SourceRoot,
    [string]$DestinationRoot
  )

  if (!(Test-Path -LiteralPath $SourceRoot -PathType Container)) {
    return
  }

  New-Item -ItemType Directory -Force -Path $DestinationRoot | Out-Null
  $resolvedSourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
  $files = Get-ChildItem -LiteralPath $SourceRoot -Recurse -File | Sort-Object -Property FullName

  foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($resolvedSourceRoot.Length).TrimStart('\', '/')
    $destination = Join-Path $DestinationRoot $relativePath
    $parent = Split-Path -Parent $destination
    if (![string]::IsNullOrWhiteSpace($parent)) {
      New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }
    Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
  }
}

function Invoke-BuildNativeCompiler {
  param([string]$RepoRoot)

  $buildScript = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"
  $buildOutput = @(& $buildScript)
  $buildOutputLines = New-Object System.Collections.Generic.List[string]
  $frontendScaffoldRelativePath = $null
  $frontendInvocationLockRelativePath = $null
  $frontendCoreFeatureExpansionRelativePath = $null
  $frontendEdgeCompatRelativePath = $null
  $frontendEdgeRobustnessRelativePath = $null
  foreach ($line in $buildOutput) {
    $lineText = [string]$line
    $buildOutputLines.Add($lineText)
    if ($lineText.StartsWith("frontend_scaffold=")) {
      $frontendScaffoldRelativePath = $lineText.Substring("frontend_scaffold=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_invocation_lock=")) {
      $frontendInvocationLockRelativePath = $lineText.Substring("frontend_invocation_lock=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_core_feature_expansion=")) {
      $frontendCoreFeatureExpansionRelativePath = $lineText.Substring("frontend_core_feature_expansion=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_edge_compat=")) {
      $frontendEdgeCompatRelativePath = $lineText.Substring("frontend_edge_compat=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_edge_robustness=")) {
      $frontendEdgeRobustnessRelativePath = $lineText.Substring("frontend_edge_robustness=".Length).Trim()
    }
  }
  return [pscustomobject]@{
    exit_code = [int]$LASTEXITCODE
    build_output_lines = $buildOutputLines.ToArray()
    frontend_scaffold_relative_path = $frontendScaffoldRelativePath
    frontend_invocation_lock_relative_path = $frontendInvocationLockRelativePath
    frontend_core_feature_expansion_relative_path = $frontendCoreFeatureExpansionRelativePath
    frontend_edge_compat_relative_path = $frontendEdgeCompatRelativePath
    frontend_edge_robustness_relative_path = $frontendEdgeRobustnessRelativePath
  }
}

function Invoke-NativeCompiler {
  param(
    [string]$ExePath,
    [string[]]$Arguments
  )

  $process = Start-Process -FilePath $ExePath -ArgumentList $Arguments -NoNewWindow -Wait -PassThru
  return [int]$process.ExitCode
}

function Resolve-FrontendScaffoldPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_scaffold_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend modular scaffold"
}

function Resolve-FrontendInvocationLockPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_invocation_lock_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend invocation lock"
}

function Resolve-FrontendCoreFeatureExpansionPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_core_feature_expansion_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend core feature expansion"
}

function Resolve-FrontendEdgeCompatibilityPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_edge_compat_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend edge compatibility"
}

function Resolve-FrontendEdgeRobustnessPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_edge_robustness_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend edge robustness"
}

function Assert-FrontendModuleScaffold {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend modular scaffold artifact missing at $scaffoldPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $scaffoldPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend modular scaffold artifact is not valid JSON at $scaffoldPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend modular scaffold contract id mismatch in $scaffoldPath"
    exit 2
  }

  $modules = @($payload.modules)
  $requiredModules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
  $presentModules = @{}
  foreach ($module in $modules) {
    $moduleName = [string]$module.name
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      Write-Error "frontend modular scaffold has module with missing name in $scaffoldPath"
      exit 2
    }
    $moduleSources = @($module.sources)
    if ($moduleSources.Count -eq 0) {
      Write-Error "frontend modular scaffold module '$moduleName' has no sources in $scaffoldPath"
      exit 2
    }
    $presentModules[$moduleName] = $true
  }
  foreach ($moduleName in $requiredModules) {
    if (-not $presentModules.ContainsKey($moduleName)) {
      Write-Error "frontend modular scaffold missing required module '$moduleName' in $scaffoldPath"
      exit 2
    }
  }

  $sharedSources = @($payload.shared_sources)
  if ($sharedSources.Count -eq 0) {
    Write-Error "frontend modular scaffold shared_sources must be non-empty in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.module_count -ne $modules.Count) {
    Write-Error "frontend modular scaffold module_count mismatch in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.shared_source_count -ne $sharedSources.Count) {
    Write-Error "frontend modular scaffold shared_source_count mismatch in $scaffoldPath"
    exit 2
  }
}

function Assert-FrontendInvocationLock {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $lockPath -PathType Leaf)) {
    Write-Error "frontend invocation lock artifact missing at $lockPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend invocation lock artifact is not valid JSON at $lockPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend invocation lock contract id mismatch in $lockPath"
    exit 2
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
  if ([string]$payload.scaffold_contract_id -ne $expectedScaffoldContractId) {
    Write-Error "frontend invocation lock scaffold contract id mismatch in $lockPath"
    exit 2
  }

  $scaffold = $payload.scaffold
  if ($null -eq $scaffold) {
    Write-Error "frontend invocation lock scaffold metadata missing in $lockPath"
    exit 2
  }

  $scaffoldRelativePath = [string]$scaffold.path
  $scaffoldExpectedHash = [string]$scaffold.sha256
  if ([string]::IsNullOrWhiteSpace($scaffoldRelativePath) -or [string]::IsNullOrWhiteSpace($scaffoldExpectedHash)) {
    Write-Error "frontend invocation lock scaffold metadata invalid in $lockPath"
    exit 2
  }

  $scaffoldPath = $scaffoldRelativePath
  if (-not [System.IO.Path]::IsPathRooted($scaffoldPath)) {
    $scaffoldPath = Join-Path $RepoRoot $scaffoldPath
  }
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend invocation lock scaffold path missing at $scaffoldPath"
    exit 2
  }
  $scaffoldActualHash = Get-FileSha256Hex -Path $scaffoldPath
  if ($scaffoldActualHash -ne $scaffoldExpectedHash.ToLowerInvariant()) {
    Write-Error "frontend invocation lock scaffold sha256 mismatch in $lockPath"
    exit 2
  }

  $binaries = @($payload.binaries)
  if ($binaries.Count -lt 2) {
    Write-Error "frontend invocation lock binaries list must include native and c-api runner entries in $lockPath"
    exit 2
  }

  $expectedBinaries = [ordered]@{
    "objc3c-native" = "artifacts/bin/objc3c-native.exe"
    "objc3c-frontend-c-api-runner" = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  }
  $binaryIndex = @{}
  foreach ($binary in $binaries) {
    $binaryName = [string]$binary.name
    $binaryPath = [string]$binary.path
    $binaryHash = [string]$binary.sha256
    if ([string]::IsNullOrWhiteSpace($binaryName) -or
        [string]::IsNullOrWhiteSpace($binaryPath) -or
        [string]::IsNullOrWhiteSpace($binaryHash)) {
      Write-Error "frontend invocation lock binary entry is invalid in $lockPath"
      exit 2
    }
    if ($binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock contains duplicate binary entry '$binaryName' in $lockPath"
      exit 2
    }
    $binaryIndex[$binaryName] = $binary
  }

  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock missing binary '$binaryName' in $lockPath"
      exit 2
    }
    $binary = $binaryIndex[$binaryName]
    $expectedRelativePath = [string]$expectedBinaries[$binaryName]
    $manifestRelativePath = ([string]$binary.path).Replace('\', '/')
    if ($manifestRelativePath -ne $expectedRelativePath) {
      Write-Error "frontend invocation lock binary path mismatch for '$binaryName' in $lockPath"
      exit 2
    }
    $binaryPath = Join-Path $RepoRoot $expectedRelativePath
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      Write-Error "frontend invocation lock binary path missing at $binaryPath"
      exit 2
    }
    $binaryActualHash = Get-FileSha256Hex -Path $binaryPath
    if ($binaryActualHash -ne ([string]$binary.sha256).ToLowerInvariant()) {
      Write-Error "frontend invocation lock binary sha256 mismatch for '$binaryName' in $lockPath"
      exit 2
    }
  }
}

function Assert-FrontendCoreFeatureExpansion {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs
  )

  $featurePath = Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $featurePath -PathType Leaf)) {
    Write-Error "frontend core feature expansion artifact missing at $featurePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $featurePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend core feature expansion artifact is not valid JSON at $featurePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend core feature expansion contract id mismatch in $featurePath"
    exit 2
  }

  $expectedDependencyContracts = @(
    "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1",
    "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
  )
  $presentDependencyContracts = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (![string]::IsNullOrWhiteSpace($contractIdText)) {
      $presentDependencyContracts[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencyContracts) {
    if (-not $presentDependencyContracts.ContainsKey($requiredContractId)) {
      Write-Error "frontend core feature expansion missing dependency contract '$requiredContractId' in $featurePath"
      exit 2
    }
  }

  $requiredModules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
  $presentModules = @{}
  foreach ($moduleName in @($payload.module_names)) {
    $moduleText = [string]$moduleName
    if (![string]::IsNullOrWhiteSpace($moduleText)) {
      $presentModules[$moduleText] = $true
    }
  }
  foreach ($requiredModule in $requiredModules) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Write-Error "frontend core feature expansion missing required module '$requiredModule' in $featurePath"
      exit 2
    }
  }

  $invocation = $payload.invocation
  if ($null -eq $invocation) {
    Write-Error "frontend core feature expansion invocation metadata missing in $featurePath"
    exit 2
  }
  if ([string]$invocation.default_out_dir -ne "tmp/artifacts/compilation/objc3c-native") {
    Write-Error "frontend core feature expansion default_out_dir mismatch in $featurePath"
    exit 2
  }
  if ([string]$invocation.cache_root -ne "tmp/artifacts/objc3c-native/cache") {
    Write-Error "frontend core feature expansion cache_root mismatch in $featurePath"
    exit 2
  }
  if (-not [bool]$invocation.supports_cache) {
    Write-Error "frontend core feature expansion supports_cache must be true in $featurePath"
    exit 2
  }

  $backendRouting = $payload.backend_routing
  if ($null -eq $backendRouting) {
    Write-Error "frontend core feature expansion backend_routing metadata missing in $featurePath"
    exit 2
  }
  if (-not [bool]$backendRouting.supports_capability_routing) {
    Write-Error "frontend core feature expansion supports_capability_routing must be true in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend core feature expansion capability_summary_flag mismatch in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend core feature expansion route_flag mismatch in $featurePath"
    exit 2
  }

  $allowedBackends = @{}
  foreach ($backend in @($backendRouting.allowed_ir_object_backends)) {
    $backendText = ([string]$backend).Trim()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends[$backendText.ToLowerInvariant()] = $backendText
    }
  }
  foreach ($requiredBackend in @("clang", "llvm-direct")) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Write-Error "frontend core feature expansion missing backend '$requiredBackend' in $featurePath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $requestedBackend = $null
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $requestedBackend = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $requestedBackend = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $usesCapabilityRouting = $true
      continue
    }
    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        $usesCapabilityRouting = $false
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($requestedBackend)) {
    $normalizedRequestedBackend = $requestedBackend.Trim().ToLowerInvariant().Replace("_", "-")
    if (-not $allowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Write-Error "requested --objc3-ir-object-backend '$requestedBackend' is not allowed by frontend core feature expansion in $featurePath"
      exit 2
    }
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    feature_path = $featurePath
    allowed_ir_object_backends = @($allowedBackends.Keys)
  }
}

function Assert-FrontendEdgeCompatibility {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $compatPath = Resolve-FrontendEdgeCompatibilityPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $compatPath -PathType Leaf)) {
    Write-Error "frontend edge compatibility artifact missing at $compatPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $compatPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge compatibility artifact is not valid JSON at $compatPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge compatibility contract id mismatch in $compatPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1",
    "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge compatibility missing dependency contract '$requiredContractId' in $compatPath"
      exit 2
    }
  }

  $edgeCompat = $payload.invocation_edge_compat
  if ($null -eq $edgeCompat) {
    Write-Error "frontend edge compatibility invocation_edge_compat metadata missing in $compatPath"
    exit 2
  }
  if ([int]$edgeCompat.fail_closed_exit_code -ne 2) {
    Write-Error "frontend edge compatibility fail_closed_exit_code must be 2 in $compatPath"
    exit 2
  }
  if (-not [bool]$edgeCompat.disallow_relative_parent_segments) {
    Write-Error "frontend edge compatibility disallow_relative_parent_segments must be true in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend edge compatibility route_flag mismatch in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend edge compatibility capability_summary_flag mismatch in $compatPath"
    exit 2
  }

  $backendCompat = $payload.backend_compat
  if ($null -eq $backendCompat) {
    Write-Error "frontend edge compatibility backend_compat metadata missing in $compatPath"
    exit 2
  }

  $canonicalBackends = @{}
  foreach ($backend in @($backendCompat.canonical_allowed_backends)) {
    $backendText = ([string]$backend).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $canonicalBackends[$backendText] = $true
    }
  }
  if ($canonicalBackends.Count -eq 0) {
    Write-Error "frontend edge compatibility canonical_allowed_backends must be non-empty in $compatPath"
    exit 2
  }
  if ($null -ne $CoreFeatureGuard) {
    foreach ($coreBackend in @($CoreFeatureGuard.allowed_ir_object_backends)) {
      $coreBackendText = ([string]$coreBackend).Trim().ToLowerInvariant()
      if (-not [string]::IsNullOrWhiteSpace($coreBackendText) -and
          -not $canonicalBackends.ContainsKey($coreBackendText)) {
        Write-Error "frontend edge compatibility missing backend '$coreBackendText' declared by frontend core feature expansion"
        exit 2
      }
    }
  }

  $aliasMap = @{}
  $aliasPayload = $backendCompat.alias_to_canonical
  if ($null -eq $aliasPayload) {
    Write-Error "frontend edge compatibility alias_to_canonical mapping missing in $compatPath"
    exit 2
  }
  foreach ($property in $aliasPayload.PSObject.Properties) {
    $alias = ([string]$property.Name).Trim().ToLowerInvariant().Replace("_", "-")
    $canonical = ([string]$property.Value).Trim().ToLowerInvariant().Replace("_", "-")
    if ([string]::IsNullOrWhiteSpace($alias) -or [string]::IsNullOrWhiteSpace($canonical)) {
      Write-Error "frontend edge compatibility alias_to_canonical entries must be non-empty in $compatPath"
      exit 2
    }
    if (-not $canonicalBackends.ContainsKey($canonical)) {
      Write-Error "frontend edge compatibility alias '$alias' maps to unknown canonical backend '$canonical' in $compatPath"
      exit 2
    }
    $aliasMap[$alias] = $canonical
  }
  foreach ($canonicalBackend in $canonicalBackends.Keys) {
    if (-not $aliasMap.ContainsKey($canonicalBackend)) {
      $aliasMap[$canonicalBackend] = $canonicalBackend
    }
  }

  $singleValueFlags = @{}
  foreach ($flag in @($backendCompat.single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $singleValueFlags[$flagText] = 0
    }
  }
  foreach ($requiredSingleValueFlag in @("--objc3-ir-object-backend", "--llvm-capabilities-summary")) {
    if (-not $singleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Write-Error "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $compatPath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $normalizedArgs = New-Object System.Collections.Generic.List[string]
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false
  $routeFlagOccurrences = 0

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $emitPrefix = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token.StartsWith("--clang=", [System.StringComparison]::Ordinal)) {
      $clangPath = $token.Substring("--clang=".Length)
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeFlagOccurrences++
      $usesCapabilityRouting = $true
      $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeFlagOccurrences++
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }

    $normalizedArgs.Add($token)
  }

  foreach ($flag in $singleValueFlags.Keys) {
    if ([int]$singleValueFlags[$flag] -gt 1) {
      Write-Error "$flag can be provided at most once"
      exit 2
    }
  }
  if ($routeFlagOccurrences -gt 1) {
    Write-Error "--objc3-route-backend-from-capabilities can be provided at most once"
    exit 2
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    edge_compat_path = $compatPath
    normalized_compile_args = $normalizedArgs.ToArray()
  }
}

function Assert-FrontendEdgeRobustness {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $robustnessPath = Resolve-FrontendEdgeRobustnessPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $robustnessPath -PathType Leaf)) {
    Write-Error "frontend edge robustness artifact missing at $robustnessPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $robustnessPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge robustness artifact is not valid JSON at $robustnessPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge robustness contract id mismatch in $robustnessPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1",
    "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge robustness missing dependency contract '$requiredContractId' in $robustnessPath"
      exit 2
    }
  }

  $guardrails = $payload.wrapper_guardrails
  if ($null -eq $guardrails) {
    Write-Error "frontend edge robustness wrapper_guardrails metadata missing in $robustnessPath"
    exit 2
  }

  $requiredWrapperSingleFlags = @("--use-cache", "--out-dir")
  $wrapperSingleSet = @{}
  foreach ($flag in @($guardrails.wrapper_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $wrapperSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredWrapperSingleFlags) {
    if (-not $wrapperSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing wrapper_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredCompileSingleFlags = @(
    "--objc3-ir-object-backend",
    "--llvm-capabilities-summary",
    "--objc3-route-backend-from-capabilities"
  )
  $compileSingleSet = @{}
  foreach ($flag in @($guardrails.compile_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $compileSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredCompileSingleFlags) {
    if (-not $compileSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing compile_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredRejectEmptyFlags = @("--emit-prefix", "--clang", "--use-cache")
  $rejectEmptySet = @{}
  foreach ($flag in @($guardrails.reject_empty_equals_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $rejectEmptySet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredRejectEmptyFlags) {
    if (-not $rejectEmptySet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing reject_empty_equals_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    edge_robustness_path = $robustnessPath
  }
}

$parsed = Parse-WrapperArguments -RawArgs $args
$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$buildResult = $null

if (-not $parsed.use_cache) {
  $buildResult = Invoke-BuildNativeCompiler -RepoRoot $repoRoot
  foreach ($lineText in @($buildResult.build_output_lines)) {
    Write-Output $lineText
  }
  $buildExit = [int]$buildResult.exit_code
  if ($buildExit -ne 0) { exit $buildExit }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "native compiler executable missing at $exe"
    exit 2
  }

  Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
  Assert-FrontendInvocationLock -RepoRoot $repoRoot -BuildResult $buildResult
  $coreFeatureGuard = Assert-FrontendCoreFeatureExpansion -RepoRoot $repoRoot -BuildResult $buildResult -ParsedArgs $parsed
  $edgeCompatGuard = Assert-FrontendEdgeCompatibility `
    -RepoRoot $repoRoot `
    -BuildResult $buildResult `
    -ParsedArgs $parsed `
    -CoreFeatureGuard $coreFeatureGuard
  Assert-FrontendEdgeRobustness -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
  $effectiveCompileArgs = @($parsed.compile_args)
  if ($null -ne $edgeCompatGuard -and $null -ne $edgeCompatGuard.normalized_compile_args) {
    $effectiveCompileArgs = @($edgeCompatGuard.normalized_compile_args)
  }
  $compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments $effectiveCompileArgs
  exit $compileExit
}

$needsBuild = -not (Test-Path -LiteralPath $exe -PathType Leaf)
if (-not $needsBuild) {
  $requiredArtifacts = @(
    Resolve-FrontendScaffoldPath -RepoRoot $repoRoot -BuildResult $buildResult,
    Resolve-FrontendInvocationLockPath -RepoRoot $repoRoot -BuildResult $buildResult,
    Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $repoRoot -BuildResult $buildResult,
    Resolve-FrontendEdgeCompatibilityPath -RepoRoot $repoRoot -BuildResult $buildResult,
    Resolve-FrontendEdgeRobustnessPath -RepoRoot $repoRoot -BuildResult $buildResult
  )
  foreach ($artifactPath in $requiredArtifacts) {
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      $needsBuild = $true
      break
    }
  }
}

if ($needsBuild) {
  $buildResult = Invoke-BuildNativeCompiler -RepoRoot $repoRoot
  foreach ($lineText in @($buildResult.build_output_lines)) {
    Write-Output $lineText
  }
  $buildExit = [int]$buildResult.exit_code
  if ($buildExit -ne 0) {
    Write-Output "cache_hit=false"
    exit $buildExit
  }
}

if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
  Write-Error "native compiler executable missing at $exe"
  exit 2
}

Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
Assert-FrontendInvocationLock -RepoRoot $repoRoot -BuildResult $buildResult
$coreFeatureGuard = Assert-FrontendCoreFeatureExpansion -RepoRoot $repoRoot -BuildResult $buildResult -ParsedArgs $parsed
$edgeCompatGuard = Assert-FrontendEdgeCompatibility `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -ParsedArgs $parsed `
  -CoreFeatureGuard $coreFeatureGuard
Assert-FrontendEdgeRobustness -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
$effectiveCompileArgs = @($parsed.compile_args)
if ($null -ne $edgeCompatGuard -and $null -ne $edgeCompatGuard.normalized_compile_args) {
  $effectiveCompileArgs = @($edgeCompatGuard.normalized_compile_args)
}

$argsWithoutOutDir = Get-ArgsWithoutOutDir -CompileArgs $effectiveCompileArgs
$inputPath = $null
if ($argsWithoutOutDir.Count -gt 0) {
  $inputCandidate = $argsWithoutOutDir[0]
  if (-not [string]::IsNullOrWhiteSpace($inputCandidate)) {
    $inputPath = [System.IO.Path]::GetFullPath($inputCandidate)
  }
}

$cacheRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/cache"
$compilerSourcePath = Join-Path $repoRoot "native/objc3c/src/main.cpp"
$cacheKey = Get-CacheKey `
  -InputPath $inputPath `
  -ArgsWithoutOutDir $argsWithoutOutDir `
  -CompilerSourcePath $compilerSourcePath `
  -WrapperScriptPath $PSCommandPath

if ($null -ne $cacheKey) {
  $entryDir = Join-Path $cacheRoot $cacheKey
  $filesDir = Join-Path $entryDir "files"
  $exitPath = Join-Path $entryDir "exit_code.txt"
  $readyPath = Join-Path $entryDir "ready.marker"

  if ((Test-Path -LiteralPath $readyPath -PathType Leaf) -and
      (Test-Path -LiteralPath $exitPath -PathType Leaf) -and
      (Test-Path -LiteralPath $filesDir -PathType Container)) {
    try {
      Copy-DirectoryContents -SourceRoot $filesDir -DestinationRoot $parsed.out_dir
      $cachedExitCode = [int](Get-Content -LiteralPath $exitPath -Raw)
      Write-Output "cache_hit=true"
      exit $cachedExitCode
    } catch {
      # fall through to cache miss path
    }
  }
}

$compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments $effectiveCompileArgs

if ($null -ne $cacheKey) {
  try {
    New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
    $stagingDir = Join-Path $cacheRoot ("_stage_" + [Guid]::NewGuid().ToString("N"))
    $stageFilesDir = Join-Path $stagingDir "files"
    New-Item -ItemType Directory -Force -Path $stageFilesDir | Out-Null

    Copy-DirectoryContents -SourceRoot $parsed.out_dir -DestinationRoot $stageFilesDir
    Set-Content -LiteralPath (Join-Path $stagingDir "exit_code.txt") -Value "$compileExit" -Encoding ascii
    Set-Content -LiteralPath (Join-Path $stagingDir "ready.marker") -Value "ready" -Encoding ascii

    $entryDir = Join-Path $cacheRoot $cacheKey
    if (Test-Path -LiteralPath $entryDir -PathType Container) {
      # Preserve existing entry and retain this write as a traceable collision artifact.
      $collisionDir = Join-Path $cacheRoot ("_collision_" + $cacheKey + "_" + [Guid]::NewGuid().ToString("N"))
      Move-Item -LiteralPath $stagingDir -Destination $collisionDir -Force
    } else {
      Move-Item -LiteralPath $stagingDir -Destination $entryDir -Force
    }
  } catch {
    # Fail closed: cache population must never block compile wrapper.
  }
}

Write-Output "cache_hit=false"
exit $compileExit
