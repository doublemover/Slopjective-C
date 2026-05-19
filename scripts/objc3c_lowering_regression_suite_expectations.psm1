Set-StrictMode -Version Latest

function Get-DispatchIrExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectationPath = [System.IO.Path]::ChangeExtension($FixturePath, ".dispatch-ir.expect.txt")
  if (!(Test-Path -LiteralPath $expectationPath -PathType Leaf)) {
    return [pscustomobject]@{
      enabled = $false
      path = $expectationPath
      tokens = @()
      parse_error = ""
    }
  }

  $tokens = New-Object 'System.Collections.Generic.List[string]'
  $parseErrors = New-Object 'System.Collections.Generic.List[string]'
  $tokenSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
  $lineNumber = 0
  foreach ($line in Get-Content -LiteralPath $expectationPath) {
    $lineNumber++
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed.StartsWith("#", [System.StringComparison]::Ordinal)) {
      continue
    }
    if (-not $tokenSet.Add($trimmed)) {
      $null = $parseErrors.Add(("duplicate token at line {0}: {1}" -f $lineNumber, $trimmed))
      continue
    }
    $null = $tokens.Add($trimmed)
  }

  if ($tokens.Count -eq 0) {
    $null = $parseErrors.Add("dispatch IR expectation file has no tokens")
  }

  $parseError = if ($parseErrors.Count -gt 0) { $parseErrors -join "; " } else { "" }

  return [pscustomobject]@{
    enabled = $true
    path = $expectationPath
    tokens = @($tokens.ToArray())
    parse_error = $parseError
  }
}

function Get-Objc3IrExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectationPath = [System.IO.Path]::ChangeExtension($FixturePath, ".objc3-ir.expect.txt")
  if (!(Test-Path -LiteralPath $expectationPath -PathType Leaf)) {
    return [pscustomobject]@{
      enabled = $false
      path = $expectationPath
      tokens = @()
      parse_error = ""
    }
  }

  $tokens = New-Object 'System.Collections.Generic.List[string]'
  $parseErrors = New-Object 'System.Collections.Generic.List[string]'
  $tokenSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
  $lineNumber = 0
  foreach ($line in Get-Content -LiteralPath $expectationPath) {
    $lineNumber++
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed.StartsWith("#", [System.StringComparison]::Ordinal)) {
      continue
    }
    if (-not $tokenSet.Add($trimmed)) {
      $null = $parseErrors.Add(("duplicate token at line {0}: {1}" -f $lineNumber, $trimmed))
      continue
    }
    $null = $tokens.Add($trimmed)
  }

  if ($tokens.Count -eq 0) {
    $null = $parseErrors.Add("objc3 IR expectation file has no tokens")
  }

  $parseError = if ($parseErrors.Count -gt 0) { $parseErrors -join "; " } else { "" }

  return [pscustomobject]@{
    enabled = $true
    path = $expectationPath
    tokens = @($tokens.ToArray())
    parse_error = $parseError
  }
}

Export-ModuleMember -Function @(
  "Get-DispatchIrExpectation",
  "Get-Objc3IrExpectation"
)
