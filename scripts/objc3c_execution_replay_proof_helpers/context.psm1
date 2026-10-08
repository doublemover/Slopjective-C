Set-StrictMode -Version Latest

function Get-ExecutionReplayProofLlvmToolName {
  param([Parameter(Mandatory = $true)][string]$ToolName)

  if ([System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT -and -not $ToolName.EndsWith(".exe")) {
    return "$ToolName.exe"
  }
  return $ToolName
}

function Get-ExecutionReplayProofLlvmRootCandidates {
  $candidates = @()
  foreach ($envName in @("OBJC3C_LLVM_ROOT", "LLVM_ROOT")) {
    $configured = [System.Environment]::GetEnvironmentVariable($envName)
    if (-not [string]::IsNullOrWhiteSpace($configured)) {
      $candidates += $configured
    }
  }

  if ([System.Environment]::OSVersion.Platform -eq [System.PlatformID]::Win32NT) {
    $version = if ($env:OBJC3C_CI_LLVM_VERSION) { $env:OBJC3C_CI_LLVM_VERSION } else { "22.1.6" }
    $userProfile = [System.Environment]::GetEnvironmentVariable("USERPROFILE")
    if (-not [string]::IsNullOrWhiteSpace($userProfile)) {
      $candidates += (Join-Path $userProfile ("Tools\LLVM\llvm-{0}-msvc" -f $version))
    }
    $candidates += "C:\Program Files\LLVM"
  }

  $seen = @{}
  $ordered = @()
  foreach ($candidate in $candidates) {
    if ([string]::IsNullOrWhiteSpace($candidate)) {
      continue
    }
    $key = $candidate.ToLowerInvariant()
    if ($seen.ContainsKey($key)) {
      continue
    }
    $seen[$key] = $true
    $ordered += $candidate
  }
  return $ordered
}

function Resolve-ExecutionReplayProofLlvmTool {
  param([Parameter(Mandatory = $true)][string]$ToolName)

  $executableName = Get-ExecutionReplayProofLlvmToolName -ToolName $ToolName
  foreach ($llvmRoot in @(Get-ExecutionReplayProofLlvmRootCandidates)) {
    $candidate = Join-Path (Join-Path $llvmRoot "bin") $executableName
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
      return $candidate
    }
  }

  $command = Get-Command $ToolName -ErrorAction SilentlyContinue
  if ($null -ne $command -and -not [string]::IsNullOrWhiteSpace($command.Source)) {
    return $command.Source
  }
  $exeCommand = Get-Command $executableName -ErrorAction SilentlyContinue
  if ($null -ne $exeCommand -and -not [string]::IsNullOrWhiteSpace($exeCommand.Source)) {
    return $exeCommand.Source
  }

  return $ToolName
}

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
  $script:llvmReadobjCommand = if ([string]::IsNullOrWhiteSpace($script:configuredLlvmReadobj)) { Resolve-ExecutionReplayProofLlvmTool -ToolName "llvm-readobj" } else { $script:configuredLlvmReadobj }
}

function Initialize-ExecutionReplayProofTimings {
  $script:stageTimings = [ordered]@{
    compile_seconds = 0.0
    readobj_seconds = 0.0
    comparison_seconds = 0.0
    output_report_seconds = 0.0
  }
}
