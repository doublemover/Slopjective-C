$ErrorActionPreference = "Stop"

function Get-Objc3cNativeFrontendArtifactContractIds {
  return [ordered]@{
    ModuleScaffold = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
    InvocationLock = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    CoreFeatureExpansion = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  }
}

function Get-Objc3cNativeFrontendBinaryNames {
  return [ordered]@{
    Native = "objc3c-native"
    CapiRunner = "objc3c-frontend-c-api-runner"
  }
}

function Get-Objc3cNativeFrontendCoreFeaturePathConstants {
  return [ordered]@{
    DefaultOutDir = "tmp/artifacts/compilation/objc3c-native"
    CacheRoot = "tmp/artifacts/objc3c-native/cache"
  }
}

function Get-Objc3cNativeFrontendBackendRoutingConstants {
  return [ordered]@{
    AllowedIrObjectBackends = @("clang", "llvm-direct")
    SupportsCapabilityRouting = $true
    CapabilitySummaryFlag = "--llvm-capabilities-summary"
    RouteFlag = "--objc3-route-backend-from-capabilities"
  }
}
