$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_superclean_surface_catalog.psm1") -Force

function Add-Objc3cNativeRepoSupercleanSurfaceEntries {
  param(
    [Parameter(Mandatory = $true)]
    $Payload,
    [Parameter(Mandatory = $true)]
    $Entries
  )

  foreach ($key in $Entries.Keys) {
    $Payload[$key] = $Entries[$key]
  }
}

function Test-Objc3cNativeRepoSupercleanPathIsUnderRoot {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RootPath,
    [Parameter(Mandatory = $true)]
    [string]$TargetPath
  )

  $rootFullPath = [System.IO.Path]::GetFullPath($RootPath).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
  $targetFullPath = [System.IO.Path]::GetFullPath($TargetPath)

  return `
    $targetFullPath.Equals($rootFullPath, [System.StringComparison]::OrdinalIgnoreCase) -or `
    $targetFullPath.StartsWith(
      $rootFullPath + [System.IO.Path]::DirectorySeparatorChar,
      [System.StringComparison]::OrdinalIgnoreCase
    )
}

function Write-Objc3cNativeRepoSupercleanSourceOfTruthArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$ExecutionMode,
    [Parameter(Mandatory = $true)]
    [string]$CompileCommandsPath,
    [Parameter(Mandatory = $true)]
    [string]$NativeExecutablePath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendCapiRunnerPath,
    [Parameter(Mandatory = $true)]
    [string]$RuntimeLibraryPath,
    [Parameter(Mandatory = $true)]
    [object[]]$FrontendDefinitions
  )

  $tmpArtifactsRoot = Join-Path $RepoRoot "tmp/artifacts"
  if (Test-Objc3cNativeRepoSupercleanPathIsUnderRoot -RootPath $tmpArtifactsRoot -TargetPath $OutputPath) {
    throw "repo-superclean source-of-truth artifact must be generated from the active build state, not tmp/artifacts: $OutputPath"
  }

  $payload = New-Objc3cNativeRepoSupercleanBaseSurface -ExecutionMode $ExecutionMode

  $payload["native_build_outputs"] = [ordered]@{
    native_executable = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $NativeExecutablePath
    frontend_c_api_runner = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $FrontendCapiRunnerPath
    runtime_library = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $RuntimeLibraryPath
    compile_commands = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $CompileCommandsPath
  }

  Add-Objc3cNativeRepoSupercleanSurfaceEntries `
    -Payload $payload `
    -Entries (New-Objc3cNativeRepoSupercleanBonusSurfaces)
  Add-Objc3cNativeRepoSupercleanSurfaceEntries `
    -Payload $payload `
    -Entries (New-Objc3cNativeRepoSupercleanPerformanceSurfaces)
  Add-Objc3cNativeRepoSupercleanSurfaceEntries `
    -Payload $payload `
    -Entries (New-Objc3cNativeRepoSupercleanReleaseSurfaces)
  Add-Objc3cNativeRepoSupercleanSurfaceEntries `
    -Payload $payload `
    -Entries (New-Objc3cNativeRepoSupercleanProgramSurfaces)

  $payload["frontend_contract_artifacts"] = @(
    $FrontendDefinitions | ForEach-Object {
      [ordered]@{
        name = [string]$_.Name
        family = [string]$_.Family
        artifact_path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath ([string]$_.OutputPath)
      }
    }
  )
  $payload["explicit_non_goals"] = Get-Objc3cNativeRepoSupercleanExplicitNonGoals

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

Export-ModuleMember -Function "Write-Objc3cNativeRepoSupercleanSourceOfTruthArtifact"
