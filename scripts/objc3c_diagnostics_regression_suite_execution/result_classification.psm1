$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$script:Objc3cDiagnosticsScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:Objc3cDiagnosticsScriptsRoot "objc3c_diagnostics_regression_suite_assertions.psm1") -Force -DisableNameChecking

function New-Objc3cDiagnosticsRegressionCaseClassification {
  param(
    [pscustomobject]$ExpectedSpec,
    [int]$Run1ExitCode,
    [int]$Run2ExitCode,
    [pscustomobject]$Run1Diagnostics,
    [pscustomobject]$Run2Diagnostics,
    [pscustomobject]$Run1DiagnosticsJson,
    [pscustomobject]$Run2DiagnosticsJson,
    [string]$Run1OutDir,
    [string]$Run2OutDir
  )

  $errors = New-Object 'System.Collections.Generic.List[string]'

  if (-not $ExpectedSpec.header_found) {
    $null = $errors.Add("missing expected diagnostic header")
  } elseif (@($ExpectedSpec.codes).Count -eq 0) {
    $null = $errors.Add("expected diagnostic header has no parseable O3* codes")
  }

  if ($Run1ExitCode -eq 0) {
    $null = $errors.Add("run1 expected nonzero exit but got 0")
  }
  if ($Run2ExitCode -eq 0) {
    $null = $errors.Add("run2 expected nonzero exit but got 0")
  }
  if ($Run1ExitCode -ne $Run2ExitCode) {
    $null = $errors.Add("exit code drift across replay ($Run1ExitCode vs $Run2ExitCode)")
  }

  if (-not $Run1Diagnostics.exists) {
    $null = $errors.Add("run1 missing module.diagnostics.txt")
  } elseif (-not $Run1Diagnostics.populated) {
    $null = $errors.Add("run1 diagnostics file is empty")
  }

  if (-not $Run2Diagnostics.exists) {
    $null = $errors.Add("run2 missing module.diagnostics.txt")
  } elseif (-not $Run2Diagnostics.populated) {
    $null = $errors.Add("run2 diagnostics file is empty")
  }

  if (-not $Run1DiagnosticsJson.exists) {
    $null = $errors.Add("run1 missing module.diagnostics.json")
  } elseif (-not $Run1DiagnosticsJson.populated) {
    $null = $errors.Add("run1 diagnostics json file is empty")
  } elseif (-not $Run1DiagnosticsJson.valid) {
    $null = $errors.Add("run1 diagnostics json invalid: $($Run1DiagnosticsJson.parse_error)")
  }

  if (-not $Run2DiagnosticsJson.exists) {
    $null = $errors.Add("run2 missing module.diagnostics.json")
  } elseif (-not $Run2DiagnosticsJson.populated) {
    $null = $errors.Add("run2 diagnostics json file is empty")
  } elseif (-not $Run2DiagnosticsJson.valid) {
    $null = $errors.Add("run2 diagnostics json invalid: $($Run2DiagnosticsJson.parse_error)")
  }

  $diagnosticsDeterministic = $false
  if ($Run1Diagnostics.exists -and $Run2Diagnostics.exists) {
    $diagnosticsDeterministic = Test-Objc3cDiagnosticsByteArrayEqual -Left $Run1Diagnostics.bytes -Right $Run2Diagnostics.bytes
    if (-not $diagnosticsDeterministic) {
      $null = $errors.Add("diagnostics replay mismatch: run1/run2 bytes differ")
    }
  }

  $diagnosticsJsonDeterministic = $false
  if ($Run1DiagnosticsJson.exists -and $Run2DiagnosticsJson.exists) {
    $diagnosticsJsonDeterministic = Test-Objc3cDiagnosticsByteArrayEqual -Left $Run1DiagnosticsJson.bytes -Right $Run2DiagnosticsJson.bytes
    if (-not $diagnosticsJsonDeterministic) {
      $null = $errors.Add("diagnostics json replay mismatch: run1/run2 bytes differ")
    }
  }

  $run1JsonCodesMatchText = $false
  if ($Run1Diagnostics.exists -and $Run1Diagnostics.populated -and $Run1DiagnosticsJson.exists -and $Run1DiagnosticsJson.valid) {
    $run1JsonCodeCheck = Test-Objc3cDiagnosticsCodeSetExactMatch -Expected $Run1Diagnostics.codes -Actual $Run1DiagnosticsJson.codes
    $run1JsonCodesMatchText = $run1JsonCodeCheck.match
    if (-not $run1JsonCodeCheck.match) {
      if (@($run1JsonCodeCheck.missing).Count -gt 0) {
        $null = $errors.Add("run1 diagnostics json missing code(s) present in text: $($run1JsonCodeCheck.missing -join ', ')")
      }
      if (@($run1JsonCodeCheck.unexpected).Count -gt 0) {
        $null = $errors.Add("run1 diagnostics json has unexpected code(s) vs text: $($run1JsonCodeCheck.unexpected -join ', ')")
      }
    }
  }

  $run2JsonCodesMatchText = $false
  if ($Run2Diagnostics.exists -and $Run2Diagnostics.populated -and $Run2DiagnosticsJson.exists -and $Run2DiagnosticsJson.valid) {
    $run2JsonCodeCheck = Test-Objc3cDiagnosticsCodeSetExactMatch -Expected $Run2Diagnostics.codes -Actual $Run2DiagnosticsJson.codes
    $run2JsonCodesMatchText = $run2JsonCodeCheck.match
    if (-not $run2JsonCodeCheck.match) {
      if (@($run2JsonCodeCheck.missing).Count -gt 0) {
        $null = $errors.Add("run2 diagnostics json missing code(s) present in text: $($run2JsonCodeCheck.missing -join ', ')")
      }
      if (@($run2JsonCodeCheck.unexpected).Count -gt 0) {
        $null = $errors.Add("run2 diagnostics json has unexpected code(s) vs text: $($run2JsonCodeCheck.unexpected -join ', ')")
      }
    }
  }

  $unexpectedArtifactsRun1 = @(Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts -OutDir $Run1OutDir)
  if ($unexpectedArtifactsRun1.Count -gt 0) {
    $null = $errors.Add("run1 fail-closed violation: emitted artifact(s) $($unexpectedArtifactsRun1 -join ', ')")
  }

  $unexpectedArtifactsRun2 = @(Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts -OutDir $Run2OutDir)
  if ($unexpectedArtifactsRun2.Count -gt 0) {
    $null = $errors.Add("run2 fail-closed violation: emitted artifact(s) $($unexpectedArtifactsRun2 -join ', ')")
  }

  $run1CodeCheck = Test-Objc3cDiagnosticsExpectedCodesForRun -ExpectedSpec $ExpectedSpec -Diagnostics $Run1Diagnostics -RunLabel "run1"
  $run1CodeMatch = [bool]$run1CodeCheck.codes_match
  foreach ($errorMessage in $run1CodeCheck.errors) {
    $null = $errors.Add($errorMessage)
  }
  $run2CodeCheck = Test-Objc3cDiagnosticsExpectedCodesForRun -ExpectedSpec $ExpectedSpec -Diagnostics $Run2Diagnostics -RunLabel "run2"
  $run2CodeMatch = [bool]$run2CodeCheck.codes_match
  foreach ($errorMessage in $run2CodeCheck.errors) {
    $null = $errors.Add($errorMessage)
  }

  return [pscustomobject]@{
    passed = ($errors.Count -eq 0)
    checks = [ordered]@{
      exit_nonzero = (($Run1ExitCode -ne 0) -and ($Run2ExitCode -ne 0))
      exit_deterministic = ($Run1ExitCode -eq $Run2ExitCode)
      diagnostics_present = ($Run1Diagnostics.exists -and $Run2Diagnostics.exists)
      diagnostics_nonempty = ($Run1Diagnostics.populated -and $Run2Diagnostics.populated)
      diagnostics_deterministic = $diagnosticsDeterministic
      diagnostics_json_present = ($Run1DiagnosticsJson.exists -and $Run2DiagnosticsJson.exists)
      diagnostics_json_nonempty = ($Run1DiagnosticsJson.populated -and $Run2DiagnosticsJson.populated)
      diagnostics_json_valid = ($Run1DiagnosticsJson.valid -and $Run2DiagnosticsJson.valid)
      diagnostics_json_deterministic = $diagnosticsJsonDeterministic
      diagnostics_json_codes_match_text = ($run1JsonCodesMatchText -and $run2JsonCodesMatchText)
      codes_match_expected = ($run1CodeMatch -and $run2CodeMatch)
      fail_closed_artifacts = ($unexpectedArtifactsRun1.Count -eq 0 -and $unexpectedArtifactsRun2.Count -eq 0)
    }
    errors = @($errors)
  }
}

Export-ModuleMember -Function @(
  "New-Objc3cDiagnosticsRegressionCaseClassification"
)
