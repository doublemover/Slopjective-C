$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendCoreCompileArgsImpl {
  param(
    [object]$ParsedArgs,
    [hashtable]$AllowedBackends,
    [string]$ArtifactPath
  )

  $state = New-FrontendCoreCompileArgGuardState
  $compileArgs = @(Read-FrontendFeatureCompileArgs -ParsedArgs $ParsedArgs)

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if (Read-FrontendCoreBackendCompileArgToken -State $state -Token $token -CompileArgs $compileArgs -Index ([ref]$i)) {
      continue
    }
    if (Read-FrontendCoreCapabilityRouteCompileArgToken -State $state -Token $token) {
      continue
    }
    if (Read-FrontendCoreCapabilitySummaryCompileArgToken -State $state -Token $token -CompileArgs $compileArgs -Index ([ref]$i)) {
      continue
    }
  }

  Assert-FrontendCoreCompileArgState `
    -State $state `
    -AllowedBackends $AllowedBackends `
    -ArtifactPath $ArtifactPath
}
