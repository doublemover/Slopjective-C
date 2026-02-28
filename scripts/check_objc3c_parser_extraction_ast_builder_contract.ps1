$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

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

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = [System.IO.Path]::GetFullPath("$Path")
  $fullRoot = [System.IO.Path]::GetFullPath("$Root")
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Read-NormalizedText {
  param([Parameter(Mandatory = $true)][string]$Path)

  $text = Get-Content -LiteralPath $Path -Raw
  $text = $text -replace "`r`n", "`n"
  $text = $text -replace "`r", "`n"
  return $text
}

function Get-FileSha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Add-Check {
  param(
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][string]$Detail,
    [Parameter()][object]$Evidence = $null
  )

  $checks.Add([pscustomobject]@{
      id = $Id
      passed = $Passed
      detail = $Detail
      evidence = $Evidence
    }) | Out-Null
}

function Assert-Contract {
  param(
    [Parameter(Mandatory = $true)][bool]$Condition,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage,
    [Parameter()][object]$Evidence = $null
  )

  if (-not $Condition) {
    Add-Check -Id $Id -Passed $false -Detail $FailureMessage -Evidence $Evidence
    throw "parser extraction contract FAIL: $FailureMessage"
  }
  Add-Check -Id $Id -Passed $true -Detail $PassMessage -Evidence $Evidence
}

