$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_lexer_extraction_token_contract_helpers.psm1") -DisableNameChecking

function Assert-Objc3cLexerExtractionTokenContractSourceFiles {
  param([Parameter(Mandatory = $true)]$Config)

  Assert-FileExists -Path $Config.LexerHeaderPath -Id "source.lexer_header.exists" -Description "lexer header"
  Assert-FileExists -Path $Config.LexerSourcePath -Id "source.lexer_source.exists" -Description "lexer source"
  Assert-FileExists -Path $Config.TokenContractHeaderPath -Id "source.token_contract_header.exists" -Description "token contract header"
  Assert-FileExists -Path $Config.TokenHeaderPath -Id "source.token_header.exists" -Description "token header"
  Assert-FileExists -Path $Config.PipelineStageRunnerPath -Id "source.pipeline_stage_runner.exists" -Description "pipeline stage runner source"
  Assert-FileExists -Path $Config.LexCmakePath -Id "source.lex_cmake.exists" -Description "lexer CMake file"
  Assert-FileExists -Path $Config.PositiveFixturePath -Id "fixture.positive.exists" -Description "positive lexer contract fixture"
  foreach ($negativeFixture in $Config.NegativeFixtures) {
    Assert-FileExists `
      -Path $negativeFixture.path `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixture.path)) `
      -Description "negative lexer fixture"
  }
}

function Invoke-Objc3cLexerExtractionTokenContractSourceAssertions {
  param([Parameter(Mandatory = $true)]$Config)

  Assert-Objc3cLexerExtractionTokenContractSourceFiles -Config $Config

  $lexerHeaderText = Read-NormalizedText -Path $Config.LexerHeaderPath
  $lexerSourceText = Read-NormalizedText -Path $Config.LexerSourcePath
  $tokenContractHeaderText = Read-NormalizedText -Path $Config.TokenContractHeaderPath
  $tokenHeaderText = Read-NormalizedText -Path $Config.TokenHeaderPath
  $pipelineStageRunnerText = Read-NormalizedText -Path $Config.PipelineStageRunnerPath
  $lexCmakeText = Read-NormalizedText -Path $Config.LexCmakePath

  $lexerHeaderRunSignatureValid = [regex]::IsMatch(
    $lexerHeaderText,
    '(?s)class\s+Objc3Lexer\s*\{.+?std::vector<\s*(?:Objc3LexToken|Token)\s*>\s+Run\s*\(\s*std::vector<std::string>\s*&\s*diagnostics\s*\)\s*;'
  )
  Assert-Contract `
    -Condition $lexerHeaderRunSignatureValid `
    -Id "contract.lexer_header.surface" `
    -FailureMessage "lexer header missing Objc3Lexer Run diagnostics surface" `
    -PassMessage "lexer header exposes Objc3Lexer Run diagnostics contract"

  $requiredTokenKinds = @(
    "KwModule", "KwLet", "KwFn", "KwPure", "KwExtern", "KwReturn",
    "KwIf", "KwElse", "KwDo", "KwFor", "KwSwitch", "KwCase", "KwDefault",
    "KwWhile", "KwBreak", "KwContinue", "KwI32", "KwBool", "KwTrue",
    "KwFalse", "KwNil", "LessLessEqual", "GreaterGreaterEqual",
    "PlusPlus", "MinusMinus", "PercentEqual", "Question", "Tilde"
  )
  $missingTokenKinds = @($requiredTokenKinds | Where-Object { $tokenContractHeaderText -notmatch ("(?<![A-Za-z0-9_])" + [regex]::Escape($_) + "(?![A-Za-z0-9_])") })
  Assert-Contract `
    -Condition ($missingTokenKinds.Count -eq 0) `
    -Id "contract.token_contract_header.required_kinds" `
    -FailureMessage ("token contract header missing required token kinds: {0}" -f ($missingTokenKinds -join ",")) `
    -PassMessage "token contract header includes required token contract kinds" `
    -Evidence @{ missing = $missingTokenKinds }

  Assert-Contract `
    -Condition (-not (Assert-TokensPresent -Text $tokenHeaderText -RequiredTokens @("using TokenKind = Objc3LexTokenKind;", "using Token = Objc3LexToken;"))) `
    -Id "contract.token_contract_header.canonical_names" `
    -FailureMessage "token contract header still publishes retired short token names" `
    -PassMessage "token contract header publishes only explicit Objc3LexToken names"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexerSourceText -RequiredTokens @("O3L001", "O3L002", "O3L003", "O3L004", "O3C002")) `
    -Id "contract.lexer_diagnostics.codes" `
    -FailureMessage "lexer source missing one or more lexical/canonical diagnostic codes (O3L001-004, O3C002)" `
    -PassMessage "lexer source contains lexical and canonical rejection diagnostic contract codes"

  $removedAliasPatterns = @(
    '(?s)Objc3RejectedCanonicalLiteralKind::Yes:.+?\+\+canonical_literal_rejection_counts_\.yes_literal_sites;',
    '(?s)Objc3RejectedCanonicalLiteralKind::No:.+?\+\+canonical_literal_rejection_counts_\.no_literal_sites;',
    '(?s)Objc3RejectedCanonicalLiteralKind::Null:.+?\+\+canonical_literal_rejection_counts_\.null_literal_sites;'
  )
  $missingRemovedAliasPatterns = New-Object 'System.Collections.Generic.List[string]'
  foreach ($pattern in $removedAliasPatterns) {
    if (-not [regex]::IsMatch($lexerSourceText, $pattern)) {
      $missingRemovedAliasPatterns.Add($pattern) | Out-Null
    }
  }
  Assert-Contract `
    -Condition ($missingRemovedAliasPatterns.Count -eq 0) `
    -Id "contract.lexer_removed_literal_aliases" `
    -FailureMessage "lexer source missing one or more removed literal alias rejection contracts (YES/NO/NULL)" `
    -PassMessage "lexer source records canonical literal rejection counts for YES/NO/NULL" `
    -Evidence @{ missing_patterns = $missingRemovedAliasPatterns }

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexerSourceText -RequiredTokens @("ClassifyObjc3RejectedCanonicalLiteral(ident)", "Objc3RejectedCanonicalLiteralReplacementSpelling(rejected_literal)", "O3C002")) `
    -Id "contract.lexer_removed_literal_aliases.diagnostic_table" `
    -FailureMessage "lexer source missing rejected canonical literal classifier or diagnostic spelling table use" `
    -PassMessage "lexer source rejects removed literal aliases through canonical classifier and diagnostic table"

  $pipelineUsesLexer = (
    $pipelineStageRunnerText.IndexOf('#include "lex/objc3_lexer.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($pipelineStageRunnerText, '(?m)^\s*Objc3Lexer\s+lexer\s*\(\s*source(?:\s*,\s*[^)]+)?\s*\)\s*;') -and
    $pipelineStageRunnerText.IndexOf("lexer.Run(result.stage_diagnostics.lexer)", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineStageRunnerText.IndexOf("lexer.CanonicalLiteralRejectionCounts()", [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($pipelineStageRunnerText, '(?m)^\s*std::vector<\s*(?:Objc3LexToken|Token)\s*>\s+tokens\s*=\s*lexer\.Run\(')
  )
  Assert-Contract `
    -Condition $pipelineUsesLexer `
    -Id "contract.pipeline.consumes_lexer_module" `
    -FailureMessage "pipeline no longer consumes extracted lexer module surface" `
    -PassMessage "pipeline consumes extracted lexer module surface"

  Assert-Contract `
    -Condition ($pipelineStageRunnerText.IndexOf("class Objc3Lexer {", [System.StringComparison]::Ordinal) -lt 0) `
    -Id "contract.pipeline.no_inline_lexer" `
    -FailureMessage "pipeline source contains inline lexer class definition" `
    -PassMessage "pipeline source does not inline lexer implementation"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexCmakeText -RequiredTokens @("add_library(objc3c_lex STATIC", "objc3_lexer.cpp")) `
    -Id "contract.cmake.lexer_target_registered" `
    -FailureMessage "lexer CMake missing objc3c_lex target registration for objc3_lexer.cpp" `
    -PassMessage "CMake registers objc3c_lex target with extracted lexer source"
}

Export-ModuleMember -Function "Invoke-Objc3cLexerExtractionTokenContractSourceAssertions"
