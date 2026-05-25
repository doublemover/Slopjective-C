Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking

function New-ExecutionSmokeCaseContext {
  param(
    [Parameter(Mandatory = $true)][object]$Fixture,
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][string]$Kind
  )

  $fixtureRel = Get-RepoRelativePath -Path $Fixture.FullName -Root $Context.repo_root
  $caseDirName = Get-CaseDirectoryName `
    -RunDir $Context.run_dir `
    -Kind $Kind `
    -FixtureRelativePath $fixtureRel `
    -FixtureBaseName $Fixture.BaseName
  $caseDir = Join-Path $Context.run_dir $caseDirName
  $compileDir = Join-Path $caseDir "compile"
  New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

  return [pscustomobject]@{
    fixture_rel = $fixtureRel
    case_dir = $caseDir
    compile_dir = $compileDir
    exe_path = Join-Path $caseDir "module.exe"
    compile_log = Join-Path $caseDir "compile.log"
    link_log = Join-Path $caseDir "link.log"
    run_log = Join-Path $caseDir "run.log"
  }
}

function Get-ExecutionSmokeNativeArgs {
  param(
    [Parameter(Mandatory = $true)][object]$Fixture,
    $CompileArgs,
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][string]$CompileDir
  )

  $nativeArgs = @($Fixture.FullName, "--out-dir", $CompileDir, "--emit-prefix", "module", "--llc", $Context.llc_command)
  if (@($CompileArgs).Count -gt 0) {
    $nativeArgs += @($CompileArgs)
  }
  return $nativeArgs
}

function Get-ExecutionSmokeRuntimeLibrary {
  param([Parameter(Mandatory = $true)]$LaunchContract)

  return [pscustomobject]@{
    path = $LaunchContract.runtime_library_path
    relative_path = $LaunchContract.runtime_library_relative_path
    source = $LaunchContract.runtime_library_source
  }
}

function Get-ExecutionSmokeCompileText {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$CompileLog
  )

  $compileDiagPath = Join-Path $CompileDir "module.diagnostics.txt"
  if (Test-Path -LiteralPath $compileDiagPath -PathType Leaf) {
    return Get-Content -LiteralPath $compileDiagPath -Raw
  }
  if (Test-Path -LiteralPath $CompileLog -PathType Leaf) {
    return Get-Content -LiteralPath $CompileLog -Raw
  }
  return ""
}

function Get-ExecutionSmokeLogExcerpt {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [int]$TailLines = 120
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    return "log missing: $Path"
  }
  $lines = @(Get-Content -LiteralPath $Path -Tail $TailLines)
  if ($lines.Count -eq 0) {
    return "log empty: $Path"
  }
  return "log tail: $Path`n" + ($lines -join "`n")
}

function Write-ExecutionSmokeProgressStart {
  param(
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][string]$Kind,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][ref]$LastCompletedFixture
  )

  Write-Output ("execution-smoke-progress: [{0}/{1}] START kind={2} fixture={3} elapsed={4:n3}s last={5}" -f $FixtureIndex, $TotalSelectedFixtures, $Kind, $FixtureRel, $SuiteStopwatch.Elapsed.TotalSeconds, $LastCompletedFixture.Value)
}

function Write-ExecutionSmokeProgressDone {
  param(
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][string]$Kind,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][double]$DurationSeconds,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch
  )

  Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind={2} fixture={3} duration={4:n3}s elapsed={5:n3}s" -f $FixtureIndex, $TotalSelectedFixtures, $Kind, $FixtureRel, $DurationSeconds, $SuiteStopwatch.Elapsed.TotalSeconds)
}

Export-ModuleMember -Function @(
  "Get-ExecutionSmokeCompileText",
  "Get-ExecutionSmokeLogExcerpt",
  "Get-ExecutionSmokeNativeArgs",
  "Get-ExecutionSmokeRuntimeLibrary",
  "New-ExecutionSmokeCaseContext",
  "Write-ExecutionSmokeProgressDone",
  "Write-ExecutionSmokeProgressStart"
)
