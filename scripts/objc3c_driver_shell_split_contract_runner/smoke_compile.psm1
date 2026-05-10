$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_driver_shell_split_contract_helpers.psm1") -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_invocation.psm1") -Force -DisableNameChecking

function Assert-Objc3cDriverShellSplitParseProbe {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$ParseProbe
  )

  Assert-Objc3cDriverShellSplitExitCode `
    -ActualExitCode $ParseProbe.ExitCode `
    -ExpectedExitCode 2 `
    -Id "smoke.parse_probe.exit_code" `
    -FailureMessage ("parse probe expected exit=2 got exit={0}" -f $ParseProbe.ExitCode) `
    -PassMessage "parse probe enforces shell parse failure exit mapping (exit=2)" `
    -Evidence @{ exit_code = $ParseProbe.ExitCode; log = (Get-RepoRelativePath -Path $ParseProbe.LogPath -Root $Config.RepoRoot) }
  Assert-Objc3cDriverShellSplitTextContains `
    -Text $ParseProbe.Text `
    -Token $ParseProbe.ExpectedDiagnosticToken `
    -Id "smoke.parse_probe.diagnostic_token" `
    -FailureMessage ("parse probe missing diagnostic token '{0}'" -f $ParseProbe.ExpectedDiagnosticToken) `
    -PassMessage "parse probe emits deterministic unknown-arg diagnostic token"
}

function Assert-Objc3cDriverShellSplitCompileReplay {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Replay
  )

  Assert-Objc3cDriverShellSplitExitCode `
    -ActualExitCode $Replay.Run1Exit `
    -ExpectedExitCode 0 `
    -Id "smoke.compile_run1.exit_code" `
    -FailureMessage ("compile run1 failed with exit={0}" -f $Replay.Run1Exit) `
    -PassMessage "compile run1 succeeded" `
    -Evidence @{ exit_code = $Replay.Run1Exit; log = (Get-RepoRelativePath -Path $Replay.Run1Log -Root $Config.RepoRoot) }
  Assert-Objc3cDriverShellSplitExitCode `
    -ActualExitCode $Replay.Run2Exit `
    -ExpectedExitCode 0 `
    -Id "smoke.compile_run2.exit_code" `
    -FailureMessage ("compile run2 failed with exit={0}" -f $Replay.Run2Exit) `
    -PassMessage "compile run2 succeeded" `
    -Evidence @{ exit_code = $Replay.Run2Exit; log = (Get-RepoRelativePath -Path $Replay.Run2Log -Root $Config.RepoRoot) }
}

function Test-Objc3cDriverShellSplitArtifacts {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Replay
  )

  $artifactDigests = [ordered]@{}
  foreach ($artifactName in $Config.ExpectedArtifacts) {
    $run1ArtifactPath = Join-Path $Replay.Run1Dir $artifactName
    $run2ArtifactPath = Join-Path $Replay.Run2Dir $artifactName
    Assert-Objc3cDriverShellSplitArtifactPair `
      -Config $Config `
      -ArtifactName $artifactName `
      -Run1ArtifactPath $run1ArtifactPath `
      -Run2ArtifactPath $run2ArtifactPath

    $run1Hash = Get-FileSha256Hex -Path $run1ArtifactPath
    $run2Hash = Get-FileSha256Hex -Path $run2ArtifactPath
    $artifactDigests[$artifactName] = [ordered]@{
      run1_sha256 = $run1Hash
      run2_sha256 = $run2Hash
      deterministic = ($run1Hash -eq $run2Hash)
    }

    Assert-Objc3cDriverShellSplitArtifactDigest `
      -ArtifactName $artifactName `
      -Run1Hash $run1Hash `
      -Run2Hash $run2Hash
  }

  $artifactDigests
}

function Assert-Objc3cDriverShellSplitCompileOutputs {
  param(
    [Parameter(Mandatory = $true)]$Replay
  )

  $llText = Read-NormalizedText -Path (Join-Path $Replay.Run1Dir "module.ll")
  Assert-Objc3cDriverShellSplitTextContains `
    -Text $llText `
    -Token "define i32 @objc3c_entry" `
    -Id "smoke.ll.contains_objc3c_entry" `
    -FailureMessage "module.ll missing objc3c_entry symbol in smoke compile" `
    -PassMessage "module.ll contains objc3c_entry entrypoint marker"

  $diagText = Read-NormalizedText -Path (Join-Path $Replay.Run1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($diagText)) `
    -Id "smoke.diagnostics.empty_on_success" `
    -FailureMessage "smoke compile emitted diagnostics for successful compile path" `
    -PassMessage "smoke compile diagnostics artifact is empty on success"
}

function Invoke-Objc3cDriverShellSplitContractSmokeCompile {
  param([Parameter(Mandatory = $true)]$Config)

  Assert-Objc3cDriverShellSplitNativeExecutableReady -Config $Config

  $parseProbe = Invoke-Objc3cDriverShellSplitParseProbe -Config $Config
  Assert-Objc3cDriverShellSplitParseProbe -Config $Config -ParseProbe $parseProbe

  $layout = New-Objc3cDriverShellSplitCompileReplayLayout -Config $Config
  $replay = Invoke-Objc3cDriverShellSplitCompileReplay -Config $Config -Layout $layout
  Assert-Objc3cDriverShellSplitCompileReplay -Config $Config -Replay $replay
  $artifactDigests = Test-Objc3cDriverShellSplitArtifacts -Config $Config -Replay $replay
  Assert-Objc3cDriverShellSplitCompileOutputs -Replay $replay

  [ordered]@{
    fixture = Get-RepoRelativePath -Path $Config.FixturePath -Root $Config.RepoRoot
    compile_run1_exit = $replay.Run1Exit
    compile_run2_exit = $replay.Run2Exit
    compile_run1_dir = Get-RepoRelativePath -Path $replay.Run1Dir -Root $Config.RepoRoot
    compile_run2_dir = Get-RepoRelativePath -Path $replay.Run2Dir -Root $Config.RepoRoot
    artifact_digests = $artifactDigests
  }
}

Export-ModuleMember -Function "Invoke-Objc3cDriverShellSplitContractSmokeCompile"
