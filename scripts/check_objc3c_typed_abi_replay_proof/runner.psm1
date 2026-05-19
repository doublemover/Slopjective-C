$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

. (Join-Path $PSScriptRoot "run_id.psm1")
. (Join-Path $PSScriptRoot "path_normalization.psm1")
. (Join-Path $PSScriptRoot "hashing.psm1")
. (Join-Path $PSScriptRoot "expectations.psm1")
. (Join-Path $PSScriptRoot "command_invocation.psm1")
. (Join-Path $PSScriptRoot "catalog.psm1")
. (Join-Path $PSScriptRoot "case_execution.psm1")
. (Join-Path $PSScriptRoot "summary.psm1")

function Invoke-Objc3cTypedAbiReplayProof {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $runId = Resolve-Objc3cTypedAbiReplayRunId `
    -ConfiguredRunId $env:OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID `
    -DefaultRunId "typed_abi-lane-c-typed-abi-default"
  $context = New-Objc3cTypedAbiReplayContext -RepoRoot $repoRoot -RunId $runId

  New-Item -ItemType Directory -Force -Path $context.RunDir | Out-Null
  Push-Location $repoRoot
  try {
    Ensure-Objc3cTypedAbiReplayNativeExecutable -Context $context
    $fixtureRoots = Get-Objc3cTypedAbiReplayFixtureRoots `
      -PositiveFixtureDir $context.PositiveFixtureDir `
      -RepoRoot $repoRoot

    $results = @()
    foreach ($fixtureRel in $fixtureRoots) {
      $results += Invoke-Objc3cTypedAbiReplayCase -FixtureRel $fixtureRel -Context $context
      Write-Output "[PASS] $fixtureRel"
    }

    Write-Objc3cTypedAbiReplaySummary `
      -SummaryPath $context.SummaryPath `
      -RepoRoot $repoRoot `
      -Results $results
  }
  finally {
    Pop-Location
  }
}

Export-ModuleMember -Function Invoke-Objc3cTypedAbiReplayProof
