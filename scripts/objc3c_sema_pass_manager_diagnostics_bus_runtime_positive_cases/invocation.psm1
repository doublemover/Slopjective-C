function Invoke-SemaPassManagerPositiveReplay {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$Backend,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][string]$Run1Log,
    [Parameter(Mandatory = $true)][string]$Run2Log,
    [string[]]$AdditionalArgs = @()
  )

  $argsRun1 = Get-SemaPassManagerPositiveBackendArgs `
    -FixturePath $FixturePath `
    -OutDir $Run1Dir `
    -Backend $Backend `
    -AdditionalArgs $AdditionalArgs
  $argsRun2 = Get-SemaPassManagerPositiveBackendArgs `
    -FixturePath $FixturePath `
    -OutDir $Run2Dir `
    -Backend $Backend `
    -AdditionalArgs $AdditionalArgs

  return [pscustomobject]@{
    run1_exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun1 -LogPath $Run1Log
    run2_exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun2 -LogPath $Run2Log
  }
}
