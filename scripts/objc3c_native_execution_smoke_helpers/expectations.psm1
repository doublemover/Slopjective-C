Set-StrictMode -Version Latest

function Get-NativeCompileArgsFromSpec {
  param([object]$ExecutionSpec)

  $compileArgs = @()
  if ($null -ne $ExecutionSpec -and $ExecutionSpec.PSObject.Properties.Name -contains "native_compile_args") {
    foreach ($arg in @($ExecutionSpec.native_compile_args)) {
      $text = "$arg".Trim()
      if (![string]::IsNullOrWhiteSpace($text)) {
        $compileArgs += $text
      }
    }
  }
  return @($compileArgs)
}

function Get-RuntimeDispatchExpectationFromSpec {
  param([object]$ExecutionSpec)

  $requiresLiveRuntimeDispatch = $false
  $requiresLiveRuntimeDispatchExplicit = $false
  $runtimeDispatchSymbol = "objc3_runtime_dispatch_i32"

  if ($null -ne $ExecutionSpec -and $ExecutionSpec.PSObject.Properties.Name -contains "requires_runtime_link") {
    $requiresLiveRuntimeDispatch = [bool]$ExecutionSpec.requires_runtime_link
    $requiresLiveRuntimeDispatchExplicit = $true
  }
  if ($null -ne $ExecutionSpec -and $ExecutionSpec.PSObject.Properties.Name -contains "requires_live_runtime_dispatch") {
    $requiresLiveRuntimeDispatch = [bool]$ExecutionSpec.requires_live_runtime_dispatch
    $requiresLiveRuntimeDispatchExplicit = $true
  }
  if ($null -ne $ExecutionSpec -and $ExecutionSpec.PSObject.Properties.Name -contains "runtime_dispatch_symbol") {
    $candidate = "$($ExecutionSpec.runtime_dispatch_symbol)".Trim()
    if (-not [string]::IsNullOrWhiteSpace($candidate)) {
      $runtimeDispatchSymbol = $candidate
    }
  }

  return [pscustomobject]@{
    requires_live_runtime_dispatch = $requiresLiveRuntimeDispatch
    requires_live_runtime_dispatch_explicit = $requiresLiveRuntimeDispatchExplicit
    runtime_dispatch_symbol = $runtimeDispatchSymbol
  }
}

function Get-ExecutionSpecFromExpectation {
  param([object]$Spec)

  if ($null -ne $Spec -and $Spec.PSObject.Properties.Name -contains "execution") {
    return $Spec.execution
  }
  return $null
}

function Assert-ExpectationFixtureName {
  param(
    [Parameter(Mandatory = $true)][object]$Spec,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$ExpectationPath,
    [Parameter(Mandatory = $true)][string]$ExpectationKind
  )

  $fixtureName = "$($Spec.fixture)".Trim()
  if ([string]::IsNullOrWhiteSpace($fixtureName) -or $fixtureName -ne [System.IO.Path]::GetFileName($FixturePath)) {
    throw "execution smoke FAIL: $ExpectationKind expectation fixture field does not match fixture name in $ExpectationPath"
  }
}

function Read-ExpectationJson {
  param(
    [Parameter(Mandatory = $true)][string]$Path
  )

  $raw = Get-Content -LiteralPath $Path -Raw
  try {
    return $raw | ConvertFrom-Json
  }
  catch {
    throw "execution smoke FAIL: invalid json in ${Path}: $($_.Exception.Message)"
  }
}

function Get-PositiveExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectedPath = [System.IO.Path]::ChangeExtension($FixturePath, ".exitcode.txt")
  if (!(Test-Path -LiteralPath $expectedPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing positive expectation file $expectedPath"
  }

  $raw = (Get-Content -LiteralPath $expectedPath -Raw).Trim()
  $parsed = 0
  if (![int]::TryParse($raw, [ref]$parsed)) {
    throw "execution smoke FAIL: invalid exit code '$raw' in $expectedPath"
  }

  $compileArgs = @()
  $dispatchExpectation = Get-RuntimeDispatchExpectationFromSpec -ExecutionSpec $null
  $metaPath = [System.IO.Path]::ChangeExtension($FixturePath, ".meta.json")
  if (Test-Path -LiteralPath $metaPath -PathType Leaf) {
    $metaSpec = Read-ExpectationJson -Path $metaPath
    Assert-ExpectationFixtureName -Spec $metaSpec -FixturePath $FixturePath -ExpectationPath $metaPath -ExpectationKind "positive"
    $executionSpec = Get-ExecutionSpecFromExpectation -Spec $metaSpec
    $compileArgs = @(Get-NativeCompileArgsFromSpec -ExecutionSpec $executionSpec)
    $dispatchExpectation = Get-RuntimeDispatchExpectationFromSpec -ExecutionSpec $executionSpec
  }

  return [pscustomobject]@{
    expected_path = $expectedPath
    expected_exit = [int]$parsed
    compile_args = @($compileArgs)
    requires_live_runtime_dispatch = $dispatchExpectation.requires_live_runtime_dispatch
    requires_live_runtime_dispatch_explicit = $dispatchExpectation.requires_live_runtime_dispatch_explicit
    runtime_dispatch_symbol = $dispatchExpectation.runtime_dispatch_symbol
    meta_path = $metaPath
  }
}

function Get-NegativeExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectPath = [System.IO.Path]::ChangeExtension($FixturePath, ".meta.json")
  if (!(Test-Path -LiteralPath $expectPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing negative expectation file $expectPath"
  }

  $spec = Read-ExpectationJson -Path $expectPath
  $stage = "$($spec.expect_failure.stage)".Trim().ToLowerInvariant()
  if ([string]::IsNullOrWhiteSpace($stage) -or ($stage -ne "compile" -and $stage -ne "link" -and $stage -ne "run")) {
    throw "execution smoke FAIL: negative expectation requires expect_failure.stage=compile|link|run in $expectPath"
  }

  $requiredTokens = @()
  if ($null -ne $spec.expect_failure.required_diagnostic_tokens) {
    foreach ($token in @($spec.expect_failure.required_diagnostic_tokens)) {
      $text = "$token".Trim()
      if (![string]::IsNullOrWhiteSpace($text)) {
        $requiredTokens += $text
      }
    }
  }
  if ($requiredTokens.Count -eq 0) {
    throw "execution smoke FAIL: negative expectation requires expect_failure.required_diagnostic_tokens in $expectPath"
  }

  Assert-ExpectationFixtureName -Spec $spec -FixturePath $FixturePath -ExpectationPath $expectPath -ExpectationKind "negative"
  $executionSpec = Get-ExecutionSpecFromExpectation -Spec $spec
  $compileArgs = @(Get-NativeCompileArgsFromSpec -ExecutionSpec $executionSpec)
  $dispatchExpectation = Get-RuntimeDispatchExpectationFromSpec -ExecutionSpec $executionSpec

  return [pscustomobject]@{
    stage = $stage
    compile_args = @($compileArgs)
    requires_live_runtime_dispatch = $dispatchExpectation.requires_live_runtime_dispatch
    requires_live_runtime_dispatch_explicit = $dispatchExpectation.requires_live_runtime_dispatch_explicit
    runtime_dispatch_symbol = $dispatchExpectation.runtime_dispatch_symbol
    required_link_tokens = @($requiredTokens)
    expectation_path = $expectPath
  }
}

Export-ModuleMember -Function @(
  "Get-NegativeExpectation",
  "Get-PositiveExpectation"
)
