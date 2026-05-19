$ErrorActionPreference = "Stop"

function Get-Objc3cNativeFrontendCloseoutConformanceContractIds {
  return [ordered]@{
    RecoveryDeterminismHardening = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    EdgeCompatCompletion = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    ConformanceMatrix = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
    ConformanceCorpus = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
    IntegrationCloseout = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
  }
}

function Get-Objc3cNativeFrontendConformanceProfileDimensions {
  return [ordered]@{
    CacheModes = @("no-cache", "cache-aware")
    BackendModes = @("default", "clang", "llvm-direct")
    SummaryModes = @("none", "present")
  }
}

function Get-Objc3cNativeFrontendConformanceCorpusPathConstants {
  return [ordered]@{
    FixtureSource = "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
    OutputDirectory = "tmp/reports/parser_build/backend_route_capability_smoke/out"
    CapabilitiesSummary = "tmp/artifacts/objc3c-native/llvm_capabilities_summary.json"
  }
}
