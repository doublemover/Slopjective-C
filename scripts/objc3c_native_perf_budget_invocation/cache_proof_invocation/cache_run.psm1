Set-StrictMode -Version Latest

function Invoke-Objc3cNativePerfWrapperCacheRun {
  param(
    [string]$CompileScript,
    [string]$SourcePath,
    [string]$OutputDirectory,
    [string]$Extension,
    [string]$LogPath,
    [switch]$ForceClangObjectBackend
  )

  $scriptArgs = New-Objc3cNativePerfWrapperCompileArguments `
    -SourcePath $SourcePath `
    -UseCache `
    -OutputDirectory $OutputDirectory `
    -Extension $Extension `
    -ForceClangObjectBackend:$ForceClangObjectBackend.IsPresent

  return (Invoke-Objc3cNativePerfWrapperProcess `
      -ScriptPath $CompileScript `
      -ScriptArguments $scriptArgs `
      -LogPath $LogPath)
}
