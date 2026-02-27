$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
$compileScript = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"
$outBaseDir = Join-Path $repoRoot "artifacts/compilation/objc3c-native/negative_expectation_check"
$diagnosticHeaderPattern = '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$'
$diagnosticCodePattern = 'O3[A-Z]\d{3}'

function Get-ExpectedDiagnosticCodes {
  param(
    [string]$FixturePath
  )

  $content = Get-Content -Raw $FixturePath
  $headerMatch = [regex]::Match($content, $diagnosticHeaderPattern)
  if (-not $headerMatch.Success) {
    return @{
      HeaderFound = $false
      Codes       = @()
    }
  }

  $rawCodes = [regex]::Matches($headerMatch.Groups[1].Value, $diagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  $codes = @($rawCodes | Select-Object -Unique)

  return @{
    HeaderFound = $true
    Codes       = $codes
  }
}

function Invoke-CompileRun {
  param(
    [string]$FixturePath,
    [string]$OutDir
  )

  if (Test-Path $OutDir) {
    Remove-Item -Recurse -Force $OutDir
  }
  New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

  & powershell -NoProfile -ExecutionPolicy Bypass -File $compileScript $FixturePath --out-dir $OutDir --emit-prefix module | Out-Null
  return [int]$LASTEXITCODE
}

function Get-DiagnosticsData {
  param(
    [string]$OutDir
  )

  $diagPath = Join-Path $OutDir "module.diagnostics.txt"
  if (-not (Test-Path $diagPath -PathType Leaf)) {
    return $null
  }

  $bytes = [System.IO.File]::ReadAllBytes($diagPath)
  $text = [System.IO.File]::ReadAllText($diagPath)
  $codes = [regex]::Matches($text, $diagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  } | Select-Object -Unique

  return @{
    Path  = $diagPath
    Bytes = $bytes
    Text  = $text
    Codes = @($codes)
  }
}

function Test-ByteArrayEqual {
  param(
    [byte[]]$Left,
    [byte[]]$Right
  )

  if ($null -eq $Left -or $null -eq $Right) {
    return $false
  }

  if ($Left.Length -ne $Right.Length) {
    return $false
  }

  for ($i = 0; $i -lt $Left.Length; $i++) {
    if ($Left[$i] -ne $Right[$i]) {
      return $false
    }
  }

  return $true
}

if (-not (Test-Path $compileScript -PathType Leaf)) {
  Write-Output "FAIL: missing compile script at $compileScript"
  exit 1
}

if (-not (Test-Path $fixtureDir -PathType Container)) {
  Write-Output "FAIL: missing negative fixture directory at $fixtureDir"
  exit 1
}

$fixtures = @(Get-ChildItem -Path $fixtureDir -File | Where-Object {
    $_.Extension -in @(".objc3", ".m")
  } | Sort-Object FullName)

if ($fixtures.Count -eq 0) {
  Write-Output "FAIL: no negative fixtures found at $fixtureDir"
  exit 1
}

New-Item -ItemType Directory -Force -Path $outBaseDir | Out-Null

$failedFixtures = New-Object 'System.Collections.Generic.List[object]'

Push-Location $repoRoot
try {
  foreach ($fixture in $fixtures) {
    $fixtureName = $fixture.Name
    $fixtureErrors = New-Object 'System.Collections.Generic.List[string]'

    $headerResult = Get-ExpectedDiagnosticCodes -FixturePath $fixture.FullName
    if (-not $headerResult.HeaderFound) {
      $fixtureErrors.Add("missing expected diagnostic header")
    } elseif ($headerResult.Codes.Count -eq 0) {
      $fixtureErrors.Add("expected diagnostic header has no parseable diagnostic codes")
    }

    $run1Dir = Join-Path $outBaseDir ($fixture.BaseName + "_run1")
    $run2Dir = Join-Path $outBaseDir ($fixture.BaseName + "_run2")

    $exit1 = Invoke-CompileRun -FixturePath $fixture.FullName -OutDir $run1Dir
    $exit2 = Invoke-CompileRun -FixturePath $fixture.FullName -OutDir $run2Dir

    if ($exit1 -eq 0) {
      $fixtureErrors.Add("run1 unexpectedly succeeded")
    }
    if ($exit2 -eq 0) {
      $fixtureErrors.Add("run2 unexpectedly succeeded")
    }
    if ($exit1 -ne $exit2) {
      $fixtureErrors.Add("exit code drift across replay ($exit1 vs $exit2)")
    }

    $diag1 = Get-DiagnosticsData -OutDir $run1Dir
    $diag2 = Get-DiagnosticsData -OutDir $run2Dir

    if ($null -eq $diag1) {
      $fixtureErrors.Add("run1 missing module.diagnostics.txt")
    }
    if ($null -eq $diag2) {
      $fixtureErrors.Add("run2 missing module.diagnostics.txt")
    }

    if ($null -ne $diag1 -and $diag1.Bytes.Length -eq 0) {
      $fixtureErrors.Add("run1 diagnostics file is empty")
    }
    if ($null -ne $diag2 -and $diag2.Bytes.Length -eq 0) {
      $fixtureErrors.Add("run2 diagnostics file is empty")
    }

    if ($null -ne $diag1 -and $null -ne $diag2) {
      if (-not (Test-ByteArrayEqual -Left $diag1.Bytes -Right $diag2.Bytes)) {
        $fixtureErrors.Add("diagnostics bytes drift across replay")
      }
    }

    if ($headerResult.Codes.Count -gt 0) {
      if ($null -ne $diag1) {
        $missingRun1 = @($headerResult.Codes | Where-Object { $_ -notin $diag1.Codes })
        if ($missingRun1.Count -gt 0) {
          $fixtureErrors.Add("run1 missing expected diagnostic code(s): $($missingRun1 -join ', ')")
        }
      } else {
        $fixtureErrors.Add("run1 cannot validate expected diagnostic codes because diagnostics are missing")
      }

      if ($null -ne $diag2) {
        $missingRun2 = @($headerResult.Codes | Where-Object { $_ -notin $diag2.Codes })
        if ($missingRun2.Count -gt 0) {
          $fixtureErrors.Add("run2 missing expected diagnostic code(s): $($missingRun2 -join ', ')")
        }
      } else {
        $fixtureErrors.Add("run2 cannot validate expected diagnostic codes because diagnostics are missing")
      }
    }

    if ($fixtureErrors.Count -eq 0) {
      Write-Output "PASS: $fixtureName"
    } else {
      Write-Output "FAIL: $fixtureName"
      foreach ($errorMessage in $fixtureErrors) {
        Write-Output "  - $errorMessage"
      }
      $failedFixtures.Add([PSCustomObject]@{
          Fixture = $fixtureName
          Errors  = @($fixtureErrors)
        })
    }
  }
}
finally {
  Pop-Location
}

$total = $fixtures.Count
$failed = $failedFixtures.Count
$passed = $total - $failed

if ($failed -eq 0) {
  Write-Output "PASS SUMMARY: $passed/$total fixtures passed expectation enforcement."
  exit 0
}

Write-Output "FAIL SUMMARY: $passed/$total fixtures passed; $failed failed expectation enforcement."
exit 1
