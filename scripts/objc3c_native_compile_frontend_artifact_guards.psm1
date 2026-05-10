$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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

  $expectedContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend modular scaffold contract id mismatch in $scaffoldPath"
    exit 2
  }

  $modules = @($payload.modules)
  $requiredModules = @("driver","diagnostics-io","ir","lex-parse","frontend-api","lowering","pipeline","sema")
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

  $expectedContractId = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend invocation lock contract id mismatch in $lockPath"
    exit 2
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
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

Export-ModuleMember -Function @("Assert-FrontendModuleScaffold", "Assert-FrontendInvocationLock")
