function Invoke-SemaPassManagerNegativeBackendMatrixRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$NegativeFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $matrixNegativeFixturePath = $negativeFixturePath
  $matrixNegativeFixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($matrixNegativeFixturePath)
  $matrixNegativeExpectedCodes = @(Get-ExpectedSemaCodesFromFixture -FixturePath $matrixNegativeFixturePath)
  $matrixNegativeCaseDir = Join-Path (Join-Path $runDir "negative_backend_matrix") $matrixNegativeFixtureStem
  $matrixNegativeClangDir = Join-Path $matrixNegativeCaseDir "clang"
  $matrixNegativeLlvmDirectDir = Join-Path $matrixNegativeCaseDir "llvm_direct"
  New-Item -ItemType Directory -Force -Path $matrixNegativeClangDir | Out-Null
  New-Item -ItemType Directory -Force -Path $matrixNegativeLlvmDirectDir | Out-Null
  $matrixNegativeClangLog = Join-Path $matrixNegativeCaseDir "clang.log"
  $matrixNegativeLlvmDirectLog = Join-Path $matrixNegativeCaseDir "llvm_direct.log"
  $matrixNegativeClangArgs = @($matrixNegativeFixturePath, "--out-dir", $matrixNegativeClangDir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $matrixNegativeLlvmDirectArgs = @($matrixNegativeFixturePath, "--out-dir", $matrixNegativeLlvmDirectDir, "--emit-prefix", "module", "--objc3-ir-object-backend", "llvm-direct")
  $matrixNegativeClangExit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $matrixNegativeClangArgs -LogPath $matrixNegativeClangLog
  $matrixNegativeLlvmDirectExit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $matrixNegativeLlvmDirectArgs -LogPath $matrixNegativeLlvmDirectLog
  $matrixNegativeLlvmDirectLogText = Read-NormalizedText -Path $matrixNegativeLlvmDirectLog
  if ($null -eq $matrixNegativeLlvmDirectLogText) {
    $matrixNegativeLlvmDirectLogText = ""
  }
  $matrixNegativeUnavailableMarkers = @(
    "llvm-direct object emission failed: llc executable not found:",
    "llvm-direct object emission backend unavailable in this build",
    "llvm-direct object emission failed: llc exited with status "
  )
  $matrixNegativeUnavailable = $false
  foreach ($marker in $matrixNegativeUnavailableMarkers) {
    if ($matrixNegativeLlvmDirectLogText.IndexOf($marker, [System.StringComparison]::Ordinal) -ge 0) {
      $matrixNegativeUnavailable = $true
      break
    }
  }
  $matrixNegativeParityMode = ($matrixNegativeClangExit -ne 0 -and $matrixNegativeLlvmDirectExit -ne 0 -and $matrixNegativeClangExit -eq $matrixNegativeLlvmDirectExit)
  $matrixNegativeUnavailableMode = (
    $matrixNegativeClangExit -ne 0 -and
    $matrixNegativeLlvmDirectExit -in @(2, 3) -and
    $matrixNegativeUnavailable
  )
  $matrixNegativeExitPassMessage = if ($matrixNegativeParityMode) {
    "negative sema backend matrix replay fails identically for clang/llvm-direct: $matrixNegativeFixtureStem"
  }
  else {
    "negative sema backend matrix replay is fail-closed due to unavailable llvm-direct backend: $matrixNegativeFixtureStem"
  }

  Assert-Contract `
    -Condition ($matrixNegativeParityMode -or $matrixNegativeUnavailableMode) `
    -Id ("runtime.negative.matrix.backend.exit_codes.{0}" -f $matrixNegativeFixtureStem) `
    -FailureMessage ("negative sema backend matrix replay must either fail identically across clang/llvm-direct or fail-closed deterministically when llvm-direct backend is unavailable ({0}: clang={1} llvm-direct={2})" -f $matrixNegativeFixtureStem, $matrixNegativeClangExit, $matrixNegativeLlvmDirectExit) `
    -PassMessage $matrixNegativeExitPassMessage

  $matrixNegativeClangDiagTxtPath = Join-Path $matrixNegativeClangDir "module.diagnostics.txt"
  $matrixNegativeLlvmDirectDiagTxtPath = Join-Path $matrixNegativeLlvmDirectDir "module.diagnostics.txt"
  $matrixNegativeClangDiagJsonPath = Join-Path $matrixNegativeClangDir "module.diagnostics.json"
  $matrixNegativeLlvmDirectDiagJsonPath = Join-Path $matrixNegativeLlvmDirectDir "module.diagnostics.json"
  $matrixNegativeClangDiagTxtHash = ""
  $matrixNegativeClangDiagJsonHash = ""
  if ($matrixNegativeParityMode) {
    Assert-Contract `
      -Condition ((Test-Path -LiteralPath $matrixNegativeClangDiagTxtPath -PathType Leaf) -and
                  (Test-Path -LiteralPath $matrixNegativeLlvmDirectDiagTxtPath -PathType Leaf) -and
                  (Test-Path -LiteralPath $matrixNegativeClangDiagJsonPath -PathType Leaf) -and
                  (Test-Path -LiteralPath $matrixNegativeLlvmDirectDiagJsonPath -PathType Leaf)) `
      -Id ("runtime.negative.matrix.backend.diagnostics.exists.{0}" -f $matrixNegativeFixtureStem) `
      -FailureMessage ("negative sema backend matrix replay missing diagnostics artifacts: {0}" -f $matrixNegativeFixtureStem) `
      -PassMessage ("negative sema backend matrix replay diagnostics artifacts exist: {0}" -f $matrixNegativeFixtureStem)

    $matrixNegativeClangDiagTxtHash = Get-FileSha256Hex -Path $matrixNegativeClangDiagTxtPath
    $matrixNegativeLlvmDirectDiagTxtHash = Get-FileSha256Hex -Path $matrixNegativeLlvmDirectDiagTxtPath
    $matrixNegativeClangDiagJsonHash = Get-FileSha256Hex -Path $matrixNegativeClangDiagJsonPath
    $matrixNegativeLlvmDirectDiagJsonHash = Get-FileSha256Hex -Path $matrixNegativeLlvmDirectDiagJsonPath
    Assert-Contract `
      -Condition ($matrixNegativeClangDiagTxtHash -eq $matrixNegativeLlvmDirectDiagTxtHash) `
      -Id ("runtime.negative.matrix.backend.diagnostics_txt.deterministic_sha256.{0}" -f $matrixNegativeFixtureStem) `
      -FailureMessage ("negative sema backend matrix diagnostics text hash drift between clang/llvm-direct: {0}" -f $matrixNegativeFixtureStem) `
      -PassMessage ("negative sema backend matrix diagnostics text hash is backend-invariant: {0}" -f $matrixNegativeFixtureStem)
    Assert-Contract `
      -Condition ($matrixNegativeClangDiagJsonHash -eq $matrixNegativeLlvmDirectDiagJsonHash) `
      -Id ("runtime.negative.matrix.backend.diagnostics_json.deterministic_sha256.{0}" -f $matrixNegativeFixtureStem) `
      -FailureMessage ("negative sema backend matrix diagnostics json hash drift between clang/llvm-direct: {0}" -f $matrixNegativeFixtureStem) `
      -PassMessage ("negative sema backend matrix diagnostics json hash is backend-invariant: {0}" -f $matrixNegativeFixtureStem)

    $matrixNegativeClangDiagTxtText = Read-NormalizedText -Path $matrixNegativeClangDiagTxtPath
    $matrixNegativeLlvmDirectDiagTxtText = Read-NormalizedText -Path $matrixNegativeLlvmDirectDiagTxtPath
    $matrixNegativeClangDiagJsonText = Read-NormalizedText -Path $matrixNegativeClangDiagJsonPath
    $matrixNegativeLlvmDirectDiagJsonText = Read-NormalizedText -Path $matrixNegativeLlvmDirectDiagJsonPath
    foreach ($expectedCode in $matrixNegativeExpectedCodes) {
      Assert-Contract `
        -Condition ($matrixNegativeClangDiagTxtText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $matrixNegativeLlvmDirectDiagTxtText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $matrixNegativeClangDiagJsonText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $matrixNegativeLlvmDirectDiagJsonText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
        -Id ("runtime.negative.matrix.backend.expected_code.{0}.{1}" -f $matrixNegativeFixtureStem, $expectedCode) `
        -FailureMessage ("negative sema backend matrix diagnostics missing expected code {0}: {1}" -f $expectedCode, $matrixNegativeFixtureStem) `
        -PassMessage ("negative sema backend matrix diagnostics contain expected code {0}: {1}" -f $expectedCode, $matrixNegativeFixtureStem)
    }
  }
  else {
    Assert-Contract `
      -Condition $matrixNegativeUnavailable `
      -Id ("runtime.negative.matrix.backend.unavailable.fail_closed_marker.{0}" -f $matrixNegativeFixtureStem) `
      -FailureMessage ("negative sema backend matrix unavailable mode must include deterministic llvm-direct backend-unavailable diagnostics markers: {0}" -f $matrixNegativeFixtureStem) `
      -PassMessage ("negative sema backend matrix unavailable mode includes deterministic llvm-direct backend-unavailable diagnostics markers: {0}" -f $matrixNegativeFixtureStem)
  }

  foreach ($forbiddenArtifact in @("module.manifest.json", "module.ll", "module.obj", "module.object-backend.txt")) {
    $clangArtifactPath = Join-Path $matrixNegativeClangDir $forbiddenArtifact
    $llvmDirectArtifactPath = Join-Path $matrixNegativeLlvmDirectDir $forbiddenArtifact
    $forbiddenPresent = (Test-Path -LiteralPath $clangArtifactPath -PathType Leaf) -or (Test-Path -LiteralPath $llvmDirectArtifactPath -PathType Leaf)
    Assert-Contract `
      -Condition (-not $forbiddenPresent) `
      -Id ("runtime.negative.matrix.backend.artifact.absent.{0}.{1}" -f $matrixNegativeFixtureStem, $forbiddenArtifact) `
      -FailureMessage ("negative sema backend matrix replay produced forbidden artifact {0}: {1}" -f $forbiddenArtifact, $matrixNegativeFixtureStem) `
      -PassMessage ("negative sema backend matrix replay keeps {0} absent: {1}" -f $forbiddenArtifact, $matrixNegativeFixtureStem)
  }

  $caseResults.Add([pscustomobject]@{
      kind = "negative-matrix"
      mode = if ($matrixNegativeParityMode) { "parity" } else { "unavailable-fail-closed" }
      fixture = Get-RepoRelativePath -Path $matrixNegativeFixturePath -Root $repoRoot
      backend_clang_exit = $matrixNegativeClangExit
      backend_llvm_direct_exit = $matrixNegativeLlvmDirectExit
      clang_dir = Get-RepoRelativePath -Path $matrixNegativeClangDir -Root $repoRoot
      llvm_direct_dir = Get-RepoRelativePath -Path $matrixNegativeLlvmDirectDir -Root $repoRoot
      diagnostics_txt_sha256 = $matrixNegativeClangDiagTxtHash
      diagnostics_json_sha256 = $matrixNegativeClangDiagJsonHash
    }) | Out-Null
}

function Invoke-SemaPassManagerNegativeClangRuntimeCases {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string[]]$NegativeFixturePaths,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  foreach ($negativeFixturePath in $negativeFixturePaths) {
    $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)
    $expectedCodes = @(Get-ExpectedSemaCodesFromFixture -FixturePath $negativeFixturePath)
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
      -FailureMessage ("negative sema fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
      -PassMessage ("negative sema fixture fails deterministically: {0}" -f $fixtureStem) `
      -Evidence @{
        run1_log = Get-RepoRelativePath -Path $run1Log -Root $repoRoot
        run2_log = Get-RepoRelativePath -Path $run2Log -Root $repoRoot
      }

    $diagTxtRun1Path = Join-Path $run1Dir "module.diagnostics.txt"
    $diagTxtRun2Path = Join-Path $run2Dir "module.diagnostics.txt"
    $diagJsonRun1Path = Join-Path $run1Dir "module.diagnostics.json"
    $diagJsonRun2Path = Join-Path $run2Dir "module.diagnostics.json"
    Assert-Contract `
      -Condition ((Test-Path -LiteralPath $diagTxtRun1Path -PathType Leaf) -and (Test-Path -LiteralPath $diagTxtRun2Path -PathType Leaf) -and
                  (Test-Path -LiteralPath $diagJsonRun1Path -PathType Leaf) -and (Test-Path -LiteralPath $diagJsonRun2Path -PathType Leaf)) `
      -Id ("runtime.negative.diagnostics.exists.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture missing diagnostics artifact(s): {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics artifacts present: {0}" -f $fixtureStem)

    $diagTxtRun1Text = Read-NormalizedText -Path $diagTxtRun1Path
    $diagTxtRun2Text = Read-NormalizedText -Path $diagTxtRun2Path
    $diagJsonRun1Text = Read-NormalizedText -Path $diagJsonRun1Path
    $diagJsonRun2Text = Read-NormalizedText -Path $diagJsonRun2Path
    $diagTxtRun1Hash = Get-FileSha256Hex -Path $diagTxtRun1Path
    $diagTxtRun2Hash = Get-FileSha256Hex -Path $diagTxtRun2Path
    $diagJsonRun1Hash = Get-FileSha256Hex -Path $diagJsonRun1Path
    $diagJsonRun2Hash = Get-FileSha256Hex -Path $diagJsonRun2Path
    Assert-Contract `
      -Condition ((-not [string]::IsNullOrWhiteSpace($diagTxtRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagTxtRun2Text)) -and
                  (-not [string]::IsNullOrWhiteSpace($diagJsonRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagJsonRun2Text))) `
      -Id ("runtime.negative.diagnostics.nonempty.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture diagnostics are unexpectedly empty: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics are populated: {0}" -f $fixtureStem)
    Assert-Contract `
      -Condition ($diagTxtRun1Hash -eq $diagTxtRun2Hash) `
      -Id ("runtime.negative.diagnostics_txt.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture diagnostics text hash drift: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics text hash stable: {0}" -f $fixtureStem) `
      -Evidence @{ run1_sha256 = $diagTxtRun1Hash; run2_sha256 = $diagTxtRun2Hash }
    Assert-Contract `
      -Condition ($diagJsonRun1Hash -eq $diagJsonRun2Hash) `
      -Id ("runtime.negative.diagnostics_json.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture diagnostics json hash drift: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics json hash stable: {0}" -f $fixtureStem) `
      -Evidence @{ run1_sha256 = $diagJsonRun1Hash; run2_sha256 = $diagJsonRun2Hash }

    foreach ($expectedCode in $expectedCodes) {
      Assert-Contract `
        -Condition ($diagTxtRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $diagTxtRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $diagJsonRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $diagJsonRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
        -Id ("runtime.negative.diagnostics.expected_code.{0}.{1}" -f $fixtureStem, $expectedCode) `
        -FailureMessage ("negative sema fixture diagnostics missing expected code {0}: {1}" -f $expectedCode, $fixtureStem) `
        -PassMessage ("negative sema fixture diagnostics contain expected code {0}: {1}" -f $expectedCode, $fixtureStem)
    }

    foreach ($forbiddenArtifact in @("module.manifest.json", "module.ll", "module.obj", "module.object-backend.txt")) {
      $forbiddenRun1 = Join-Path $run1Dir $forbiddenArtifact
      $forbiddenRun2 = Join-Path $run2Dir $forbiddenArtifact
      $forbiddenPresent = (Test-Path -LiteralPath $forbiddenRun1 -PathType Leaf) -or (Test-Path -LiteralPath $forbiddenRun2 -PathType Leaf)
      Assert-Contract `
        -Condition (-not $forbiddenPresent) `
        -Id ("runtime.negative.artifact.absent.{0}.{1}" -f $fixtureStem, $forbiddenArtifact) `
        -FailureMessage ("negative sema fixture produced forbidden artifact {0}: {1}" -f $forbiddenArtifact, $fixtureStem) `
        -PassMessage ("negative sema fixture keeps {0} absent: {1}" -f $forbiddenArtifact, $fixtureStem)
    }

    $caseResults.Add([pscustomobject]@{
        kind = "negative"
        backend = "clang"
        fixture = Get-RepoRelativePath -Path $negativeFixturePath -Root $repoRoot
        expected_codes = $expectedCodes
        run1_exit = $run1Exit
        run2_exit = $run2Exit
        run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $repoRoot
        run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $repoRoot
        diagnostics_txt_sha256 = $diagTxtRun1Hash
        diagnostics_json_sha256 = $diagJsonRun1Hash
      }) | Out-Null
  }
}
