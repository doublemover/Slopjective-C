function Invoke-SemaPassManagerNegativeBackendMatrixRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$NegativeFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($NegativeFixturePath)
  $expectedCodes = @(Get-ExpectedSemaCodesFromFixture -FixturePath $NegativeFixturePath)
  $caseDir = Join-Path (Join-Path $RunDir "negative_backend_matrix") $fixtureStem
  $clangDir = Join-Path $caseDir "clang"
  $llvmDirectDir = Join-Path $caseDir "llvm_direct"
  New-Item -ItemType Directory -Force -Path $clangDir | Out-Null
  New-Item -ItemType Directory -Force -Path $llvmDirectDir | Out-Null

  $clangLog = Join-Path $caseDir "clang.log"
  $llvmDirectLog = Join-Path $caseDir "llvm_direct.log"
  $clangArgs = @($NegativeFixturePath, "--out-dir", $clangDir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $llvmDirectArgs = @($NegativeFixturePath, "--out-dir", $llvmDirectDir, "--emit-prefix", "module", "--objc3-ir-object-backend", "llvm-direct")
  $clangExit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $clangArgs -LogPath $clangLog
  $llvmDirectExit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $llvmDirectArgs -LogPath $llvmDirectLog

  $llvmDirectLogText = Read-NormalizedText -Path $llvmDirectLog
  if ($null -eq $llvmDirectLogText) {
    $llvmDirectLogText = ""
  }

  $llvmDirectUnavailable = Test-SemaPassManagerNegativeRuntimeUnavailableLog -LogText $llvmDirectLogText
  $parityMode = ($clangExit -ne 0 -and $llvmDirectExit -ne 0 -and $clangExit -eq $llvmDirectExit)
  $unavailableMode = (
    $clangExit -ne 0 -and
    $llvmDirectExit -in @(2, 3) -and
    $llvmDirectUnavailable
  )
  $exitPassMessage = if ($parityMode) {
    "negative sema backend matrix replay fails identically for clang/llvm-direct: $fixtureStem"
  }
  else {
    "negative sema backend matrix replay is fail-closed due to unavailable llvm-direct backend: $fixtureStem"
  }

  Assert-Contract `
    -Condition ($parityMode -or $unavailableMode) `
    -Id ("runtime.negative.matrix.backend.exit_codes.{0}" -f $fixtureStem) `
    -FailureMessage ("negative sema backend matrix replay must either fail identically across clang/llvm-direct or fail-closed deterministically when llvm-direct backend is unavailable ({0}: clang={1} llvm-direct={2})" -f $fixtureStem, $clangExit, $llvmDirectExit) `
    -PassMessage $exitPassMessage

  $clangDiagnostics = Get-SemaPassManagerNegativeRuntimeDiagnosticsPaths -OutputDir $clangDir
  $llvmDirectDiagnostics = Get-SemaPassManagerNegativeRuntimeDiagnosticsPaths -OutputDir $llvmDirectDir
  $clangDiagTxtHash = ""
  $clangDiagJsonHash = ""

  if ($parityMode) {
    Assert-SemaPassManagerNegativeRuntimeDiagnosticsExist `
      -FixtureStem $fixtureStem `
      -ContractId "runtime.negative.matrix.backend.diagnostics.exists.{0}" `
      -FailureMessage "negative sema backend matrix replay missing diagnostics artifacts: {0}" `
      -PassMessage "negative sema backend matrix replay diagnostics artifacts exist: {0}" `
      -LeftDiagnostics $clangDiagnostics `
      -RightDiagnostics $llvmDirectDiagnostics

    $clangDiagTxtHash = Get-FileSha256Hex -Path $clangDiagnostics.Txt
    $llvmDirectDiagTxtHash = Get-FileSha256Hex -Path $llvmDirectDiagnostics.Txt
    $clangDiagJsonHash = Get-FileSha256Hex -Path $clangDiagnostics.Json
    $llvmDirectDiagJsonHash = Get-FileSha256Hex -Path $llvmDirectDiagnostics.Json
    Assert-Contract `
      -Condition ($clangDiagTxtHash -eq $llvmDirectDiagTxtHash) `
      -Id ("runtime.negative.matrix.backend.diagnostics_txt.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema backend matrix diagnostics text hash drift between clang/llvm-direct: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema backend matrix diagnostics text hash is backend-invariant: {0}" -f $fixtureStem)
    Assert-Contract `
      -Condition ($clangDiagJsonHash -eq $llvmDirectDiagJsonHash) `
      -Id ("runtime.negative.matrix.backend.diagnostics_json.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema backend matrix diagnostics json hash drift between clang/llvm-direct: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema backend matrix diagnostics json hash is backend-invariant: {0}" -f $fixtureStem)

    Assert-SemaPassManagerNegativeRuntimeExpectedCodesPresent `
      -FixtureStem $fixtureStem `
      -ExpectedCodes $expectedCodes `
      -ContractId "runtime.negative.matrix.backend.expected_code.{0}.{1}" `
      -FailureMessage "negative sema backend matrix diagnostics missing expected code {0}: {1}" `
      -PassMessage "negative sema backend matrix diagnostics contain expected code {0}: {1}" `
      -LeftTxtText (Read-NormalizedText -Path $clangDiagnostics.Txt) `
      -RightTxtText (Read-NormalizedText -Path $llvmDirectDiagnostics.Txt) `
      -LeftJsonText (Read-NormalizedText -Path $clangDiagnostics.Json) `
      -RightJsonText (Read-NormalizedText -Path $llvmDirectDiagnostics.Json)
  }
  else {
    Assert-Contract `
      -Condition $llvmDirectUnavailable `
      -Id ("runtime.negative.matrix.backend.unavailable.fail_closed_marker.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema backend matrix unavailable mode must include deterministic llvm-direct backend-unavailable diagnostics markers: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema backend matrix unavailable mode includes deterministic llvm-direct backend-unavailable diagnostics markers: {0}" -f $fixtureStem)
  }

  Assert-SemaPassManagerNegativeRuntimeForbiddenArtifactsAbsent `
    -FixtureStem $fixtureStem `
    -ContractId "runtime.negative.matrix.backend.artifact.absent.{0}.{1}" `
    -FailureMessage "negative sema backend matrix replay produced forbidden artifact {0}: {1}" `
    -PassMessage "negative sema backend matrix replay keeps {0} absent: {1}" `
    -OutputDirs @($clangDir, $llvmDirectDir)

  $CaseResults.Add([pscustomobject]@{
      kind = "negative-matrix"
      mode = if ($parityMode) { "parity" } else { "unavailable-fail-closed" }
      fixture = Get-RepoRelativePath -Path $NegativeFixturePath -Root $RepoRoot
      backend_clang_exit = $clangExit
      backend_llvm_direct_exit = $llvmDirectExit
      clang_dir = Get-RepoRelativePath -Path $clangDir -Root $RepoRoot
      llvm_direct_dir = Get-RepoRelativePath -Path $llvmDirectDir -Root $RepoRoot
      diagnostics_txt_sha256 = $clangDiagTxtHash
      diagnostics_json_sha256 = $clangDiagJsonHash
    }) | Out-Null
}
