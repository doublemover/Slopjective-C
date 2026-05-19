Set-StrictMode -Version Latest

function Write-Objc3cParserReplaySummary {
  param(
    [Parameter(Mandatory = $true)][string]$SummaryPath,
    [Parameter(Mandatory = $true)][string]$ProofRunId,
    [Parameter(Mandatory = $true)][string[]]$FixturePatterns,
    [Parameter(Mandatory = $true)][int]$Total,
    [Parameter(Mandatory = $true)][int]$PassedCount,
    [Parameter(Mandatory = $true)][int]$FailedCount,
    [Parameter(Mandatory = $true)][string]$Status,
    [Parameter(Mandatory = $true)][object[]]$Results
  )

  $summary = [ordered]@{
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    proof_run_id = $ProofRunId
    fixture_patterns = @($FixturePatterns)
    total = $Total
    passed = $PassedCount
    failed = $FailedCount
    status = $Status
    results = @($Results)
  }
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $SummaryPath -Encoding utf8
}

Export-ModuleMember -Function @(
  "Write-Objc3cParserReplaySummary"
)
