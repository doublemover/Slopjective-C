$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Test-Objc3cDiagnosticsCodeSetExactMatchCore {
  param(
    [string[]]$Expected,
    [string[]]$Actual
  )

  $missing = @($Expected | Where-Object { $_ -notin $Actual })
  $unexpected = @($Actual | Where-Object { $_ -notin $Expected })
  return [pscustomobject]@{
    match = ($missing.Count -eq 0 -and $unexpected.Count -eq 0)
    missing = $missing
    unexpected = $unexpected
  }
}

function Test-Objc3cDiagnosticsExpectedCodesForRunCore {
  param(
    [pscustomobject]$ExpectedSpec,
    [pscustomobject]$Diagnostics,
    [string]$RunLabel
  )

  $errors = New-Object 'System.Collections.Generic.List[string]'
  $codesMatch = $true

  if (@($ExpectedSpec.codes).Count -gt 0) {
    if ($Diagnostics.exists -and $Diagnostics.populated) {
      $codeCheck = Test-Objc3cDiagnosticsCodeSetExactMatchCore -Expected $ExpectedSpec.codes -Actual $Diagnostics.codes
      $codesMatch = $codeCheck.match
      if (-not $codeCheck.match) {
        if (@($codeCheck.missing).Count -gt 0) {
          $null = $errors.Add("$RunLabel missing expected code(s): $($codeCheck.missing -join ', ')")
        }
        if (@($codeCheck.unexpected).Count -gt 0) {
          $null = $errors.Add("$RunLabel unexpected code(s): $($codeCheck.unexpected -join ', ')")
        }
      }
    } else {
      $codesMatch = $false
      $null = $errors.Add("$RunLabel cannot enforce expected diagnostic codes because diagnostics are unavailable")
    }
  }

  return [pscustomobject]@{
    codes_match = $codesMatch
    errors = @($errors)
  }
}

Export-ModuleMember -Function @(
  "Test-Objc3cDiagnosticsCodeSetExactMatchCore",
  "Test-Objc3cDiagnosticsExpectedCodesForRunCore"
)
