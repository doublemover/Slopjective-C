function Get-Objc3cParserReplayExpectedParserCodes {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $content = Get-Content -LiteralPath $FixturePath -Raw
  $header = [regex]::Match($content, $script:ExpectedHeaderPattern)
  if (-not $header.Success) {
    return [pscustomobject]@{
      header_found = $false
      expected_codes = @()
      non_parser_codes = @()
    }
  }

  $parsedCodes = [regex]::Matches($header.Groups[1].Value, $script:DiagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  $expectedCodes = @(Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $parsedCodes)
  $nonParserCodes = @($expectedCodes | Where-Object { $_ -notmatch $script:ParserCodePattern })
  return [pscustomobject]@{
    header_found = $true
    expected_codes = @($expectedCodes)
    non_parser_codes = @($nonParserCodes)
  }
}
