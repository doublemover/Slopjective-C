$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendModuleScaffold {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $payload = Read-Objc3cNativeFrontendArtifactGuardJson `
    -Path $scaffoldPath `
    -MissingMessage "frontend modular scaffold artifact missing at $scaffoldPath" `
    -InvalidMessage "frontend modular scaffold artifact is not valid JSON at $scaffoldPath"

  Assert-Objc3cNativeFrontendArtifactGuardContractId `
    -Payload $payload `
    -ExpectedContractId "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1" `
    -ErrorMessage "frontend modular scaffold contract id mismatch in $scaffoldPath"

  Assert-Objc3cNativeFrontendScaffoldModules -Payload $payload -ScaffoldPath $scaffoldPath
  Assert-Objc3cNativeFrontendScaffoldCounts -Payload $payload -ScaffoldPath $scaffoldPath
}

function Assert-Objc3cNativeFrontendScaffoldModules {
  param(
    [Parameter(Mandatory = $true)]
    $Payload,
    [Parameter(Mandatory = $true)]
    [string]$ScaffoldPath
  )

  $modules = @($Payload.modules)
  $requiredModules = @("driver","diagnostics-io","ir","lex-parse","frontend-api","lowering","pipeline","sema")
  $presentModules = @{}
  foreach ($module in $modules) {
    $moduleName = [string]$module.name
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      Write-Error "frontend modular scaffold has module with missing name in $ScaffoldPath"
      exit 2
    }
    $moduleSources = @($module.sources)
    if ($moduleSources.Count -eq 0) {
      Write-Error "frontend modular scaffold module '$moduleName' has no sources in $ScaffoldPath"
      exit 2
    }
    $presentModules[$moduleName] = $true
  }
  foreach ($moduleName in $requiredModules) {
    if (-not $presentModules.ContainsKey($moduleName)) {
      Write-Error "frontend modular scaffold missing required module '$moduleName' in $ScaffoldPath"
      exit 2
    }
  }
}

function Assert-Objc3cNativeFrontendScaffoldCounts {
  param(
    [Parameter(Mandatory = $true)]
    $Payload,
    [Parameter(Mandatory = $true)]
    [string]$ScaffoldPath
  )

  $modules = @($Payload.modules)
  $sharedSources = @($Payload.shared_sources)
  if ($sharedSources.Count -eq 0) {
    Write-Error "frontend modular scaffold shared_sources must be non-empty in $ScaffoldPath"
    exit 2
  }
  if ([int]$Payload.module_count -ne $modules.Count) {
    Write-Error "frontend modular scaffold module_count mismatch in $ScaffoldPath"
    exit 2
  }
  if ([int]$Payload.shared_source_count -ne $sharedSources.Count) {
    Write-Error "frontend modular scaffold shared_source_count mismatch in $ScaffoldPath"
    exit 2
  }
}
