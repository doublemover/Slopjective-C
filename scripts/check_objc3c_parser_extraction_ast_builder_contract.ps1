$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_parser_extraction_ast_builder_contract_helpers.psm1") -Force -DisableNameChecking

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/parser-extraction-ast-builder-contract"
$configuredRunId = $env:OBJC3C_PARSER_AST_CONTRACT_RUN_ID
$runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$runDirRel = "tmp/artifacts/objc3c-native/parser-extraction-ast-builder-contract/$runId"
$summaryRel = "$runDirRel/summary.json"

$buildScriptPath = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
$nativeExePath = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExePath } else { $configuredNativeExe }
$nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)

$parserHeaderPath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_parser.h"
$parserSourcePath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_parser.cpp"
$parserContractHeaderPath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_parser_contract.h"
$astBuilderScaffoldHeaderPath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder.h"
$astBuilderScaffoldSourcePath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder.cpp"
$astBuilderContractHeaderPath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder_contract.h"
$astBuilderContractSourcePath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder_contract.cpp"
$astHeaderPath = Join-Path $repoRoot "native/objc3c/src/ast/objc3_ast.h"
$pipelineSourcePath = Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
$cmakePath = Join-Path $repoRoot "native/objc3c/CMakeLists.txt"

$positiveFixturePath = Join-Path $repoRoot "tests/tooling/fixtures/native/parser_split/positive_ast_builder_scaffold_smoke.objc3"
$negativeFixturePaths = @(
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_conditional_parser_missing_colon.objc3"),
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_return_parser_missing_semicolon.objc3"),
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_loop_control_parser_missing_while_paren.objc3"),
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_message_unterminated.objc3")
)

$checks = New-Object 'System.Collections.Generic.List[object]'
$caseResults = New-Object 'System.Collections.Generic.List[object]'
$hadFatalError = $false
$fatalErrorMessage = ""

Set-ParserExtractionAstBuilderContractContext -Checks $checks -RepoRoot $repoRoot

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

