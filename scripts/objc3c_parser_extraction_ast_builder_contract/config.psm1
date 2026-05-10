function New-ParserExtractionAstBuilderContractConfig {
  $repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/parser-extraction-ast-builder-contract"
  $configuredRunId = $env:OBJC3C_PARSER_AST_CONTRACT_RUN_ID
  $runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
  $runDir = Join-Path $suiteRoot $runId
  $runDirRel = "tmp/artifacts/objc3c-native/parser-extraction-ast-builder-contract/$runId"

  $defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
  $nativeExePath = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExePath } else { $configuredNativeExe }

  return [pscustomobject]@{
    repoRoot = $repoRoot
    suiteRoot = $suiteRoot
    runId = $runId
    runDir = $runDir
    summaryPath = (Join-Path $runDir "summary.json")
    runDirRel = $runDirRel
    summaryRel = "$runDirRel/summary.json"
    buildScriptPath = (Join-Path $repoRoot "scripts/build_objc3c_native.ps1")
    nativeExePath = $nativeExePath
    nativeExeExplicit = (-not [string]::IsNullOrWhiteSpace($configuredNativeExe))
    parserHeaderPath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_parser.h")
    parserSourcePath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_parser.cpp")
    parserContractHeaderPath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_parser_contract.h")
    astBuilderScaffoldHeaderPath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder.h")
    astBuilderScaffoldSourcePath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder.cpp")
    astBuilderContractHeaderPath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder_contract.h")
    astBuilderContractSourcePath = (Join-Path $repoRoot "native/objc3c/src/parse/objc3_ast_builder_contract.cpp")
    astHeaderPath = (Join-Path $repoRoot "native/objc3c/src/ast/objc3_ast.h")
    pipelineSourcePath = (Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp")
    cmakePath = (Join-Path $repoRoot "native/objc3c/CMakeLists.txt")
    positiveFixturePath = (Join-Path $repoRoot "tests/tooling/fixtures/native/parser_split/positive_ast_builder_scaffold_smoke.objc3")
    negativeFixturePaths = @(
      (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_conditional_parser_missing_colon.objc3"),
      (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_return_parser_missing_semicolon.objc3"),
      (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_loop_control_parser_missing_while_paren.objc3"),
      (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_message_unterminated.objc3")
    )
  }
}