function Assert-FileExists {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$Description
  )

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $repoRoot
  Assert-Contract `
    -Condition $exists `
    -Id $Id `
    -FailureMessage "missing $Description at $relative" `
    -PassMessage "found $Description at $relative" `
    -Evidence @{ path = $relative }
}

function Assert-TokensPresent {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$RequiredTokens
  )

  foreach ($token in $RequiredTokens) {
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      return $false
    }
  }
  return $true
}

function Get-ExpectedParserCodesFromFixture {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $text = Read-NormalizedText -Path $FixturePath
  $match = [regex]::Match($text, '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$')
  if (-not $match.Success) {
    throw "parser extraction FAIL: missing expected diagnostic header in $FixturePath"
  }
  $codes = [regex]::Matches($match.Groups[1].Value, 'O3[A-Z]\d{3}') | ForEach-Object { $_.Value.ToUpperInvariant() }
  $normalized = @($codes | Sort-Object -Unique)
  if ($normalized.Count -eq 0) {
    throw "parser extraction FAIL: expected diagnostic header has no parseable codes in $FixturePath"
  }
  $nonParserCodes = @($normalized | Where-Object { $_ -notmatch '^O3P\d{3}$' })
  if ($nonParserCodes.Count -gt 0) {
    throw "parser extraction FAIL: expected diagnostics for parser fixture must be O3P* only in $FixturePath (found: $($nonParserCodes -join ','))"
  }
  return $normalized
}

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

  if (-not $nativeExeExplicit -and !(Test-Path -LiteralPath $nativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $repoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $repoRoot) }
  }

  Assert-FileExists -Path $nativeExePath -Id "runtime.native_executable.exists" -Description "native executable"

  $positiveCaseDir = Join-Path $runDir "positive_smoke"
  $positiveRun1Dir = Join-Path $positiveCaseDir "run1"
  $positiveRun2Dir = Join-Path $positiveCaseDir "run2"
  New-Item -ItemType Directory -Force -Path $positiveRun1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $positiveRun2Dir | Out-Null
  $positiveRun1Log = Join-Path $positiveCaseDir "run1.log"
  $positiveRun2Log = Join-Path $positiveCaseDir "run2.log"
  $positiveArgsRun1 = @($positiveFixturePath, "--out-dir", $positiveRun1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveArgsRun2 = @($positiveFixturePath, "--out-dir", $positiveRun2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveRun1Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $positiveArgsRun1 -LogPath $positiveRun1Log
  $positiveRun2Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $positiveArgsRun2 -LogPath $positiveRun2Log

  Assert-Contract `
    -Condition ($positiveRun1Exit -eq 0 -and $positiveRun2Exit -eq 0) `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage ("positive parser scaffold fixture compile exits must be zero (run1={0} run2={1})" -f $positiveRun1Exit, $positiveRun2Exit) `
    -PassMessage "positive parser scaffold fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_log = Get-RepoRelativePath -Path $positiveRun1Log -Root $repoRoot
      run2_log = Get-RepoRelativePath -Path $positiveRun2Log -Root $repoRoot
    }

  $positiveArtifacts = @("module.manifest.json", "module.diagnostics.txt", "module.ll", "module.obj", "module.object-backend.txt")
  $positiveDigests = @{}
  foreach ($artifact in $positiveArtifacts) {
    $artifactRun1 = Join-Path $positiveRun1Dir $artifact
    $artifactRun2 = Join-Path $positiveRun2Dir $artifact
    $existsRun1 = Test-Path -LiteralPath $artifactRun1 -PathType Leaf
    $existsRun2 = Test-Path -LiteralPath $artifactRun2 -PathType Leaf
    Assert-Contract `
      -Condition ($existsRun1 -and $existsRun2) `
      -Id ("runtime.positive.artifact.exists.{0}" -f $artifact) `
      -FailureMessage ("positive parser scaffold fixture missing artifact across replay: {0}" -f $artifact) `
      -PassMessage ("positive parser scaffold artifact present across replay: {0}" -f $artifact)

    if ($artifact -eq "module.obj") {
      $objRun1Bytes = (Get-Item -LiteralPath $artifactRun1).Length
      $objRun2Bytes = (Get-Item -LiteralPath $artifactRun2).Length
      Assert-Contract `
        -Condition ($objRun1Bytes -gt 0 -and $objRun2Bytes -gt 0) `
        -Id "runtime.positive.artifact.nonempty.module.obj" `
        -FailureMessage "positive parser scaffold module.obj is empty in one or more runs" `
        -PassMessage "positive parser scaffold module.obj is non-empty across replay" `
        -Evidence @{ run1_bytes = $objRun1Bytes; run2_bytes = $objRun2Bytes }
    }

    $hashRun1 = Get-FileSha256Hex -Path $artifactRun1
    $hashRun2 = Get-FileSha256Hex -Path $artifactRun2
    $positiveDigests[$artifact] = [ordered]@{
      run1_sha256 = $hashRun1
      run2_sha256 = $hashRun2
      deterministic = ($hashRun1 -eq $hashRun2)
    }
    if ($artifact -eq "module.obj") {
      # COFF object payloads can contain non-deterministic metadata in this environment.
      Add-Check `
        -Id "runtime.positive.artifact.hash_recorded.module.obj" `
        -Passed $true `
        -Detail "positive parser scaffold module.obj hashes recorded; determinism is not enforced for this artifact" `
        -Evidence @{ run1_sha256 = $hashRun1; run2_sha256 = $hashRun2 }
      continue
    }
    Assert-Contract `
      -Condition ($hashRun1 -eq $hashRun2) `
      -Id ("runtime.positive.artifact.deterministic_sha256.{0}" -f $artifact) `
      -FailureMessage ("positive parser scaffold artifact hash drift detected for {0}" -f $artifact) `
      -PassMessage ("positive parser scaffold artifact hash stable for {0}" -f $artifact) `
      -Evidence @{ run1_sha256 = $hashRun1; run2_sha256 = $hashRun2 }
  }

  $positiveDiagnosticsText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($positiveDiagnosticsText)) `
    -Id "runtime.positive.diagnostics.empty" `
    -FailureMessage "positive parser scaffold fixture emitted diagnostics unexpectedly" `
    -PassMessage "positive parser scaffold diagnostics are empty"

  $positiveLlText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.ll")
  Assert-Contract `
    -Condition ($positiveLlText.IndexOf("define i32 @objc3c_entry", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.ll.contains_objc3c_entry" `
    -FailureMessage "positive parser scaffold LLVM IR missing objc3c_entry" `
    -PassMessage "positive parser scaffold LLVM IR contains objc3c_entry"

  $positiveManifestText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.manifest.json")
  $manifestHasParserZeroDiag = (
    $positiveManifestText.IndexOf('"parser":{"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0 -or
    $positiveManifestText.IndexOf('"parser": {"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $manifestHasParserZeroDiag `
    -Id "runtime.positive.manifest.parser_stage_zero_diag" `
    -FailureMessage "positive parser scaffold manifest missing parser stage diagnostics=0 marker" `
    -PassMessage "positive parser scaffold manifest reports parser diagnostics=0"

  Assert-Contract `
    -Condition ($positiveManifestText.IndexOf('"name":"choose"', [System.StringComparison]::Ordinal) -ge 0 -and $positiveManifestText.IndexOf('"name":"main"', [System.StringComparison]::Ordinal) -ge 0 -and $positiveManifestText.IndexOf('"name":"seed"', [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.manifest.ast_surface_names" `
    -FailureMessage "positive parser scaffold manifest missing expected function/global names from AST surface" `
    -PassMessage "positive parser scaffold manifest includes expected AST surface function/global names"

  $backendText = (Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.object-backend.txt")).Trim()
  Assert-Contract `
    -Condition ($backendText -eq "clang") `
    -Id "runtime.positive.object_backend.clang" `
    -FailureMessage ("positive parser scaffold expected object backend 'clang' but saw '{0}'" -f $backendText) `
    -PassMessage "positive parser scaffold uses explicit clang object backend"

  $caseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $positiveFixturePath -Root $repoRoot
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_dir = Get-RepoRelativePath -Path $positiveRun1Dir -Root $repoRoot
      run2_dir = Get-RepoRelativePath -Path $positiveRun2Dir -Root $repoRoot
      artifact_digests = $positiveDigests
    }) | Out-Null

  foreach ($negativeFixturePath in $negativeFixturePaths) {
    $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)
    $expectedCodes = @(Get-ExpectedParserCodesFromFixture -FixturePath $negativeFixturePath)
    $caseDir = Join-Path (Join-Path $runDir "negative_cases") $fixtureStem
    $run1Dir = Join-Path $caseDir "run1"
    $run2Dir = Join-Path $caseDir "run2"
    New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
    New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
    $run1Log = Join-Path $caseDir "run1.log"
    $run2Log = Join-Path $caseDir "run2.log"

    $argsRun1 = @($negativeFixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $argsRun2 = @($negativeFixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $run1Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $argsRun1 -LogPath $run1Log
    $run2Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $argsRun2 -LogPath $run2Log

    Assert-Contract `
      -Condition ($run1Exit -ne 0 -and $run2Exit -ne 0 -and $run1Exit -eq $run2Exit) `
      -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
      -FailureMessage ("negative parser fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
      -PassMessage ("negative parser fixture fails deterministically: {0}" -f $fixtureStem) `
      -Evidence @{
        run1_log = Get-RepoRelativePath -Path $run1Log -Root $repoRoot
        run2_log = Get-RepoRelativePath -Path $run2Log -Root $repoRoot
      }

    $diagRun1Path = Join-Path $run1Dir "module.diagnostics.txt"
    $diagRun2Path = Join-Path $run2Dir "module.diagnostics.txt"
    Assert-Contract `
      -Condition ((Test-Path -LiteralPath $diagRun1Path -PathType Leaf) -and (Test-Path -LiteralPath $diagRun2Path -PathType Leaf)) `
      -Id ("runtime.negative.diagnostics.exists.{0}" -f $fixtureStem) `
      -FailureMessage ("negative parser fixture missing diagnostics artifact(s): {0}" -f $fixtureStem) `
      -PassMessage ("negative parser fixture diagnostics artifacts present: {0}" -f $fixtureStem)

    $diagRun1Text = Read-NormalizedText -Path $diagRun1Path
    $diagRun2Text = Read-NormalizedText -Path $diagRun2Path
    $diagRun1Hash = Get-FileSha256Hex -Path $diagRun1Path
    $diagRun2Hash = Get-FileSha256Hex -Path $diagRun2Path
    Assert-Contract `
      -Condition ((-not [string]::IsNullOrWhiteSpace($diagRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagRun2Text))) `
      -Id ("runtime.negative.diagnostics.nonempty.{0}" -f $fixtureStem) `
      -FailureMessage ("negative parser fixture diagnostics are unexpectedly empty: {0}" -f $fixtureStem) `
      -PassMessage ("negative parser fixture diagnostics are populated: {0}" -f $fixtureStem)
    Assert-Contract `
      -Condition ($diagRun1Hash -eq $diagRun2Hash) `
      -Id ("runtime.negative.diagnostics.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative parser fixture diagnostics hash drift: {0}" -f $fixtureStem) `
      -PassMessage ("negative parser fixture diagnostics hash stable: {0}" -f $fixtureStem) `
      -Evidence @{ run1_sha256 = $diagRun1Hash; run2_sha256 = $diagRun2Hash }

    foreach ($expectedCode in $expectedCodes) {
      Assert-Contract `
        -Condition ($diagRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and $diagRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
        -Id ("runtime.negative.diagnostics.expected_code.{0}.{1}" -f $fixtureStem, $expectedCode) `
        -FailureMessage ("negative parser fixture diagnostics missing expected code {0}: {1}" -f $expectedCode, $fixtureStem) `
        -PassMessage ("negative parser fixture diagnostics contain expected code {0}: {1}" -f $expectedCode, $fixtureStem)
    }

    foreach ($forbiddenArtifact in @("module.manifest.json", "module.ll", "module.obj", "module.object-backend.txt")) {
      $forbiddenRun1 = Join-Path $run1Dir $forbiddenArtifact
      $forbiddenRun2 = Join-Path $run2Dir $forbiddenArtifact
      $forbiddenPresent = (Test-Path -LiteralPath $forbiddenRun1 -PathType Leaf) -or (Test-Path -LiteralPath $forbiddenRun2 -PathType Leaf)
      Assert-Contract `
        -Condition (-not $forbiddenPresent) `
        -Id ("runtime.negative.artifact.absent.{0}.{1}" -f $fixtureStem, $forbiddenArtifact) `
        -FailureMessage ("negative parser fixture produced forbidden artifact {0}: {1}" -f $forbiddenArtifact, $fixtureStem) `
        -PassMessage ("negative parser fixture keeps {0} absent: {1}" -f $forbiddenArtifact, $fixtureStem)
    }

    $caseResults.Add([pscustomobject]@{
        kind = "negative"
        fixture = Get-RepoRelativePath -Path $negativeFixturePath -Root $repoRoot
        expected_codes = $expectedCodes
        run1_exit = $run1Exit
        run2_exit = $run2Exit
        run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $repoRoot
        run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $repoRoot
        diagnostics_sha256 = $diagRun1Hash
      }) | Out-Null
  }
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
