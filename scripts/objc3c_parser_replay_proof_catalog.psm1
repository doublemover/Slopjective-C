Set-StrictMode -Version Latest

function Get-Objc3cParserReplayFixturePatterns {
  return @(
    "negative_loop_control_parser_*.objc3",
    "negative_assignment_parser_*.objc3",
    "negative_bitwise_parser_*.objc3",
    "negative_modulo_parser_*.objc3",
    "negative_do_while_parser_*.objc3",
    "negative_for_parser_*.objc3",
    "negative_for_step_parser_*.objc3",
    "negative_foundation_alias_parser_*.objc3",
    "negative_id_parser_*.objc3",
    "negative_prototype_parser_*.objc3",
    "negative_message_missing_keyword_colon.objc3",
    "negative_message_unterminated.objc3",
    "negative_message_parser_*.objc3",
    "negative_return_parser_*.objc3",
    "negative_nested_block_parser_*.objc3",
    "negative_switch_parser_*.objc3",
    "negative_conditional_parser_*.objc3",
    "negative_nil_literal_parser_*.objc3",
    "negative_hex_literal_parser_*.objc3",
    "negative_binary_literal_parser_*.objc3",
    "negative_octal_literal_parser_*.objc3",
    "negative_separator_literal_parser_*.objc3",
    "negative_unary_plus_parser_*.objc3"
  )
}

function Get-Objc3cParserReplayFixtures {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureDir,
    [Parameter(Mandatory = $true)][string[]]$FixturePatterns
  )

  $fixturesByPath = [System.Collections.Generic.Dictionary[string, System.IO.FileInfo]]::new([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($pattern in $FixturePatterns) {
    $matched = @(Get-ChildItem -LiteralPath $FixtureDir -File -Filter $pattern | Sort-Object -Property Name)
    if ($matched.Count -eq 0) {
      Write-Output ("error: parser replay proof FAIL: expected parser fixtures matching {0}; found 0" -f $pattern)
      exit 1
    }
    foreach ($fixture in $matched) {
      if (-not $fixturesByPath.ContainsKey($fixture.FullName)) {
        $fixturesByPath.Add($fixture.FullName, $fixture)
      }
    }
  }

  $fixtures = @($fixturesByPath.Values | Sort-Object -Property Name)
  if ($fixtures.Count -lt 36) {
    Write-Output ("error: parser replay proof FAIL: expected at least 36 parser fixtures across patterns; found {0}" -f $fixtures.Count)
    exit 1
  }

  return $fixtures
}

Export-ModuleMember -Function @(
  "Get-Objc3cParserReplayFixturePatterns",
  "Get-Objc3cParserReplayFixtures"
)
