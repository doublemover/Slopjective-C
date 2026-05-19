function New-Objc3cTypedAbiReplayContext {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunId
  )

  $suiteRoot = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/typed-abi-replay-proof"
  $runDir = Join-Path $suiteRoot $RunId

  return [pscustomobject]@{
    RepoRoot = $RepoRoot
    SuiteRoot = $suiteRoot
    RunId = $RunId
    RunDir = $runDir
    SummaryPath = Join-Path $runDir "summary.json"
    ExecutablePath = Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe"
    BuildScriptPath = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"
    PositiveFixtureDir = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery/positive"
  }
}

function Ensure-Objc3cTypedAbiReplayNativeExecutable {
  param([Parameter(Mandatory = $true)][pscustomobject]$Context)

  if (!(Test-Path -LiteralPath $Context.ExecutablePath -PathType Leaf)) {
    & $Context.BuildScriptPath
    if ($LASTEXITCODE -ne 0) {
      throw "typed-abi replay FAIL: build failed with exit $LASTEXITCODE"
    }
  }
}

function Get-Objc3cTypedAbiReplayFixtureRoots {
  param(
    [Parameter(Mandatory = $true)][string]$PositiveFixtureDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  if (!(Test-Path -LiteralPath $PositiveFixtureDir -PathType Container)) {
    throw "typed-abi replay FAIL: missing positive fixture directory $PositiveFixtureDir"
  }

  $expectationFiles = @(
    Get-ChildItem -LiteralPath $PositiveFixtureDir -Recurse -File -Filter "*.objc3-ir.expect.txt" |
      Sort-Object -Property FullName
  )
  if ($expectationFiles.Count -eq 0) {
    $fixtureDirRel = Get-Objc3cTypedAbiRepoRelativePath -Path $PositiveFixtureDir -Root $RepoRoot
    throw "typed-abi replay FAIL: no .objc3-ir.expect.txt files found under $fixtureDirRel"
  }

  $fixtureRoots = @()
  foreach ($expectationFile in $expectationFiles) {
    $fixturePath = Get-Objc3cTypedAbiFixturePathFromExpectation -ExpectationPath $expectationFile.FullName
    if (!(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
      $expectationRel = Get-Objc3cTypedAbiRepoRelativePath -Path $expectationFile.FullName -Root $RepoRoot
      throw "typed-abi replay FAIL: missing fixture for expectation $expectationRel"
    }
    $fixtureRoots += (Get-Objc3cTypedAbiRepoRelativePath -Path $fixturePath -Root $RepoRoot)
  }
  return @($fixtureRoots | Sort-Object -Unique)
}
