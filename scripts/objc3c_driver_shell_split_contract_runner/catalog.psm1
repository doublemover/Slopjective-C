$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Get-Objc3cDriverShellSplitExpectedProjectIncludes {
  @(
    "driver/objc3_driver_main.h"
  )
}

function Get-Objc3cDriverShellSplitForbiddenMainTokens {
  @(
    "ParseObjc3CliOptions(",
    "ApplyObjc3LLVMCabilityRouting(",
    "RunObjc3CompilationDriver(",
    "CompileObjc3SourceForCli(",
    "RunObjectiveCCompile(",
    "WriteManifestArtifact(",
    "WriteDiagnosticsArtifacts(",
    "RunIRCompile("
  )
}

function Get-Objc3cDriverShellSplitExpectedArtifacts {
  @(
    "module.manifest.json",
    "module.diagnostics.txt",
    "module.ll",
    "module.obj",
    "module.object-backend.txt"
  )
}

function New-Objc3cDriverShellSplitContractConfig {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/driver-shell-split"
  $configuredRunId = $env:OBJC3C_DRIVER_SHELL_SPLIT_RUN_ID
  $runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $runDirRel = "tmp/artifacts/objc3c-native/driver-shell-split/$runId"
  $summaryRel = "$runDirRel/summary.json"

  $defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
  $nativeExePath = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExePath } else { $configuredNativeExe }
  $nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)

  [pscustomobject]@{
    RepoRoot = $repoRoot
    SuiteRoot = $suiteRoot
    RunId = $runId
    RunDir = $runDir
    SummaryPath = $summaryPath
    RunDirRel = $runDirRel
    SummaryRel = $summaryRel
    MainSourcePath = Join-Path $repoRoot "native/objc3c/src/main.cpp"
    DriverMainHeaderPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_driver_main.h"
    DriverMainImplPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_driver_main.cpp"
    DriverHeaderPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_compilation_driver.h"
    DriverImplPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_compilation_driver.cpp"
    CliOptionsHeaderPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_cli_options.h"
    BuildScriptPath = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
    NativeExePath = $nativeExePath
    NativeExeExplicit = $nativeExeExplicit
    FixturePath = Join-Path $repoRoot "tests/tooling/fixtures/native/driver_split/smoke_compile_driver_shell_split.objc3"
    ExpectedProjectIncludes = @(Get-Objc3cDriverShellSplitExpectedProjectIncludes)
    ForbiddenMainTokens = @(Get-Objc3cDriverShellSplitForbiddenMainTokens)
    ExpectedArtifacts = @(Get-Objc3cDriverShellSplitExpectedArtifacts)
  }
}

Export-ModuleMember -Function @(
  "New-Objc3cDriverShellSplitContractConfig"
)
