Set-StrictMode -Version Latest

function Assert-RuntimeDispatchParityFromLl {
  param(
    [Parameter(Mandatory = $true)][string]$LlPath,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][bool]$RequiresLiveRuntimeDispatch,
    [Parameter(Mandatory = $true)][string]$RuntimeDispatchSymbol
  )

  if (!(Test-Path -LiteralPath $LlPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing module.ll for runtime-dispatch parity check ($FixtureRel)"
  }
  $llText = Get-Content -LiteralPath $LlPath -Raw
  $llCodeText = (($llText -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
  $declareToken = "declare i32 @$RuntimeDispatchSymbol("
  $callToken = "call i32 @$RuntimeDispatchSymbol("
  $hasDeclare = $llCodeText.IndexOf($declareToken, [System.StringComparison]::Ordinal) -ge 0
  $hasCall = $llCodeText.IndexOf($callToken, [System.StringComparison]::Ordinal) -ge 0

  if ($RequiresLiveRuntimeDispatch) {
    if (-not $hasDeclare -or -not $hasCall) {
      throw "execution smoke FAIL: live-runtime-dispatch metadata requires dispatch declaration+call for $FixtureRel (symbol=$RuntimeDispatchSymbol)"
    }
    return
  }

  if ($hasDeclare -or $hasCall) {
    throw "execution smoke FAIL: live-runtime-dispatch metadata forbids dispatch declaration/call for $FixtureRel (symbol=$RuntimeDispatchSymbol)"
  }
}

Export-ModuleMember -Function @(
  "Assert-RuntimeDispatchParityFromLl"
)
