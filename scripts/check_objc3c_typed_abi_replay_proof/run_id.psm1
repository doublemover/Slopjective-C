function Resolve-Objc3cTypedAbiReplayRunId {
  param(
    [Parameter()][string]$ConfiguredRunId,
    [Parameter(Mandatory = $true)][string]$DefaultRunId
  )

  if ([string]::IsNullOrWhiteSpace($ConfiguredRunId)) {
    return $DefaultRunId
  }

  $candidate = $ConfiguredRunId.Trim()
  if ($candidate.Length -gt 80) {
    throw "typed-abi replay FAIL: configured run id exceeds 80 characters"
  }
  if ($candidate -notmatch '^[A-Za-z0-9_-]+$') {
    throw "typed-abi replay FAIL: configured run id must match ^[A-Za-z0-9_-]+$"
  }
  return $candidate
}
