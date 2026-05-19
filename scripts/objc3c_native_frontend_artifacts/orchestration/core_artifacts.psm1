$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "..\..\objc3c_native_artifact_io.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\constants.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\loading.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\payloads.psm1") -Force -DisableNameChecking

function Write-Objc3cNativeFrontendModuleScaffoldArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string[]]$BinaryTargets
  )

  $payload = New-Objc3cNativeFrontendModuleScaffoldPayload `
    -Modules $Modules `
    -SharedSources $SharedSources `
    -BinaryTargets $BinaryTargets

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendInvocationLockArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $NativeBinaryPath `
    -MissingMessage "native binary missing for invocation lock artifact: $NativeBinaryPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $CapiBinaryPath `
    -MissingMessage "c-api runner missing for invocation lock artifact: $CapiBinaryPath"

  $scaffoldPayload = Read-Objc3cNativeFrontendJsonArtifact `
    -Path $FrontendScaffoldPath `
    -MissingMessage "frontend source graph missing for invocation lock artifact: $FrontendScaffoldPath" `
    -InvalidJsonMessage "frontend source graph is not valid JSON for invocation lock artifact: $FrontendScaffoldPath"

  Assert-Objc3cNativeFrontendArtifactContractId `
    -Payload $scaffoldPayload `
    -ExpectedContractId $contracts.ModuleScaffold `
    -MismatchMessage "frontend source graph contract id mismatch for invocation lock artifact: $FrontendScaffoldPath"

  $payload = New-Objc3cNativeFrontendInvocationLockPayload `
    -RepoRoot $RepoRoot `
    -ScaffoldPayload $scaffoldPayload `
    -FrontendScaffoldPath $FrontendScaffoldPath `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendCoreFeatureExpansionArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendInvocationLockPath
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $FrontendScaffoldPath `
    -MissingMessage "frontend source graph missing for core feature expansion artifact: $FrontendScaffoldPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $FrontendInvocationLockPath `
    -MissingMessage "frontend invocation lock missing for core feature expansion artifact: $FrontendInvocationLockPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $NativeBinaryPath `
    -MissingMessage "native binary missing for core feature expansion artifact: $NativeBinaryPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $CapiBinaryPath `
    -MissingMessage "c-api runner missing for core feature expansion artifact: $CapiBinaryPath"

  $scaffoldPayload = Read-Objc3cNativeFrontendJsonArtifact `
    -Path $FrontendScaffoldPath `
    -MissingMessage "frontend source graph missing for core feature expansion artifact: $FrontendScaffoldPath" `
    -InvalidJsonMessage "frontend source graph is not valid JSON for core feature expansion artifact: $FrontendScaffoldPath"

  $invocationLockPayload = Read-Objc3cNativeFrontendJsonArtifact `
    -Path $FrontendInvocationLockPath `
    -MissingMessage "frontend invocation lock missing for core feature expansion artifact: $FrontendInvocationLockPath" `
    -InvalidJsonMessage "frontend invocation lock is not valid JSON for core feature expansion artifact: $FrontendInvocationLockPath"

  Assert-Objc3cNativeFrontendArtifactContractId `
    -Payload $scaffoldPayload `
    -ExpectedContractId $contracts.ModuleScaffold `
    -MismatchMessage "frontend source graph contract id mismatch for core feature expansion artifact: $FrontendScaffoldPath"
  Assert-Objc3cNativeFrontendArtifactContractId `
    -Payload $invocationLockPayload `
    -ExpectedContractId $contracts.InvocationLock `
    -MismatchMessage "frontend invocation lock contract id mismatch for core feature expansion artifact: $FrontendInvocationLockPath"

  $payload = New-Objc3cNativeFrontendCoreFeatureExpansionPayload `
    -RepoRoot $RepoRoot `
    -Modules $Modules `
    -SharedSources $SharedSources `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}
