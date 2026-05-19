$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "..\objc3c_native_artifact_io.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "constants.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "loading.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "payload_edge_compat.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "payload_edge_robustness.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "payload_diagnostics.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "payload_recovery.psm1") -Force -DisableNameChecking

function Write-Objc3cNativeFrontendEdgeCompatibilityArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendCoreFeatureExpansionPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $coreFeaturePayload = Read-Objc3cNativeFrontendCloseoutEdgeJsonArtifact `
    -Path $FrontendCoreFeatureExpansionPath `
    -MissingMessage "frontend core feature expansion missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath" `
    -InvalidJsonMessage "frontend core feature expansion is not valid JSON for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"

  Assert-Objc3cNativeFrontendCloseoutEdgeContractId `
    -Payload $coreFeaturePayload `
    -ExpectedContractId $contracts.CoreFeatureExpansion `
    -MismatchMessage "frontend core feature contract id mismatch for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"

  $allowedBackends = @(Get-Objc3cNativeFrontendCloseoutEdgeAllowedBackends `
    -CoreFeaturePayload $coreFeaturePayload `
    -MissingMessage "frontend core feature expansion allowed_ir_object_backends missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath")

  $payload = New-Objc3cNativeFrontendEdgeCompatibilityPayload -AllowedBackends $allowedBackends
  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendEdgeRobustnessArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeCompatibilityPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $edgeCompatPayload = Read-Objc3cNativeFrontendCloseoutEdgeJsonArtifact `
    -Path $FrontendEdgeCompatibilityPath `
    -MissingMessage "frontend edge compatibility artifact missing for edge robustness artifact: $FrontendEdgeCompatibilityPath" `
    -InvalidJsonMessage "frontend edge compatibility artifact is not valid JSON for edge robustness artifact: $FrontendEdgeCompatibilityPath"

  Assert-Objc3cNativeFrontendCloseoutEdgeContractId `
    -Payload $edgeCompatPayload `
    -ExpectedContractId $contracts.EdgeCompatCompletion `
    -MismatchMessage "frontend edge compatibility contract id mismatch for edge robustness artifact: $FrontendEdgeCompatibilityPath"

  $payload = New-Objc3cNativeFrontendEdgeRobustnessPayload
  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendDiagnosticsHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeRobustnessPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $edgeRobustnessPayload = Read-Objc3cNativeFrontendCloseoutEdgeJsonArtifact `
    -Path $FrontendEdgeRobustnessPath `
    -MissingMessage "frontend edge robustness artifact missing for diagnostics hardening artifact: $FrontendEdgeRobustnessPath" `
    -InvalidJsonMessage "frontend edge robustness artifact is not valid JSON for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"

  Assert-Objc3cNativeFrontendCloseoutEdgeContractId `
    -Payload $edgeRobustnessPayload `
    -ExpectedContractId $contracts.EdgeRobustness `
    -MismatchMessage "frontend edge robustness contract id mismatch for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"

  $payload = New-Objc3cNativeFrontendDiagnosticsHardeningPayload
  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendRecoveryDeterminismHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendDiagnosticsHardeningPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $diagnosticsPayload = Read-Objc3cNativeFrontendCloseoutEdgeJsonArtifact `
    -Path $FrontendDiagnosticsHardeningPath `
    -MissingMessage "frontend diagnostics hardening artifact missing for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath" `
    -InvalidJsonMessage "frontend diagnostics hardening artifact is not valid JSON for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"

  Assert-Objc3cNativeFrontendCloseoutEdgeContractId `
    -Payload $diagnosticsPayload `
    -ExpectedContractId $contracts.DiagnosticsHardening `
    -MismatchMessage "frontend diagnostics hardening contract id mismatch for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"

  $payload = New-Objc3cNativeFrontendRecoveryDeterminismHardeningPayload
  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}
