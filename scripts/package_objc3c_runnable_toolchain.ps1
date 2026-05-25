param(
  [string]$PackageRoot = "",
  [string]$ManifestRelativePath = "artifacts/package/objc3c-runnable-toolchain-package.json",
  [ValidateSet("release", "address", "undefined")]
  [string]$SanitizerVariant = "release",
  [int]$Parallelism = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$packageScriptModuleRoot = Join-Path $PSScriptRoot "package_objc3c_runnable_toolchain"
Import-Module (Join-Path $packageScriptModuleRoot "cli_request.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "environment_paths.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "staging_orchestration.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "artifact_report.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "status_rendering.psm1") -Force -DisableNameChecking
$platformEvidenceModule = Import-Module `
  (Join-Path $PSScriptRoot "objc3c_platform_host_evidence_producers.psm1") `
  -Force `
  -DisableNameChecking `
  -PassThru
function Get-RunnableToolchainPlatformRuntimeManifestEvidenceCommand {
  param(
    [Parameter(Mandatory = $true)][object]$Module,
    [Parameter(Mandatory = $true)][string]$PlatformId
  )

  $commandName = if ($PlatformId -eq "linux-x64") {
    "Write-Objc3cLinuxRuntimeLibraryManifestEvidence"
  } elseif ($PlatformId -eq "darwin-arm64") {
    "Write-Objc3cDarwinRuntimeLibraryManifestEvidence"
  } else {
    ""
  }
  if ([string]::IsNullOrWhiteSpace($commandName)) {
    return $null
  }

  return Get-Command `
    -Module $Module.Name `
    -Name $commandName `
    -ErrorAction Stop
}

$request = New-RunnableToolchainPackageRequest `
  -PackageRoot $PackageRoot `
  -ManifestRelativePath $ManifestRelativePath `
  -SanitizerVariant $SanitizerVariant

$environment = Resolve-RunnableToolchainPackageEnvironment `
  -ScriptRoot $PSScriptRoot `
  -PackageRoot $request.PackageRoot `
  -ManifestRelativePath $request.ManifestRelativePath

$staging = Invoke-RunnableToolchainPackageStaging `
  -RepoRoot $environment.RepoRoot `
  -PackageRoot $environment.PackageRoot `
  -ManifestPath $environment.ManifestPath `
  -BuildScript $environment.BuildScript `
  -SanitizerVariant $request.SanitizerVariant `
  -Parallelism $Parallelism

$manifestPayload = Write-RunnableToolchainPackageManifest `
  -RepoRoot $environment.RepoRoot `
  -PackageRoot $environment.PackageRoot `
  -ManifestPath $environment.ManifestPath `
  -StagedRelativePaths $staging.StagedRelativePaths `
  -SanitizerVariant $request.SanitizerVariant

$writeRuntimeLibraryManifestEvidence = Get-RunnableToolchainPlatformRuntimeManifestEvidenceCommand `
  -Module $platformEvidenceModule `
  -PlatformId $manifestPayload.target_platform_id
if ($null -ne $writeRuntimeLibraryManifestEvidence) {
  & $writeRuntimeLibraryManifestEvidence `
    -RepoRoot $environment.RepoRoot `
    -PlatformId $manifestPayload.target_platform_id `
    -PackageRoot $environment.PackageRoot `
    -PackageManifestPath $environment.ManifestPath `
    -RuntimeLibraryRelativePath $manifestPayload.runtime_library `
    -RuntimeLibraryName $manifestPayload.runtime_library_name `
    -TargetTriple $manifestPayload.target_triple `
    -ObjectFormat $manifestPayload.object_format `
    -DebugFormat $manifestPayload.debug_format
}

Write-RunnableToolchainPackageStatus -ManifestPayload $manifestPayload
