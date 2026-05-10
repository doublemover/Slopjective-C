function Assert-ParserAstBuilderNativeExecutableReady {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit
  )

  if (-not $NativeExeExplicit -and !(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot) }
  }

  Assert-FileExists -Path $NativeExePath -Id "runtime.native_executable.exists" -Description "native executable"
}

function Invoke-ParserAstBuilderNativeReplay {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][string]$Run1Log,
    [Parameter(Mandatory = $true)][string]$Run2Log
  )

  $argsRun1 = @($FixturePath, "--out-dir", $Run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $argsRun2 = @($FixturePath, "--out-dir", $Run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $run1Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun1 -LogPath $Run1Log
  $run2Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun2 -LogPath $Run2Log

  return [pscustomobject]@{
    run1_exit = $run1Exit
    run2_exit = $run2Exit
  }
}
