$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force

$script:Objc3cNativeLegacyDirectCompileRepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

function Write-Objc3cNativeLegacyDirectCompileStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}
function Publish-ArtifactWithRetry {
  param(
    [Parameter(Mandatory = $true)]
    [string]$StagedPath,
    [Parameter(Mandatory = $true)]
    [string]$FinalPath,
    [int]$MaxAttempts = 40,
    [int]$SleepMilliseconds = 250
  )

  for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
    try {
      Move-Item -LiteralPath $StagedPath -Destination $FinalPath -Force
      return
    } catch {
      if ($attempt -eq $MaxAttempts) {
        throw "failed to publish $FinalPath after $MaxAttempts attempt(s): $($_.Exception.Message)"
      }
      Start-Sleep -Milliseconds $SleepMilliseconds
    }
  }
}

function New-StagedObjectPath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$ObjectDir,
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [int]$Index
  )

  return (Join-Path $ObjectDir ("{0}.{1:D3}.obj" -f $TargetName, $Index))
}

function Compile-ObjectFiles {
  param(
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [string[]]$SourcePaths,
    [Parameter(Mandatory = $true)]
    [string]$ObjectDir,
    [Parameter(Mandatory = $true)]
    [string]$Clangxx,
    [Parameter(Mandatory = $true)]
    [string]$IncludeDir,
    [Parameter(Mandatory = $true)]
    [string]$NativeSourceRoot,
    [bool]$EnableLlvmDirectObjectEmission = $true
  )

  New-Item -ItemType Directory -Force -Path $ObjectDir | Out-Null
  $objectPaths = New-Object System.Collections.Generic.List[string]
  for ($index = 0; $index -lt $SourcePaths.Count; $index++) {
    $sourcePath = $SourcePaths[$index]
    $objectPath = New-StagedObjectPath -ObjectDir $ObjectDir -TargetName $TargetName -Index $index
    $relativeSource = Get-Objc3cNativeRepoRelativePath -RootPath $script:Objc3cNativeLegacyDirectCompileRepoRoot -TargetPath $sourcePath
    Write-Objc3cNativeLegacyDirectCompileStep ("compile_unit=" + $TargetName + " [" + ($index + 1) + "/" + $SourcePaths.Count + "] -> " + $relativeSource)
    $compileArgs = @(
      "-std=c++20"
      "-Wall"
      "-Wextra"
      "-pedantic"
    )
    if ($EnableLlvmDirectObjectEmission) {
      $compileArgs += "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1"
    }
    $compileArgs += @(
      "-I$IncludeDir"
      "-I$NativeSourceRoot"
      "-c"
      $sourcePath
      "-o"
      $objectPath
    )
    & $Clangxx @compileArgs
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $objectPaths.Add($objectPath) | Out-Null
  }

  return $objectPaths.ToArray()
}

function Link-ExecutableFromObjects {
  param(
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [string[]]$ObjectPaths,
    [Parameter(Mandatory = $true)]
    [string]$Libclang,
    [Parameter(Mandatory = $true)]
    [string]$Clangxx,
    [Parameter(Mandatory = $true)]
    [string]$StagedOutput,
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot
  )

  Write-Objc3cNativeLegacyDirectCompileStep ("link_start=" + $TargetName + " -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
  & $Clangxx @ObjectPaths $Libclang -o $StagedOutput
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-Objc3cNativeLegacyDirectCompileStep ("link_done=" + $TargetName + " -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
}
Export-ModuleMember -Function @(
  "Publish-ArtifactWithRetry",
  "New-StagedObjectPath",
  "Compile-ObjectFiles",
  "Link-ExecutableFromObjects"
)