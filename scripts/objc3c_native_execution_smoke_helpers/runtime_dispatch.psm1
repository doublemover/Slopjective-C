Set-StrictMode -Version Latest

function Assert-RuntimeDispatchParityFromLl {
  param(
    [Parameter(Mandatory = $true)][string]$LlPath,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][bool]$RequiresLiveRuntimeDispatch,
    [Parameter(Mandatory = $true)][string[]]$RuntimeDispatchSymbols
  )

  if (!(Test-Path -LiteralPath $LlPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing module.ll for runtime-dispatch parity check ($FixtureRel)"
  }
  $llText = Get-Content -LiteralPath $LlPath -Raw
  $llCodeText = (($llText -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
  $allowedRuntimeDispatchSymbols = @(
    "objc3_runtime_dispatch_i32",
    "objc3_runtime_dispatch_i32_from_class",
    "objc3_runtime_dispatch_typed_value",
    "objc3_runtime_dispatch_typed_value_from_class"
  )
  $symbolsToCheck = @($RuntimeDispatchSymbols)
  if (-not $RequiresLiveRuntimeDispatch) {
    $symbolsToCheck = @($allowedRuntimeDispatchSymbols)
  }
  foreach ($runtimeDispatchSymbol in @($symbolsToCheck)) {
    $declareToken = "declare i32 @$runtimeDispatchSymbol("
    $callToken = "call i32 @$runtimeDispatchSymbol("
    $hasDeclare = $llCodeText.IndexOf($declareToken, [System.StringComparison]::Ordinal) -ge 0
    $hasCall = $llCodeText.IndexOf($callToken, [System.StringComparison]::Ordinal) -ge 0

    if ($RequiresLiveRuntimeDispatch) {
      if (-not $hasDeclare -or -not $hasCall) {
        throw "execution smoke FAIL: live-runtime-dispatch metadata requires dispatch declaration+call for $FixtureRel (symbol=$runtimeDispatchSymbol)"
      }
      continue
    }

    if ($hasDeclare -or $hasCall) {
      throw "execution smoke FAIL: live-runtime-dispatch metadata forbids dispatch declaration/call for $FixtureRel (symbol=$runtimeDispatchSymbol)"
    }
  }
}

Export-ModuleMember -Function @(
  "Assert-RuntimeDispatchParityFromLl"
)
