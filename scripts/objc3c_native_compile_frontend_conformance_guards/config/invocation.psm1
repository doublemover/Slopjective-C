$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendConformanceInvocationConfig {
  [pscustomobject]@{
    backend_flag = "--objc3-ir-object-backend"
    capability_summary_flag = "--llvm-capabilities-summary"
    route_flag = "--objc3-route-backend-from-capabilities"
    allowed_ir_object_backends = @("clang", "llvm-direct")
    allowed_boolean_true_values = @("1", "true", "yes", "on")
    allowed_boolean_false_values = @("0", "false", "no", "off")
    cache_modes = @("no-cache", "cache-aware")
    backend_modes = @("default", "clang", "llvm-direct")
    summary_modes = @("none", "present")
  }
}
