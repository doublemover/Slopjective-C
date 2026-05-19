function Invoke-Objc3cTypedAbiReplayCase {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][pscustomobject]$Context
  )

  $fixturePath = Join-Path $Context.RepoRoot $FixtureRel
  if (!(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
    throw "typed-abi replay FAIL: missing fixture $FixtureRel"
  }

  $expectPath = [System.IO.Path]::ChangeExtension($fixturePath, ".objc3-ir.expect.txt")
  $tokens = Get-Objc3cTypedAbiExpectationTokens -ExpectationPath $expectPath
  $slug = "$(Get-Objc3cTypedAbiShortHash -Value $FixtureRel)_$([System.IO.Path]::GetFileNameWithoutExtension($fixturePath))"
  $caseDir = Join-Path $Context.RunDir $slug
  $run1Dir = Join-Path $caseDir "run1"
  $run2Dir = Join-Path $caseDir "run2"
  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null

  $run1Exit = Invoke-Objc3cTypedAbiLoggedCommand `
    -Command $Context.ExecutablePath `
    -Arguments @($fixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module") `
    -LogPath (Join-Path $caseDir "run1.log")
  $run2Exit = Invoke-Objc3cTypedAbiLoggedCommand `
    -Command $Context.ExecutablePath `
    -Arguments @($fixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module") `
    -LogPath (Join-Path $caseDir "run2.log")
  if ($run1Exit -ne 0 -or $run2Exit -ne 0) {
    throw "typed-abi replay FAIL: compile failed for $FixtureRel (run1=$run1Exit run2=$run2Exit)"
  }

  $run1LlPath = Join-Path $run1Dir "module.ll"
  $run2LlPath = Join-Path $run2Dir "module.ll"
  if (!(Test-Path -LiteralPath $run1LlPath -PathType Leaf) -or !(Test-Path -LiteralPath $run2LlPath -PathType Leaf)) {
    throw "typed-abi replay FAIL: missing module.ll for $FixtureRel"
  }
  $run1Ll = Get-Content -LiteralPath $run1LlPath -Raw
  $run2Ll = Get-Content -LiteralPath $run2LlPath -Raw
  if ($run1Ll -ne $run2Ll) {
    throw "typed-abi replay FAIL: LLVM IR drift across replay for $FixtureRel"
  }

  $missingRun1 = @(Get-Objc3cTypedAbiMissingTokens -Text $run1Ll -Tokens $tokens)
  $missingRun2 = @(Get-Objc3cTypedAbiMissingTokens -Text $run2Ll -Tokens $tokens)
  if ($missingRun1.Count -gt 0 -or $missingRun2.Count -gt 0) {
    throw "typed-abi replay FAIL: expectation token mismatch for $FixtureRel (run1_missing=$($missingRun1 -join '|') run2_missing=$($missingRun2 -join '|'))"
  }

  return [pscustomobject]@{
    fixture = $FixtureRel
    expectation = Get-Objc3cTypedAbiRepoRelativePath -Path $expectPath -Root $Context.RepoRoot
    token_count = $tokens.Count
    replay_ir_deterministic = $true
    expectations_match = $true
  }
}
