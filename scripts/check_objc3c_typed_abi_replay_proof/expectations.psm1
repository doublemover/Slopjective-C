function Get-Objc3cTypedAbiExpectationTokens {
  param([Parameter(Mandatory = $true)][string]$ExpectationPath)

  if (!(Test-Path -LiteralPath $ExpectationPath -PathType Leaf)) {
    throw "typed-abi replay FAIL: missing expectation file $ExpectationPath"
  }

  $tokens = New-Object 'System.Collections.Generic.List[string]'
  $tokenSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
  $lineNo = 0
  foreach ($line in Get-Content -LiteralPath $ExpectationPath) {
    $lineNo++
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed.StartsWith("#", [System.StringComparison]::Ordinal)) {
      continue
    }
    if (-not $tokenSet.Add($trimmed)) {
      throw "typed-abi replay FAIL: duplicate expectation token at line $lineNo in $ExpectationPath"
    }
    $null = $tokens.Add($trimmed)
  }
  if ($tokens.Count -eq 0) {
    throw "typed-abi replay FAIL: expectation file has no tokens: $ExpectationPath"
  }
  return @($tokens.ToArray())
}

function Get-Objc3cTypedAbiMissingTokens {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$Tokens
  )

  $missing = New-Object 'System.Collections.Generic.List[string]'
  foreach ($token in $Tokens) {
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      $null = $missing.Add($token)
    }
  }
  return @($missing.ToArray())
}

function Get-Objc3cTypedAbiFixturePathFromExpectation {
  param([Parameter(Mandatory = $true)][string]$ExpectationPath)

  $suffix = ".objc3-ir.expect.txt"
  if (-not $ExpectationPath.EndsWith($suffix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "typed-abi replay FAIL: unsupported expectation filename $ExpectationPath (expected suffix $suffix)"
  }
  $root = $ExpectationPath.Substring(0, $ExpectationPath.Length - $suffix.Length)
  return "$root.objc3"
}
