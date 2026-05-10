$ErrorActionPreference = "Stop"

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_recovery_contract_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "context.psm1") -Force -DisableNameChecking

function New-RecoveryFixtureEntries {
  param(
    [object[]]$PositiveFixtures,
    [object[]]$NegativeFixtures,
    [string]$RepoRoot
  )

  return @(
    $PositiveFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "positive"
        file = $_
        relative_path = [System.IO.Path]::GetRelativePath($RepoRoot, $_.FullName).Replace("\", "/")
      }
    }
  ) + @(
    $NegativeFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "negative"
        file = $_
        relative_path = [System.IO.Path]::GetRelativePath($RepoRoot, $_.FullName).Replace("\", "/")
      }
    }
  )
}

function Get-NegativeFixtureDiagnosticTokenContracts {
  return @{
    "negative_pure_definition_impure_global_write.objc3" = @(
      "error:7:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: global-write",
      "cause-site:8:3",
      "detail:global-write@8:3"
    )
    "negative_pure_definition_impure_transitive_call.objc3" = @(
      "error:12:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: impure-callee:bump",
      "cause-site:13:10",
      "detail:global-write@8:3"
    )
    "negative_pure_definition_impure_unannotated_extern_call.objc3" = @(
      "error:7:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: unannotated-extern-call:ext_impure",
      "cause-site:8:10",
      "detail:unannotated-extern-call:ext_impure@8:10"
    )
    "negative_pure_definition_impure_message_send.objc3" = @(
      "error:5:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: message-send",
      "cause-site:6:10",
      "detail:message-send@6:10"
    )
  }
}

function Invoke-PositiveRecoveryFixtures {
  param(
    [object[]]$Fixtures,
    [string]$OutDir
  )

  foreach ($fixture in $Fixtures) {
    $source = $fixture.FullName
    $caseName = Get-FixtureCaseName -Prefix "recovery_positive" -FixturePath $source
    $caseOutDir = Join-Path $OutDir $caseName
    New-Item -ItemType Directory -Force -Path $caseOutDir | Out-Null

    $positiveExit = Invoke-Objc3cNativeWithRecovery -Arguments @($source, "--out-dir", $caseOutDir, "--emit-prefix", "module")
    if ($positiveExit -ne 0) {
      throw "contract FAIL: positive fixture compile failed for $source with exit $positiveExit"
    }

    $objPath = Join-Path $caseOutDir "module.obj"
    if (!(Test-Path -LiteralPath $objPath -PathType Leaf)) { throw "contract FAIL: missing object artifact for positive fixture $source" }
    $objSize = (Get-Item -LiteralPath $objPath).Length
    if ($objSize -le 0) { throw "contract FAIL: empty object artifact for positive fixture $source" }

    $manifestPath = Join-Path $caseOutDir "module.manifest.json"
    if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) { throw "contract FAIL: missing manifest artifact for positive fixture $source" }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw
    if ([string]::IsNullOrWhiteSpace($manifest)) {
      throw "contract FAIL: empty manifest artifact for positive fixture $source"
    }
    Assert-Objc3ManifestPipelineSurface -ManifestText $manifest -CaseName $source
    Write-Output "$caseName`_compiled=true"
    Write-Output "$caseName`_object_size=$objSize"
  }
}

