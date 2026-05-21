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
    "negative_obj3next016_parser_missing_semicolon_recovery.objc3" = @(
      "O3P104",
      "missing ';' after assignment"
    )
    "negative_obj3next016_optional_alias_fixit_recovery.objc3" = @(
      "O3C004",
      "optional<T> aliases are rejected; use canonical Optional<T> spelling",
      "O3P100",
      "unsupported Objective-C 3 statement"
    )
    "negative_obj3next016_sema_missing_return_value_recovery.objc3" = @(
      "O3S211",
      "must return 'i32'"
    )
    "negative_obj3next016_sema_unknown_symbol_recovery.objc3" = @(
      "O3S203",
      "unknown function 'known_vaule'"
    )
  }
}

function Get-NegativeFixtureDiagnosticJsonContracts {
  return @{
    "negative_obj3next016_parser_missing_semicolon_recovery.objc3" = @(
      @{
        Code = "O3P104"
        Phase = "parse"
        Category = "parsing"
        RecoveryStrategy = "parser-statement-boundary-synchronization"
      }
    )
    "negative_obj3next016_optional_alias_fixit_recovery.objc3" = @(
      @{
        Code = "O3C004"
        Phase = "parse"
        Category = "configuration"
        FixitReplacement = "Optional"
        RecoveryStrategy = "parser-canonical-spelling-rejection"
      },
      @{
        Code = "O3P100"
        Phase = "parse"
        Category = "parsing"
        RecoveryStrategy = "parser-local-rejection-boundary"
      }
    )
    "negative_obj3next016_sema_missing_return_value_recovery.objc3" = @(
      @{
        Code = "O3S211"
        Phase = "sema"
        Category = "semantic-analysis"
        RecoveryStrategy = "semantic-return-contract-boundary"
      }
    )
    "negative_obj3next016_sema_unknown_symbol_recovery.objc3" = @(
      @{
        Code = "O3S203"
        Phase = "sema"
        Category = "semantic-analysis"
        RecoveryStrategy = "semantic-symbol-resolution-boundary"
      }
    )
  }
}

function ConvertTo-PositiveDiagnosticCoordinate {
  param(
    [object]$Value,
    [string]$FieldName,
    [string]$CaseName
  )

  try {
    $intValue = [int]$Value
  } catch {
    throw "contract FAIL: $CaseName diagnostic JSON field '$FieldName' is not an integer"
  }
  if ($intValue -le 0) {
    throw "contract FAIL: $CaseName diagnostic JSON field '$FieldName' must be positive"
  }
  return $intValue
}

function Assert-DiagnosticJsonPosition {
  param(
    [object]$Position,
    [string]$FieldName,
    [string]$CaseName
  )

  if ($null -eq $Position) {
    throw "contract FAIL: $CaseName diagnostic JSON missing $FieldName"
  }
  $line = ConvertTo-PositiveDiagnosticCoordinate -Value $Position.line -FieldName "$FieldName.line" -CaseName $CaseName
  $column = ConvertTo-PositiveDiagnosticCoordinate -Value $Position.column -FieldName "$FieldName.column" -CaseName $CaseName
  return @{
    Line = $line
    Column = $column
  }
}

function Read-NegativeDiagnosticJsonArtifact {
  param(
    [string]$JsonPath,
    [string]$CaseName
  )

  if (!(Test-Path -LiteralPath $JsonPath -PathType Leaf)) {
    throw "contract FAIL: missing diagnostics JSON artifact for negative fixture $CaseName"
  }
  $jsonText = Get-Content -LiteralPath $JsonPath -Raw
  if ([string]::IsNullOrWhiteSpace($jsonText)) {
    throw "contract FAIL: empty diagnostics JSON artifact for negative fixture $CaseName"
  }
  try {
    $payload = $jsonText | ConvertFrom-Json
  } catch {
    throw "contract FAIL: diagnostics JSON artifact is not parseable for negative fixture $CaseName"
  }
  return [pscustomobject]@{
    Text = $jsonText
    Payload = $payload
  }
}

