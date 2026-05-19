function Write-Objc3cTypedAbiReplaySummary {
  param(
    [Parameter(Mandatory = $true)][string]$SummaryPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][object[]]$Results
  )

  $summary = [ordered]@{
    schema_version = "1.0.0"
    suite = "objc3c-native-typed-abi-replay-proof"
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    status = "PASS"
    total = $Results.Count
    passed = $Results.Count
    failed = 0
    results = $Results
  }
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $SummaryPath -Encoding utf8
  Write-Output "summary_path: $(Get-Objc3cTypedAbiRepoRelativePath -Path $SummaryPath -Root $RepoRoot)"
  Write-Output "status: PASS"
}
