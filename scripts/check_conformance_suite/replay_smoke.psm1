Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "state.psm1") -Force -DisableNameChecking

function Invoke-ConformanceReplaySmokeCheck {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][hashtable]$State
  )

  Write-Output "Executable conformance replay-smoke check:"
  $conformanceSmokeRoot = Join-Path $RepoRoot "tmp/artifacts/conformance-smoke"
  $nativeExePath = Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe"
  if ($State["ExecutableSmokeCandidates"].Count -eq 0) {
    Add-ConformanceSuiteFailure -State $State -Message "No executable conformance fixtures were eligible for replay-smoke validation."
  } elseif (-not (Test-Path -LiteralPath $nativeExePath -PathType Leaf)) {
    Add-ConformanceSuiteFailure -State $State -Message ("Missing native compiler executable required for replay-smoke validation: {0}" -f $nativeExePath)
  } else {
    foreach ($candidate in $State["ExecutableSmokeCandidates"]) {
      $id = "$($candidate.fixture_id)".Trim()
      if ([string]::IsNullOrWhiteSpace($id)) {
        $id = "unknown-fixture"
      }
      $safeId = ($id -replace '[^A-Za-z0-9._-]', "_")
      $fixtureRoot = Join-Path $conformanceSmokeRoot $safeId
      [System.IO.Directory]::CreateDirectory($fixtureRoot) | Out-Null
      $sourcePath = Join-Path $fixtureRoot "$safeId.objc3"
      Set-Content -LiteralPath $sourcePath -Value $candidate.source -Encoding utf8

      $run1Dir = Join-Path $fixtureRoot "replay_run_1"
      $run2Dir = Join-Path $fixtureRoot "replay_run_2"
      [System.IO.Directory]::CreateDirectory($run1Dir) | Out-Null
      [System.IO.Directory]::CreateDirectory($run2Dir) | Out-Null
      $run1Log = Join-Path $run1Dir "compile.log"
      $run2Log = Join-Path $run2Dir "compile.log"

      & $nativeExePath $sourcePath "--out-dir" $run1Dir "--emit-prefix" "module" *> $run1Log
      $exit1 = [int]$LASTEXITCODE
      & $nativeExePath $sourcePath "--out-dir" $run2Dir "--emit-prefix" "module" *> $run2Log
      $exit2 = [int]$LASTEXITCODE

      if ($exit1 -ne $exit2) {
        Add-ConformanceSuiteFailure -State $State -Message ("Replay-smoke exit-code drift for fixture '{0}' (bucket '{1}'): run1={2}, run2={3}" -f $id, $candidate.bucket, $exit1, $exit2)
        continue
      }

      if ($exit1 -eq 0) {
        $module1 = Join-Path $run1Dir "module.ll"
        $module2 = Join-Path $run2Dir "module.ll"
        if ((-not (Test-Path -LiteralPath $module1 -PathType Leaf)) -or
            (-not (Test-Path -LiteralPath $module2 -PathType Leaf))) {
          Add-ConformanceSuiteFailure -State $State -Message ("Replay-smoke success path missing IR artifact for fixture '{0}' (bucket '{1}')" -f $id, $candidate.bucket)
          continue
        }
        $hash1 = (Get-FileHash -LiteralPath $module1 -Algorithm SHA256).Hash
        $hash2 = (Get-FileHash -LiteralPath $module2 -Algorithm SHA256).Hash
        if ($hash1 -ne $hash2) {
          Add-ConformanceSuiteFailure -State $State -Message ("Replay-smoke IR hash drift for fixture '{0}' (bucket '{1}')" -f $id, $candidate.bucket)
          continue
        }
        Write-Output ("- {0} ({1}) replay-smoke success path deterministic" -f $id, $candidate.bucket)
        continue
      }

      $diag1 = Join-Path $run1Dir "module.diagnostics.txt"
      $diag2 = Join-Path $run2Dir "module.diagnostics.txt"
      if ((-not (Test-Path -LiteralPath $diag1 -PathType Leaf)) -or
          (-not (Test-Path -LiteralPath $diag2 -PathType Leaf))) {
        Add-ConformanceSuiteFailure -State $State -Message ("Replay-smoke failure path missing diagnostics artifact for fixture '{0}' (bucket '{1}')" -f $id, $candidate.bucket)
        continue
      }
      $diagHash1 = (Get-FileHash -LiteralPath $diag1 -Algorithm SHA256).Hash
      $diagHash2 = (Get-FileHash -LiteralPath $diag2 -Algorithm SHA256).Hash
      if ($diagHash1 -ne $diagHash2) {
        Add-ConformanceSuiteFailure -State $State -Message ("Replay-smoke diagnostics hash drift for fixture '{0}' (bucket '{1}')" -f $id, $candidate.bucket)
        continue
      }
      Write-Output ("- {0} ({1}) replay-smoke failure path deterministic (exit {2})" -f $id, $candidate.bucket, $exit1)
    }
  }
}

Export-ModuleMember -Function "Invoke-ConformanceReplaySmokeCheck"