function Assert-NegativeDiagnosticJsonPayload {
  param(
    [object]$Payload,
    [string]$CaseName
  )

  if ($Payload.schema_version -ne "1.0.0") {
    throw "contract FAIL: $CaseName diagnostics JSON has unexpected schema_version"
  }
  $diagnostics = @($Payload.diagnostics)
  if ($diagnostics.Count -eq 0) {
    throw "contract FAIL: $CaseName diagnostics JSON has no diagnostics"
  }

  foreach ($diagnostic in $diagnostics) {
    foreach ($field in @("severity", "code", "message", "raw", "phase", "category")) {
      if ([string]::IsNullOrWhiteSpace([string]$diagnostic.$field)) {
        throw "contract FAIL: $CaseName diagnostic JSON missing non-empty field '$field'"
      }
    }

    $line = ConvertTo-PositiveDiagnosticCoordinate -Value $diagnostic.line -FieldName "line" -CaseName $CaseName
    $column = ConvertTo-PositiveDiagnosticCoordinate -Value $diagnostic.column -FieldName "column" -CaseName $CaseName
    $spanStart = Assert-DiagnosticJsonPosition -Position $diagnostic.span.start -FieldName "span.start" -CaseName $CaseName
    $spanEnd = Assert-DiagnosticJsonPosition -Position $diagnostic.span.end -FieldName "span.end" -CaseName $CaseName
    if (($spanStart.Line -ne $line) -or ($spanStart.Column -ne $column)) {
      throw "contract FAIL: $CaseName diagnostic JSON span start does not match line/column"
    }
    if (($spanEnd.Line -lt $spanStart.Line) -or (($spanEnd.Line -eq $spanStart.Line) -and ($spanEnd.Column -lt $spanStart.Column))) {
      throw "contract FAIL: $CaseName diagnostic JSON span end precedes start"
    }

    $fixits = @($diagnostic.fixits)
    foreach ($fixit in $fixits) {
      if ($null -eq $fixit.range -or $null -eq $fixit.replacement) {
        throw "contract FAIL: $CaseName diagnostic JSON fix-it is missing range or replacement"
      }
      $null = Assert-DiagnosticJsonPosition -Position $fixit.range.start -FieldName "fixit.range.start" -CaseName $CaseName
      $null = Assert-DiagnosticJsonPosition -Position $fixit.range.end -FieldName "fixit.range.end" -CaseName $CaseName
    }

    if ($null -eq $diagnostic.recovery) {
      throw "contract FAIL: $CaseName diagnostic JSON missing recovery metadata"
    }
    if ($diagnostic.recovery.recovery_counts_as_success -ne $false) {
      throw "contract FAIL: $CaseName diagnostic JSON recovery_counts_as_success must stay false"
    }
    if ($diagnostic.recovery.accepts_invalid_program -ne $false) {
      throw "contract FAIL: $CaseName diagnostic JSON must not accept invalid programs through recovery"
    }
    if ($diagnostic.recovery.deterministic -ne $true) {
      throw "contract FAIL: $CaseName diagnostic JSON recovery metadata must be deterministic"
    }
  }
}

function Assert-NegativeDiagnosticJsonContracts {
  param(
    [object]$Payload,
    [object[]]$Contracts,
    [string]$CaseName
  )

  $diagnostics = @($Payload.diagnostics)
  foreach ($contract in $Contracts) {
    $code = [string]$contract.Code
    $matches = @($diagnostics | Where-Object { $_.code -eq $code })
    if ($matches.Count -eq 0) {
      throw "contract FAIL: $CaseName diagnostics JSON missing expected code $code"
    }
    $diagnostic = $matches[0]
    if ($diagnostic.phase -ne $contract.Phase) {
      throw "contract FAIL: $CaseName diagnostics JSON code $code has phase '$($diagnostic.phase)', expected '$($contract.Phase)'"
    }
    if ($diagnostic.category -ne $contract.Category) {
      throw "contract FAIL: $CaseName diagnostics JSON code $code has category '$($diagnostic.category)', expected '$($contract.Category)'"
    }
    if ($diagnostic.recovery.strategy -ne $contract.RecoveryStrategy) {
      throw "contract FAIL: $CaseName diagnostics JSON code $code has recovery strategy '$($diagnostic.recovery.strategy)', expected '$($contract.RecoveryStrategy)'"
    }
    if (-not [string]::IsNullOrWhiteSpace([string]$contract.FixitReplacement)) {
      $fixits = @($diagnostic.fixits)
      $matchingFixits = @($fixits | Where-Object { $_.replacement -eq $contract.FixitReplacement })
      if ($matchingFixits.Count -eq 0) {
        throw "contract FAIL: $CaseName diagnostics JSON code $code missing fix-it replacement '$($contract.FixitReplacement)'"
      }
    }
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
  $negativeFixtureDiagnosticJsonContracts = Get-NegativeFixtureDiagnosticJsonContracts
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

    $diagJson1 = Read-NegativeDiagnosticJsonArtifact -JsonPath (Join-Path $run1 "module.diagnostics.json") -CaseName "$source run1"
    $diagJson2 = Read-NegativeDiagnosticJsonArtifact -JsonPath (Join-Path $run2 "module.diagnostics.json") -CaseName "$source run2"
    Assert-NegativeDiagnosticJsonPayload -Payload $diagJson1.Payload -CaseName "$source run1"
    Assert-NegativeDiagnosticJsonPayload -Payload $diagJson2.Payload -CaseName "$source run2"

    if ($exit1 -ne $exit2) {
      throw "contract FAIL: negative fixture exit-code drift across replay for $source ($exit1 vs $exit2)"
    }
    if ($diag1 -ne $diag2) {
      throw "contract FAIL: negative fixture diagnostics drift across replay for $source"
    }
    if ($diagJson1.Text -ne $diagJson2.Text) {
      throw "contract FAIL: negative fixture diagnostics JSON drift across replay for $source"
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
    if ($negativeFixtureDiagnosticJsonContracts.ContainsKey($fixtureLeaf)) {
      Assert-NegativeDiagnosticJsonContracts `
        -Payload $diagJson1.Payload `
        -Contracts @($negativeFixtureDiagnosticJsonContracts[$fixtureLeaf]) `
        -CaseName "$source run1"
      Assert-NegativeDiagnosticJsonContracts `
        -Payload $diagJson2.Payload `
        -Contracts @($negativeFixtureDiagnosticJsonContracts[$fixtureLeaf]) `
        -CaseName "$source run2"
      Write-Output "$caseName`_diagnostic_json_contract_verified=true"
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
