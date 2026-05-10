$ErrorActionPreference = "Stop"

function Write-Objc3cNativeFrontendCloseoutConformancePayload {
  param(
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    $Payload,
    [Parameter(Mandatory = $true)]
    [int]$Depth
  )

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($Payload | ConvertTo-Json -Depth $Depth) -Encoding utf8
}

function Write-Objc3cNativeFrontendConformanceMatrixArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendRecoveryDeterminismHardeningPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds
  $recoveryPayload = Read-Objc3cNativeFrontendCloseoutConformanceJsonArtifact `
    -Path $FrontendRecoveryDeterminismHardeningPath `
    -MissingMessage "frontend recovery determinism hardening artifact missing for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath" `
    -InvalidJsonMessage "frontend recovery determinism hardening artifact is not valid JSON for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"

  Assert-Objc3cNativeFrontendCloseoutConformanceContractId `
    -Payload $recoveryPayload `
    -ExpectedContractId $contracts.RecoveryDeterminismHardening `
    -MismatchMessage "frontend recovery determinism hardening contract id mismatch for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"

  $payload = New-Objc3cNativeFrontendConformanceMatrixPayload
  Write-Objc3cNativeFrontendCloseoutConformancePayload -OutputPath $OutputPath -Payload $payload -Depth 10
}

function Write-Objc3cNativeFrontendConformanceCorpusArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceMatrixPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds
  $matrixPayload = Read-Objc3cNativeFrontendCloseoutConformanceJsonArtifact `
    -Path $FrontendConformanceMatrixPath `
    -MissingMessage "frontend conformance matrix artifact missing for conformance corpus artifact: $FrontendConformanceMatrixPath" `
    -InvalidJsonMessage "frontend conformance matrix artifact is not valid JSON for conformance corpus artifact: $FrontendConformanceMatrixPath"

  Assert-Objc3cNativeFrontendCloseoutConformanceContractId `
    -Payload $matrixPayload `
    -ExpectedContractId $contracts.ConformanceMatrix `
    -MismatchMessage "frontend conformance matrix contract id mismatch for conformance corpus artifact: $FrontendConformanceMatrixPath"

  $payload = New-Objc3cNativeFrontendConformanceCorpusPayload -MatrixPayload $matrixPayload
  Write-Objc3cNativeFrontendCloseoutConformancePayload -OutputPath $OutputPath -Payload $payload -Depth 12
}

function Write-Objc3cNativeFrontendIntegrationCloseoutArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceCorpusPath
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds
  $corpusPayload = Read-Objc3cNativeFrontendCloseoutConformanceJsonArtifact `
    -Path $FrontendConformanceCorpusPath `
    -MissingMessage "frontend conformance corpus artifact missing for integration closeout artifact: $FrontendConformanceCorpusPath" `
    -InvalidJsonMessage "frontend conformance corpus artifact is not valid JSON for integration closeout artifact: $FrontendConformanceCorpusPath"

  Assert-Objc3cNativeFrontendCloseoutConformanceContractId `
    -Payload $corpusPayload `
    -ExpectedContractId $contracts.ConformanceCorpus `
    -MismatchMessage "frontend conformance corpus contract id mismatch for integration closeout artifact: $FrontendConformanceCorpusPath"

  $acceptanceCount = [int]$corpusPayload.acceptance_corpus_count
  $rejectionCount = [int]$corpusPayload.rejection_corpus_count
  Assert-Objc3cNativeFrontendCloseoutConformanceCorpusCoverage `
    -AcceptanceCount $acceptanceCount `
    -RejectionCount $rejectionCount

  $payload = New-Objc3cNativeFrontendIntegrationCloseoutPayload `
    -AcceptanceCount $acceptanceCount `
    -RejectionCount $rejectionCount
  Write-Objc3cNativeFrontendCloseoutConformancePayload -OutputPath $OutputPath -Payload $payload -Depth 12
}
