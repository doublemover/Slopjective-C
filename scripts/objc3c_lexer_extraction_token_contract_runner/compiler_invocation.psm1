$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_lexer_extraction_token_contract_helpers.psm1") -DisableNameChecking

function Assert-Objc3cLexerExtractionNativeExecutableReady {
  param([Parameter(Mandatory = $true)]$Config)

  if (-not $Config.NativeExeExplicit -and !(Test-Path -LiteralPath $Config.NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $Config.RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $Config.BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $Config.RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $Config.RepoRoot) }
  }

  Assert-FileExists -Path $Config.NativeExePath -Id "runtime.native_executable.exists" -Description "native executable"
}

function New-Objc3cLexerExtractionCompileArguments {
  param(
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$OutDir
  )

  @($FixturePath, "--out-dir", $OutDir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
}

function New-Objc3cLexerExtractionReplayLayout {
  param(
    [Parameter(Mandatory = $true)][string]$CaseDir
  )

  $run1Dir = Join-Path $CaseDir "run1"
  $run2Dir = Join-Path $CaseDir "run2"
  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null

  [pscustomobject]@{
    CaseDir = $CaseDir
    Run1Dir = $run1Dir
    Run2Dir = $run2Dir
    Run1Log = Join-Path $CaseDir "run1.log"
    Run2Log = Join-Path $CaseDir "run2.log"
  }
}

function Invoke-Objc3cLexerExtractionCompileReplay {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)]$Layout
  )

  $run1Args = New-Objc3cLexerExtractionCompileArguments -FixturePath $FixturePath -OutDir $Layout.Run1Dir
  $run2Args = New-Objc3cLexerExtractionCompileArguments -FixturePath $FixturePath -OutDir $Layout.Run2Dir
  $run1Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $run1Args -LogPath $Layout.Run1Log
  $run2Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $run2Args -LogPath $Layout.Run2Log

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
  "Assert-Objc3cLexerExtractionNativeExecutableReady",
  "Invoke-Objc3cLexerExtractionCompileReplay",
  "New-Objc3cLexerExtractionReplayLayout"
)
