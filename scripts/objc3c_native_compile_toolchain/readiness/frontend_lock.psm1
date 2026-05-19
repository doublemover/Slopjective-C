$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-NativeCompilerExpectedFrontendLockBinaries {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot
  )

  return @{
    "objc3c-native" = (Join-Path $CompilerRepoRoot "artifacts/bin/objc3c-native.exe")
    "objc3c-frontend-c-api-runner" = (Join-Path $CompilerRepoRoot "artifacts/bin/objc3c-frontend-c-api-runner.exe")
  }
}

function Test-NativeCompilerFrontendLockBinaryEntriesCurrent {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [Parameter(Mandatory = $true)]$Payload
  )

  $expectedBinaries = Get-NativeCompilerExpectedFrontendLockBinaries -CompilerRepoRoot $CompilerRepoRoot
  $observed = @{}
  foreach ($entry in @($Payload.binaries)) {
    $binaryName = [string]$entry.name
    if ([string]::IsNullOrWhiteSpace($binaryName) -or -not $expectedBinaries.ContainsKey($binaryName)) {
      return $false
    }

    $binaryPath = $expectedBinaries[$binaryName]
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      return $false
    }
    $expectedRelativePath = (Resolve-Path -LiteralPath $binaryPath).Path.Substring((Resolve-Path -LiteralPath $CompilerRepoRoot).Path.Length).TrimStart('\', '/').Replace('\', '/')
    if ([string]$entry.path -ne $expectedRelativePath) {
      return $false
    }
    if ([string]$entry.sha256 -ne (Get-FileSha256Hex -Path $binaryPath)) {
      return $false
    }
    $observed[$binaryName] = $true
  }

  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $observed.ContainsKey($binaryName)) {
      return $false
    }
  }

  return $true
}

function Test-FrontendInvocationLockCurrent {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [object]$ExistingBuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult
  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult
  if (!(Test-Path -LiteralPath $lockPath -PathType Leaf)) {
    return $false
  }
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    return $false
  }

  try {
    $payload = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
  } catch {
    return $false
  }

  if ([string]$payload.contract_id -ne "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1") {
    return $false
  }
  if ([string]$payload.scaffold_contract_id -ne "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1") {
    return $false
  }
  if ($null -eq $payload.scaffold -or [string]$payload.scaffold.sha256 -ne (Get-FileSha256Hex -Path $scaffoldPath)) {
    return $false
  }

  return Test-NativeCompilerFrontendLockBinaryEntriesCurrent `
    -CompilerRepoRoot $CompilerRepoRoot `
    -Payload $payload
}
