$ErrorActionPreference = "Stop"

function Get-Objc3cNativeFrontendCloseoutEdgeContractIds {
  return [ordered]@{
    CoreFeatureExpansion = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    ManifestGuard = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    EdgeCompatCompletion = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    EdgeRobustness = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    DiagnosticsHardening = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
    RecoveryDeterminismHardening = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    NativeCacheEntryRecoveryDeterminismHardening = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
  }
}

function Get-Objc3cNativeFrontendCloseoutEdgeBackendCompatibility {
  return [ordered]@{
    AliasToCanonical = [ordered]@{
      "clang" = "clang"
      "clang++" = "clang"
      "clang-cl" = "clang"
      "llvm-direct" = "llvm-direct"
      "llvm_direct" = "llvm-direct"
      "llvmdirect" = "llvm-direct"
      "llvm" = "llvm-direct"
    }
    SingleValueFlags = @(
      "--objc3-ir-object-backend",
      "--llvm-capabilities-summary"
    )
  }
}

function Get-Objc3cNativeFrontendCloseoutEdgeInvocationCompatibility {
  return [ordered]@{
    SupportsEqualsFormFlags = @(
      "--out-dir",
      "--objc3-ir-object-backend",
      "--llvm-capabilities-summary"
    )
    SupportsBooleanEqualsFlags = @(
      "--use-cache",
      "--objc3-route-backend-from-capabilities"
    )
    RouteFlag = "--objc3-route-backend-from-capabilities"
    CapabilitySummaryFlag = "--llvm-capabilities-summary"
    FailClosedExitCode = 2
    DisallowRelativeParentSegments = $true
  }
}

function Get-Objc3cNativeFrontendCloseoutEdgeWrapperGuardrails {
  return [ordered]@{
    WrapperSingleValueFlags = @(
      "--use-cache",
      "--out-dir"
    )
    CompileSingleValueFlags = @(
      "--objc3-ir-object-backend",
      "--llvm-capabilities-summary",
      "--objc3-route-backend-from-capabilities"
    )
    RejectEmptyEqualsValueFlags = @(
      "--out-dir",
      "--emit-prefix",
      "--clang",
      "--objc3-ir-object-backend",
      "--llvm-capabilities-summary",
      "--objc3-route-backend-from-capabilities",
      "--use-cache"
    )
  }
}

function Get-Objc3cNativeFrontendCloseoutEdgeDiagnostics {
  return [ordered]@{
    FailClosedExitCode = 2
    RequiredErrorMessages = @(
      "--use-cache can be provided at most once",
      "invalid --use-cache value",
      "--out-dir can be provided at most once",
      "missing value for --out-dir",
      "empty value for --out-dir",
      "missing value for --emit-prefix",
      "empty value for --emit-prefix",
      "missing value for --clang",
      "empty value for --clang"
    )
  }
}

function Get-Objc3cNativeFrontendCloseoutEdgeCacheDeterminism {
  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  return [ordered]@{
    FailClosedExitCode = 2
    EntryContractId = $contracts.NativeCacheEntryRecoveryDeterminismHardening
    CacheStatusTokens = @(
      "cache_hit=true",
      "cache_hit=false"
    )
    RequiredEntryFiles = @(
      "files",
      "exit_code.txt",
      "ready.marker",
      "metadata.json"
    )
    RecoverySignals = @(
      "cache_recovery=metadata_missing",
      "cache_recovery=metadata_invalid",
      "cache_recovery=metadata_contract_mismatch",
      "cache_recovery=metadata_cache_key_mismatch",
      "cache_recovery=metadata_exit_code_mismatch",
      "cache_recovery=metadata_digest_mismatch",
      "cache_recovery=restore_failed"
    )
  }
}
