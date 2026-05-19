Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_measurements.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_construction.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativePerfDirectCompileSet {
  param(
    [object]$Config,
    [string]$CompilerExe,
    [ref]$ResolvedMaxElapsedMs,
    [ref]$FixtureSets,
    [ref]$DispatchFixtureCount,
    [ref]$Results,
    [ref]$Fixtures,
    [ref]$DispatchFixturePathSet
  )

  $resolvedMaxElapsedMsValue = [int]$ResolvedMaxElapsedMs.Value
  $benchmarkCatalog = Resolve-Objc3cNativePerfBenchmarkCatalog `
    -Config $Config `
    -ResolvedMaxElapsedMs ([ref]$resolvedMaxElapsedMsValue)
  $ResolvedMaxElapsedMs.Value = $resolvedMaxElapsedMsValue
  $catalogFixtures = @(
    $benchmarkCatalog.fixtures |
      ForEach-Object {
        if ($null -eq $_) {
          return
        }
        if ($_.PSObject.Properties.Name -contains "FullName") {
          $_
        } else {
          $fixturePath = [string]$_
          [pscustomobject]@{
            FullName = $fixturePath
            Name = [System.IO.Path]::GetFileName($fixturePath)
            Extension = [System.IO.Path]::GetExtension($fixturePath)
          }
        }
      }
  )
  $catalogDispatchFixturePathSet = $benchmarkCatalog.dispatch_fixture_path_set
  $FixtureSets.Value = @($benchmarkCatalog.fixture_sets)
  $DispatchFixtureCount.Value = [int]$benchmarkCatalog.dispatch_fixture_count

  foreach ($fixtureSet in $FixtureSets.Value) {
    Write-Objc3cNativePerfFixtureSetLine -FixtureSet $fixtureSet
  }

  $processedFixtures = @()
  foreach ($fixture in $catalogFixtures) {
    if ($fixture.GetType().FullName -like "System.Management.Automation.PSReference*") {
      continue
    }
    $fixturePath = if ($fixture.PSObject.Properties.Name -contains "FullName") {
      [string]$fixture.PSObject.Properties["FullName"].Value
    } else {
      [string]$fixture
    }
    if ([string]::IsNullOrWhiteSpace($fixturePath) -or
        !(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
      continue
    }
    $fixtureExtension = if ($fixture.PSObject.Properties.Name -contains "Extension") {
      [string]$fixture.PSObject.Properties["Extension"].Value
    } else {
      [System.IO.Path]::GetExtension($fixturePath)
    }
    $fixtureRecord = [pscustomobject]@{
      FullName = $fixturePath
      Name = [System.IO.Path]::GetFileName($fixturePath)
      Extension = $fixtureExtension
    }
    $processedFixtures += $fixtureRecord
    $fixtureRel = Get-RepoRelativePath -Path $fixturePath -Root $Config.repo_root
    $hash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $Config.run_dir ("fixture_{0}" -f $hash)
    $compileLog = Join-Path $caseDir "compile.log"
    New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

    $compileArgs = New-Objc3cNativePerfDirectCompileArguments -Fixture $fixtureRecord -CaseDir $caseDir
    $run = Invoke-Objc3cNativePerfNativeProcess `
      -Command $CompilerExe `
      -Arguments $compileArgs `
      -LogPath $compileLog

    $compileResult = New-Objc3cNativePerfCompileResult -Config $Config -Fixture $fixtureRecord -CaseDir $caseDir -Run $run
    $Results.Value += $compileResult.result
    Write-Objc3cNativePerfCompileResultLine -CompileResult $compileResult
  }

  $Fixtures.Value = $processedFixtures
  $DispatchFixturePathSet.Value = $catalogDispatchFixturePathSet
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfDirectCompileSet"
)