function Invoke-NegativeRecoveryFixtures {
  param(
    [object[]]$Fixtures,
    [string]$OutDir
  )

  $negativeFixtureDiagnosticTokenContracts = Get-NegativeFixtureDiagnosticTokenContracts
  foreach ($fixture in $Fixtures) {
    $source = $fixture.FullName
    $caseName = Get-FixtureCaseName -Prefix "recovery_negative" -FixturePath $source
    $run1 = Join-Path $OutDir ($caseName + "_run1")
    $run2 = Join-Path $OutDir ($caseName + "_run2")
    New-Item -ItemType Directory -Force -Path $run1 | Out-Null
    New-Item -ItemType Directory -Force -Path $run2 | Out-Null

    $exit1 = Invoke-Objc3cNativeWithRecovery -Arguments @($source, "--out-dir", $run1, "--emit-prefix", "module")
    if ($exit1 -eq 0) {
      throw "contract FAIL: negative fixture unexpectedly compiled for $source"
    }
    $diagPath1 = Join-Path $run1 "module.diagnostics.txt"
    if (!(Test-Path -LiteralPath $diagPath1 -PathType Leaf)) { throw "contract FAIL: missing diagnostics artifact for negative fixture $source run1" }
    $diag1 = Get-Content -LiteralPath $diagPath1 -Raw
    if ([string]::IsNullOrWhiteSpace($diag1)) {
      throw "contract FAIL: empty diagnostics artifact for negative fixture $source run1"
    }

    $exit2 = Invoke-Objc3cNativeWithRecovery -Arguments @($source, "--out-dir", $run2, "--emit-prefix", "module")
    if ($exit2 -eq 0) {
      throw "contract FAIL: negative fixture unexpectedly compiled for $source on replay"
    }
    $diagPath2 = Join-Path $run2 "module.diagnostics.txt"
    if (!(Test-Path -LiteralPath $diagPath2 -PathType Leaf)) { throw "contract FAIL: missing diagnostics artifact for negative fixture $source run2" }
    $diag2 = Get-Content -LiteralPath $diagPath2 -Raw
    if ([string]::IsNullOrWhiteSpace($diag2)) {
      throw "contract FAIL: empty diagnostics artifact for negative fixture $source run2"
    }

    if ($exit1 -ne $exit2) {
      throw "contract FAIL: negative fixture exit-code drift across replay for $source ($exit1 vs $exit2)"
    }
    if ($diag1 -ne $diag2) {
      throw "contract FAIL: negative fixture diagnostics drift across replay for $source"
    }

    $fixtureLeaf = [System.IO.Path]::GetFileName($source)
    if ($negativeFixtureDiagnosticTokenContracts.ContainsKey($fixtureLeaf)) {
      $requiredTokens = @($negativeFixtureDiagnosticTokenContracts[$fixtureLeaf])
      foreach ($token in $requiredTokens) {
        if ($diag1.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
          throw "contract FAIL: missing negative fixture diagnostic token '$token' for $source run1"
        }
        if ($diag2.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
          throw "contract FAIL: missing negative fixture diagnostic token '$token' for $source run2"
        }
      }
      Write-Output "$caseName`_diagnostic_tokens_verified=true"
    }

    Write-Output "$caseName`_fails=true"
    Write-Output "$caseName`_exit_code=$exit1"
    Write-Output "$caseName`_deterministic_diagnostics=true"
  }
}

function Invoke-SelectedRecoveryFixtureContracts {
  param(
    [string]$RepoRoot,
    [string]$OutDir,
    [string]$PositiveFixtureDir,
    [string]$NegativeFixtureDir,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  $positiveFixtures = Get-RecoveryFixtures -Directory $PositiveFixtureDir -FixtureKind "positive native recovery" -Extensions @(".objc3")
  Assert-RecoveryFixtureClass -Fixtures $positiveFixtures -FixtureKind "positive native recovery"
  $negativeFixtures = Get-RecoveryFixtures -Directory $NegativeFixtureDir -FixtureKind "negative native recovery" -Extensions @(".objc3")
  Assert-RecoveryFixtureClass -Fixtures $negativeFixtures -FixtureKind "negative native recovery"

  $recoveryFixtureEntries = New-RecoveryFixtureEntries `
    -PositiveFixtures $positiveFixtures `
    -NegativeFixtures $negativeFixtures `
    -RepoRoot $RepoRoot
  $selectedRecoveryEntries = Select-RecoveryFixtureEntries `
    -Entries $recoveryFixtureEntries `
    -FixtureListPath $FixtureList `
    -FixtureGlobPattern $FixtureGlob `
    -ShardIndexValue $ShardIndex `
    -ShardCountValue $ShardCount `
    -LimitValue $Limit `
    -RepoRoot $RepoRoot
  $selectedPositiveFixtures = @($selectedRecoveryEntries | Where-Object { $_.kind -eq "positive" } | ForEach-Object { $_.file })
  $selectedNegativeFixtures = @($selectedRecoveryEntries | Where-Object { $_.kind -eq "negative" } | ForEach-Object { $_.file })

  Write-Output ("selection: positive={0} negative={1}" -f $selectedPositiveFixtures.Count, $selectedNegativeFixtures.Count)
  Invoke-PositiveRecoveryFixtures -Fixtures $selectedPositiveFixtures -OutDir $OutDir
  Invoke-NegativeRecoveryFixtures -Fixtures $selectedNegativeFixtures -OutDir $OutDir
}

Export-ModuleMember -Function @(
  "Get-NegativeFixtureDiagnosticTokenContracts",
  "Invoke-NegativeRecoveryFixtures",
  "Invoke-PositiveRecoveryFixtures",
  "Invoke-SelectedRecoveryFixtureContracts",
  "New-RecoveryFixtureEntries"
)
