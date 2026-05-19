$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendConformanceRejectDiagnostics {
  @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
}