Push-Location $repoRoot
try {
  Assert-FileExists -Path $parserHeaderPath -Id "source.parser_header.exists" -Description "parser header"
  Assert-FileExists -Path $parserSourcePath -Id "source.parser_source.exists" -Description "parser source"
  Assert-FileExists -Path $parserContractHeaderPath -Id "source.parser_contract_header.exists" -Description "parser contract header"
  Assert-FileExists -Path $astBuilderScaffoldHeaderPath -Id "source.ast_builder_scaffold_header.exists" -Description "AST builder scaffold header"
  Assert-FileExists -Path $astBuilderScaffoldSourcePath -Id "source.ast_builder_scaffold_source.exists" -Description "AST builder scaffold source"
  Assert-FileExists -Path $astBuilderContractHeaderPath -Id "source.ast_builder_contract_header.exists" -Description "AST builder contract header"
  Assert-FileExists -Path $astBuilderContractSourcePath -Id "source.ast_builder_contract_source.exists" -Description "AST builder contract source"
  Assert-FileExists -Path $astHeaderPath -Id "source.ast_header.exists" -Description "AST header"
  Assert-FileExists -Path $pipelineSourcePath -Id "source.pipeline_source.exists" -Description "pipeline source"
  Assert-FileExists -Path $cmakePath -Id "source.cmake.exists" -Description "native CMake file"
  Assert-FileExists -Path $positiveFixturePath -Id "fixture.positive.exists" -Description "positive parser scaffold fixture"
  foreach ($negativeFixturePath in $negativeFixturePaths) {
    Assert-FileExists `
      -Path $negativeFixturePath `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)) `
      -Description "negative parser fixture"
  }

  $parserHeaderText = Read-NormalizedText -Path $parserHeaderPath
  $parserSourceText = Read-NormalizedText -Path $parserSourcePath
  $parserContractHeaderText = Read-NormalizedText -Path $parserContractHeaderPath
  $astBuilderScaffoldHeaderText = Read-NormalizedText -Path $astBuilderScaffoldHeaderPath
  $astBuilderScaffoldSourceText = Read-NormalizedText -Path $astBuilderScaffoldSourcePath
  $astBuilderContractHeaderText = Read-NormalizedText -Path $astBuilderContractHeaderPath
  $astBuilderContractSourceText = Read-NormalizedText -Path $astBuilderContractSourcePath
  $astHeaderText = Read-NormalizedText -Path $astHeaderPath
  $pipelineSourceText = Read-NormalizedText -Path $pipelineSourcePath
  $cmakeText = Read-NormalizedText -Path $cmakePath

  $parserHeaderSurfaceValid = (
    $parserHeaderText.IndexOf('#include "parse/objc3_parser_contract.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $parserHeaderText.IndexOf('#include "token/objc3_token_contract.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $parserHeaderText.IndexOf("struct Objc3ParseResult", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserHeaderText.IndexOf("Objc3ParsedProgram program;", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserHeaderText.IndexOf("std::vector<std::string> diagnostics;", [System.StringComparison]::Ordinal) -ge 0 -and
    (
      [regex]::IsMatch($parserHeaderText, 'ParseObjc3Program\s*\(\s*const\s+std::vector<\s*Objc3LexToken\s*>\s*&\s*tokens\s*\)\s*;') -or
      [regex]::IsMatch($parserHeaderText, 'ParseObjc3Program\s*\(\s*const\s+Objc3LexTokenStream\s*&\s*tokens\s*\)\s*;')
    )
  )
  Assert-Contract `
    -Condition $parserHeaderSurfaceValid `
    -Id "contract.parser_header.surface" `
    -FailureMessage "parser header missing expected extracted parser API surface" `
    -PassMessage "parser header exposes extracted parser API surface"

  $parserContractAliasModel = (
    $parserContractHeaderText.IndexOf('#include "ast/objc3_ast.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("using Objc3ParsedProgram = Objc3Program;", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("using Objc3ParsedGlobalDecl = GlobalDecl;", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("using Objc3ParsedFunctionDecl = FunctionDecl;", [System.StringComparison]::Ordinal) -ge 0
  )
  $parserContractWrapperModel = (
    $parserContractHeaderText.IndexOf('#include "ast/objc3_ast.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("struct Objc3ParsedProgram", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("Objc3Program ast;", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("using Objc3ParsedGlobalDecl = GlobalDecl;", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("using Objc3ParsedFunctionDecl = FunctionDecl;", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("MutableObjc3ParsedProgramAst(", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserContractHeaderText.IndexOf("Objc3ParsedProgramAst(", [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition ($parserContractAliasModel -or $parserContractWrapperModel) `
    -Id "contract.parser_contract_header.aliases" `
    -FailureMessage "parser contract header missing parser-to-sema contract surface" `
    -PassMessage "parser contract header exposes parser-to-sema contract surface"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $astBuilderScaffoldHeaderText -RequiredTokens @('#include "parse/objc3_parser_contract.h"', "class Objc3AstBuilder", "Objc3ParsedProgram BeginProgram() const;", "void SetModuleName(Objc3ParsedProgram &program, std::string module_name) const;", "void AddGlobalDecl(Objc3ParsedProgram &program, Objc3ParsedGlobalDecl decl) const;", "void AddFunctionDecl(Objc3ParsedProgram &program, Objc3ParsedFunctionDecl decl) const;")) `
    -Id "contract.ast_builder_scaffold_header.surface" `
    -FailureMessage "AST builder scaffold header missing expected surface" `
    -PassMessage "AST builder scaffold header exposes expected surface"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $astBuilderScaffoldSourceText -RequiredTokens @('#include "parse/objc3_ast_builder.h"', "Objc3ParsedProgram Objc3AstBuilder::BeginProgram() const", "void Objc3AstBuilder::SetModuleName(", "void Objc3AstBuilder::AddGlobalDecl(", "void Objc3AstBuilder::AddFunctionDecl(")) `
    -Id "contract.ast_builder_scaffold_source.implementations" `
    -FailureMessage "AST builder scaffold source missing expected method implementations" `
    -PassMessage "AST builder scaffold source implements expected method surface"

  $astBuilderContractDirectModel = Assert-TokensPresent `
    -Text $astBuilderContractHeaderText `
    -RequiredTokens @('#include "ast/objc3_ast.h"', '#include "token/objc3_token_contract.h"', "struct Objc3AstBuilderResult", "Objc3Program program;", "std::vector<std::string> diagnostics;", "BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens);")
  $astBuilderContractParserModel = Assert-TokensPresent `
    -Text $astBuilderContractHeaderText `
    -RequiredTokens @('#include "parse/objc3_parser_contract.h"', '#include "token/objc3_token_contract.h"', "struct Objc3AstBuilderResult", "Objc3ParsedProgram program;", "std::vector<std::string> diagnostics;", "BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens);")
  Assert-Contract `
    -Condition ($astBuilderContractDirectModel -or $astBuilderContractParserModel) `
    -Id "contract.ast_builder_contract_header.surface" `
    -FailureMessage "AST builder contract header missing expected surface" `
    -PassMessage "AST builder contract header exposes expected surface"

  $parserParseSignatureValid = [regex]::IsMatch($parserSourceText, 'Objc3(?:ParsedProgram|Program)\s+Parse\(\)')
  $parserEntryMarkersPresent = (
    $parserSourceText.IndexOf('#include "parse/objc3_ast_builder.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $parserSourceText.IndexOf("class Objc3Parser", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserSourceText.IndexOf("std::unique_ptr<GlobalDecl> ParseGlobalLet()", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserSourceText.IndexOf("std::unique_ptr<FunctionDecl> ParseFunction()", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserSourceText.IndexOf("std::unique_ptr<Stmt> ParseStatement()", [System.StringComparison]::Ordinal) -ge 0 -and
    $parserSourceText.IndexOf("std::unique_ptr<Expr> ParseExpression()", [System.StringComparison]::Ordinal) -ge 0 -and
    (
      $parserSourceText.IndexOf("Objc3ParseResult ParseObjc3Program(const std::vector<Objc3LexToken> &tokens)", [System.StringComparison]::Ordinal) -ge 0 -or
      $parserSourceText.IndexOf("Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens)", [System.StringComparison]::Ordinal) -ge 0
    )
  )
  Assert-Contract `
    -Condition ($parserParseSignatureValid -and $parserEntryMarkersPresent) `
    -Id "contract.parser_source.entrypoints" `
    -FailureMessage "parser source missing expected parser class/entrypoint extraction markers" `
    -PassMessage "parser source retains parser extraction entrypoint markers"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $astBuilderContractSourceText -RequiredTokens @('#include "parse/objc3_ast_builder_contract.h"', '#include "parse/objc3_parser.h"', "BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens)", "Objc3ParseResult parse_result = ParseObjc3Program(tokens);", "Objc3AstBuilderResult builder_result;")) `
    -Id "contract.ast_builder_contract_source.bridge" `
    -FailureMessage "AST builder contract source missing parser->AST builder bridge markers" `
    -PassMessage "AST builder contract source bridges parser results into AST builder surface"

  $requiredAstBuilderScaffoldTokens = @(
    "std::make_unique<GlobalDecl>()",
    "std::make_unique<FunctionDecl>()",
    "std::make_unique<Stmt>()",
    "std::make_unique<LetStmt>()",
    "std::make_unique<AssignStmt>()",
    "std::make_unique<ReturnStmt>()",
    "std::make_unique<IfStmt>()",
    "std::make_unique<DoWhileStmt>()",
    "std::make_unique<ForStmt>()",
    "std::make_unique<SwitchStmt>()",
    "std::make_unique<WhileStmt>()",
    "std::make_unique<BlockStmt>()",
    "std::make_unique<ExprStmt>()",
    "std::make_unique<Expr>()"
  )
  $missingBuilderTokens = @($requiredAstBuilderScaffoldTokens | Where-Object { $parserSourceText.IndexOf($_, [System.StringComparison]::Ordinal) -lt 0 })
  Assert-Contract `
    -Condition ($missingBuilderTokens.Count -eq 0) `
    -Id "contract.parser_ast_builder.scaffolding_tokens" `
    -FailureMessage ("parser AST builder scaffolding tokens missing: {0}" -f ($missingBuilderTokens -join ",")) `
    -PassMessage "parser AST builder scaffolding tokens are present in parser module" `
    -Evidence @{ missing = $missingBuilderTokens }

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $parserSourceText -RequiredTokens @("MakeObjc3SemaTokenMetadata(", "Objc3SemaTokenKind::PointerDeclarator", "Objc3SemaTokenKind::NullabilitySuffix")) `
    -Id "contract.parser_token_metadata_bridge" `
    -FailureMessage "parser source missing token metadata bridge markers for AST/scaffold flow" `
    -PassMessage "parser source keeps token metadata bridge markers for AST/scaffold flow"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $astHeaderText -RequiredTokens @("struct Expr", "struct Stmt", "struct FunctionDecl", "struct GlobalDecl", "struct Objc3Program", "std::unique_ptr<Expr>", "std::unique_ptr<Stmt>", "std::vector<std::unique_ptr<Stmt>>")) `
    -Id "contract.ast_header.scaffold_structures" `
    -FailureMessage "AST header missing expected parser-consumed scaffold structures" `
    -PassMessage "AST header exposes parser-consumed scaffold structures"

  $pipelineConsumesAstBuilderSurface = (
    $pipelineSourceText.IndexOf('#include "parse/objc3_ast_builder_contract.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);", [System.StringComparison]::Ordinal) -ge 0
  )
  $pipelineConsumesLegacyParserSurface = (
    $pipelineSourceText.IndexOf('#include "parse/objc3_parser.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("Objc3ParseResult parse_result = ParseObjc3Program(tokens);", [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition ($pipelineConsumesAstBuilderSurface -or $pipelineConsumesLegacyParserSurface) `
    -Id "contract.pipeline.consumes_parser_surface" `
    -FailureMessage "pipeline no longer consumes extracted parser/AST-builder API surface" `
    -PassMessage "pipeline consumes extracted parser/AST-builder API surface"

  Assert-Contract `
    -Condition ($pipelineSourceText.IndexOf("class Objc3Parser {", [System.StringComparison]::Ordinal) -lt 0) `
    -Id "contract.pipeline.no_inline_parser" `
    -FailureMessage "pipeline source contains inline parser class definition" `
    -PassMessage "pipeline source does not inline parser implementation"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $cmakeText -RequiredTokens @("add_library(objc3c_parse STATIC", "src/parse/objc3_ast_builder_contract.cpp", "src/parse/objc3_parser.cpp")) `
    -Id "contract.cmake.parser_target_registered" `
    -FailureMessage "CMake missing objc3c_parse target registration for parser + AST builder contract sources" `
    -PassMessage "CMake registers objc3c_parse target with parser + AST builder contract sources"

  Invoke-ParserExtractionAstBuilderRuntimeCases `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -BuildScriptPath $buildScriptPath `
    -NativeExePath $nativeExePath `
    -NativeExeExplicit $nativeExeExplicit `
    -PositiveFixturePath $positiveFixturePath `
    -NegativeFixturePaths $negativeFixturePaths `
    -CaseResults $caseResults
}
catch {
  $hadFatalError = $true
  $fatalErrorMessage = $_.Exception.Message
  Write-Output ("error: {0}" -f $fatalErrorMessage)
}
finally {
  Pop-Location
}

$checkArray = $checks.ToArray()
$caseResultArray = $caseResults.ToArray()
$total = $checkArray.Count
$passed = @($checkArray | Where-Object { $_.passed }).Count
$failed = $total - $passed
$status = if (-not $hadFatalError -and $total -gt 0 -and $failed -eq 0) { "PASS" } else { "FAIL" }

$nativeExeSummary = "$nativeExePath"
if (Test-Path -LiteralPath $nativeExePath -PathType Leaf) {
  try {
    $nativeExeSummary = Get-RepoRelativePath -Path $nativeExePath -Root $repoRoot
  }
  catch {
    $nativeExeSummary = "$nativeExePath"
  }
}

$summary = @{
  contract = "objc3c-parser-extraction-ast-builder-contract-v1"
  run_id = $runId
  run_dir = $runDirRel
  summary_path = $summaryRel
  native_executable = $nativeExeSummary
  status = $status
  total = $total
  passed = $passed
  failed = $failed
  fatal_error = $fatalErrorMessage
  checks = $checkArray
  cases = $caseResultArray
}
$summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output ("summary_path: {0}" -f $summaryRel)
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
