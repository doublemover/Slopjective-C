Set-StrictMode -Version Latest

function Initialize-ExecutionReplayProofContext {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $script:repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $script:proofRoot = Join-Path $script:repoRoot "tmp/artifacts/objc3c-native/execution-replay-proof"
  $script:proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
  $script:proofDir = Join-Path $script:proofRoot $script:proofRunId
  $script:summaryPath = Join-Path $script:proofDir "summary.json"
  $script:buildScript = Join-Path $script:repoRoot "scripts/build_objc3c_native.ps1"
  $script:compileScript = Join-Path $script:repoRoot "scripts/objc3c_native_compile.ps1"
  $script:defaultNativeExe = Join-Path $script:repoRoot "artifacts/bin/objc3c-native.exe"
  $script:configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
  $script:nativeExe = if ([string]::IsNullOrWhiteSpace($script:configuredNativeExe)) { $script:defaultNativeExe } else { $script:configuredNativeExe }
  $script:nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($script:configuredNativeExe)
  $script:shellCommand = (Get-Process -Id $PID).Path
  $script:configuredLlvmReadobj = $env:OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH
  $script:llvmReadobjCommand = if ([string]::IsNullOrWhiteSpace($script:configuredLlvmReadobj)) { "llvm-readobj" } else { $script:configuredLlvmReadobj }
}

function Initialize-ExecutionReplayProofTimings {
  $script:stageTimings = [ordered]@{
    compile_seconds = 0.0
    readobj_seconds = 0.0
    comparison_seconds = 0.0
    output_report_seconds = 0.0
  }
}
