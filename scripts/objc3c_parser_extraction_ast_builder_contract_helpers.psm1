Set-StrictMode -Version Latest

$script:ParserAstBuilderContractChecks = $null
$script:ParserAstBuilderContractRepoRoot = $null

function Set-ParserExtractionAstBuilderContractContext {
  param(
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $script:ParserAstBuilderContractChecks = $Checks
  $script:ParserAstBuilderContractRepoRoot = $RepoRoot
}

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

  if ($null -eq $script:ParserAstBuilderContractChecks) {
    throw "parser extraction contract FAIL: check sink has not been initialized"
  }

  $script:ParserAstBuilderContractChecks.Add([pscustomobject]@{
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

  if ([string]::IsNullOrWhiteSpace($script:ParserAstBuilderContractRepoRoot)) {
    throw "parser extraction contract FAIL: repo root has not been initialized"
  }

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $script:ParserAstBuilderContractRepoRoot
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

function Invoke-ParserExtractionAstBuilderRuntimeCases {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)][string[]]$NegativeFixturePaths,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  if (-not $NativeExeExplicit -and !(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot) }
  }

  Assert-FileExists -Path $NativeExePath -Id "runtime.native_executable.exists" -Description "native executable"

  $positiveCaseDir = Join-Path $RunDir "positive_smoke"
  $positiveRun1Dir = Join-Path $positiveCaseDir "run1"
  $positiveRun2Dir = Join-Path $positiveCaseDir "run2"
  New-Item -ItemType Directory -Force -Path $positiveRun1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $positiveRun2Dir | Out-Null
  $positiveRun1Log = Join-Path $positiveCaseDir "run1.log"
  $positiveRun2Log = Join-Path $positiveCaseDir "run2.log"
  $positiveArgsRun1 = @($PositiveFixturePath, "--out-dir", $positiveRun1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveArgsRun2 = @($PositiveFixturePath, "--out-dir", $positiveRun2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveRun1Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $positiveArgsRun1 -LogPath $positiveRun1Log
  $positiveRun2Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $positiveArgsRun2 -LogPath $positiveRun2Log

  Assert-Contract `
    -Condition ($positiveRun1Exit -eq 0 -and $positiveRun2Exit -eq 0) `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage ("positive parser scaffold fixture compile exits must be zero (run1={0} run2={1})" -f $positiveRun1Exit, $positiveRun2Exit) `
    -PassMessage "positive parser scaffold fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_log = Get-RepoRelativePath -Path $positiveRun1Log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $positiveRun2Log -Root $RepoRoot
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

  $CaseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $PositiveFixturePath -Root $RepoRoot
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_dir = Get-RepoRelativePath -Path $positiveRun1Dir -Root $RepoRoot
      run2_dir = Get-RepoRelativePath -Path $positiveRun2Dir -Root $RepoRoot
      artifact_digests = $positiveDigests
    }) | Out-Null

  foreach ($negativeFixturePath in $NegativeFixturePaths) {
    $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)
    $expectedCodes = @(Get-ExpectedParserCodesFromFixture -FixturePath $negativeFixturePath)
    $caseDir = Join-Path (Join-Path $RunDir "negative_cases") $fixtureStem
    $run1Dir = Join-Path $caseDir "run1"
    $run2Dir = Join-Path $caseDir "run2"
    New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
    New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
    $run1Log = Join-Path $caseDir "run1.log"
    $run2Log = Join-Path $caseDir "run2.log"

    $argsRun1 = @($negativeFixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $argsRun2 = @($negativeFixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $run1Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun1 -LogPath $run1Log
    $run2Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun2 -LogPath $run2Log

    Assert-Contract `
      -Condition ($run1Exit -ne 0 -and $run2Exit -ne 0 -and $run1Exit -eq $run2Exit) `
      -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
      -FailureMessage ("negative parser fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
      -PassMessage ("negative parser fixture fails deterministically: {0}" -f $fixtureStem) `
      -Evidence @{
        run1_log = Get-RepoRelativePath -Path $run1Log -Root $RepoRoot
        run2_log = Get-RepoRelativePath -Path $run2Log -Root $RepoRoot
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

    $CaseResults.Add([pscustomobject]@{
        kind = "negative"
        fixture = Get-RepoRelativePath -Path $negativeFixturePath -Root $RepoRoot
        expected_codes = $expectedCodes
        run1_exit = $run1Exit
        run2_exit = $run2Exit
        run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $RepoRoot
        run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $RepoRoot
        diagnostics_sha256 = $diagRun1Hash
      }) | Out-Null
  }
}

Export-ModuleMember -Function @(
  "Add-Check",
  "Assert-Contract",
  "Assert-FileExists",
  "Assert-TokensPresent",
  "Get-ExpectedParserCodesFromFixture",
  "Get-FileSha256Hex",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Invoke-ParserExtractionAstBuilderRuntimeCases",
  "Read-NormalizedText",
  "Set-ParserExtractionAstBuilderContractContext"
)
