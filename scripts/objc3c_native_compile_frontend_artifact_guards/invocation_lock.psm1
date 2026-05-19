$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendInvocationLock {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $payload = Read-Objc3cNativeFrontendArtifactGuardJson `
    -Path $lockPath `
    -MissingMessage "frontend invocation lock artifact missing at $lockPath" `
    -InvalidMessage "frontend invocation lock artifact is not valid JSON at $lockPath"

  Assert-Objc3cNativeFrontendArtifactGuardContractId `
    -Payload $payload `
    -ExpectedContractId "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1" `
    -ErrorMessage "frontend invocation lock contract id mismatch in $lockPath"

  if ([string]$payload.scaffold_contract_id -ne "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1") {
    Write-Error "frontend invocation lock scaffold contract id mismatch in $lockPath"
    exit 2
  }

  Assert-Objc3cNativeFrontendInvocationLockScaffold `
    -RepoRoot $RepoRoot `
    -Payload $payload `
    -LockPath $lockPath
  Assert-Objc3cNativeFrontendInvocationLockBinaries `
    -RepoRoot $RepoRoot `
    -Payload $payload `
    -LockPath $lockPath
}

function Assert-Objc3cNativeFrontendInvocationLockScaffold {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$Payload,
    [Parameter(Mandatory = $true)][string]$LockPath
  )

  $scaffold = $Payload.scaffold
  if ($null -eq $scaffold) {
    Write-Error "frontend invocation lock scaffold metadata missing in $LockPath"
    exit 2
  }

  $scaffoldRelativePath = [string]$scaffold.path
  $scaffoldExpectedHash = [string]$scaffold.sha256
  if ([string]::IsNullOrWhiteSpace($scaffoldRelativePath) -or [string]::IsNullOrWhiteSpace($scaffoldExpectedHash)) {
    Write-Error "frontend invocation lock scaffold metadata invalid in $LockPath"
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
    Write-Error "frontend invocation lock scaffold sha256 mismatch in $LockPath"
    exit 2
  }
}

function Assert-Objc3cNativeFrontendInvocationLockBinaries {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$Payload,
    [Parameter(Mandatory = $true)][string]$LockPath
  )

  $binaries = @($Payload.binaries)
  if ($binaries.Count -lt 2) {
    Write-Error "frontend invocation lock binaries list must include native and c-api runner entries in $LockPath"
    exit 2
  }

  $binaryIndex = @{}
  foreach ($binary in $binaries) {
    $binaryName = [string]$binary.name
    $binaryPath = [string]$binary.path
    $binaryHash = [string]$binary.sha256
    if ([string]::IsNullOrWhiteSpace($binaryName) -or
        [string]::IsNullOrWhiteSpace($binaryPath) -or
        [string]::IsNullOrWhiteSpace($binaryHash)) {
      Write-Error "frontend invocation lock binary entry is invalid in $LockPath"
      exit 2
    }
    if ($binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock contains duplicate binary entry '$binaryName' in $LockPath"
      exit 2
    }
    $binaryIndex[$binaryName] = $binary
  }

  Assert-Objc3cNativeFrontendInvocationLockExpectedBinaries `
    -RepoRoot $RepoRoot `
    -BinaryIndex $binaryIndex `
    -LockPath $LockPath
}

function Assert-Objc3cNativeFrontendInvocationLockExpectedBinaries {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][hashtable]$BinaryIndex,
    [Parameter(Mandatory = $true)][string]$LockPath
  )

  $expectedBinaries = [ordered]@{
    "objc3c-native" = "artifacts/bin/objc3c-native.exe"
    "objc3c-frontend-c-api-runner" = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  }
  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $BinaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock missing binary '$binaryName' in $LockPath"
      exit 2
    }
    $binary = $BinaryIndex[$binaryName]
    $expectedRelativePath = [string]$expectedBinaries[$binaryName]
    $manifestRelativePath = ([string]$binary.path).Replace('\', '/')
    if ($manifestRelativePath -ne $expectedRelativePath) {
      Write-Error "frontend invocation lock binary path mismatch for '$binaryName' in $LockPath"
      exit 2
    }
    $binaryPath = Join-Path $RepoRoot $expectedRelativePath
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      Write-Error "frontend invocation lock binary path missing at $binaryPath"
      exit 2
    }
    $binaryActualHash = Get-FileSha256Hex -Path $binaryPath
    if ($binaryActualHash -ne ([string]$binary.sha256).ToLowerInvariant()) {
      Write-Error "frontend invocation lock binary sha256 mismatch for '$binaryName' in $LockPath"
      exit 2
    }
  }
}
