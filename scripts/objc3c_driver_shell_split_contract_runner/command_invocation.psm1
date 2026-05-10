$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_driver_shell_split_contract_helpers.psm1") -DisableNameChecking

function Invoke-Objc3cDriverShellSplitParseProbe {
  param([Parameter(Mandatory = $true)]$Config)

  $logPath = Join-Path $Config.RunDir "parse_probe.log"
  $exitCode = Invoke-LoggedCommand `
    -Command $Config.NativeExePath `
    -Arguments @($Config.FixturePath, "--driver-shell-split-invalid-flag") `
    -LogPath $logPath
  $text = if (Test-Path -LiteralPath $logPath -PathType Leaf) { Read-NormalizedText -Path $logPath } else { "" }

  [pscustomobject]@{
    ExitCode = $exitCode
    LogPath = $logPath
    Text = $text
    ExpectedDiagnosticToken = "unknown arg: --driver-shell-split-invalid-flag"
  }
}

function New-Objc3cDriverShellSplitCompileReplayLayout {
  param([Parameter(Mandatory = $true)]$Config)

  $run1Dir = Join-Path $Config.RunDir "compile_run1"
  $run2Dir = Join-Path $Config.RunDir "compile_run2"
  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null

  [pscustomobject]@{
    Run1Dir = $run1Dir
    Run2Dir = $run2Dir
    Run1Log = Join-Path $Config.RunDir "compile_run1.log"
    Run2Log = Join-Path $Config.RunDir "compile_run2.log"
  }
}

function New-Objc3cDriverShellSplitCompileArguments {
  param(
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$OutDir
  )

  @($FixturePath, "--out-dir", $OutDir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
}

function Invoke-Objc3cDriverShellSplitCompileReplay {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Layout
  )

  $compileArgsRun1 = New-Objc3cDriverShellSplitCompileArguments -FixturePath $Config.FixturePath -OutDir $Layout.Run1Dir
  $compileArgsRun2 = New-Objc3cDriverShellSplitCompileArguments -FixturePath $Config.FixturePath -OutDir $Layout.Run2Dir
  $run1Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $compileArgsRun1 -LogPath $Layout.Run1Log
  $run2Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $compileArgsRun2 -LogPath $Layout.Run2Log

  [pscustomobject]@{
    Run1Exit = $run1Exit
    Run2Exit = $run2Exit
    Run1Dir = $Layout.Run1Dir
    Run2Dir = $Layout.Run2Dir
    Run1Log = $Layout.Run1Log
    Run2Log = $Layout.Run2Log
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cDriverShellSplitCompileReplay",
  "Invoke-Objc3cDriverShellSplitParseProbe",
  "New-Objc3cDriverShellSplitCompileReplayLayout"
)
