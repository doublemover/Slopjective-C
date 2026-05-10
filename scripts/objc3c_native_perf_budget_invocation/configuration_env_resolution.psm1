Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking

function Get-Objc3cNativePerfCompilerPaths {
  param([object]$Config)

  return [pscustomobject]@{
    exe = (Join-Path $Config.repo_root "artifacts/bin/objc3c-native.exe")
    compile_script = (Join-Path $Config.repo_root "scripts/objc3c_native_compile.ps1")
    build_script = (Join-Path $Config.repo_root "scripts/build_objc3c_native.ps1")
  }
}

function Invoke-Objc3cNativePerfCompilerBuildIfMissing {
  param(
    [object]$Config,
    [object]$CompilerPaths,
    [ref]$BuildExecuted,
    [ref]$BuildElapsedMs
  )

  if (Test-Path -LiteralPath $CompilerPaths.exe -PathType Leaf) {
    return
  }

  $BuildExecuted.Value = $true
  $buildLog = Join-Path $Config.run_dir "build.log"
  $buildRun = Invoke-Objc3cNativePerfNativeProcess `
    -Command "powershell" `
    -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $CompilerPaths.build_script) `
    -LogPath $buildLog
  $BuildElapsedMs.Value = $buildRun.elapsed_ms
  if ($buildRun.exit_code -ne 0) {
    throw "perf-budget FAIL: native compiler build failed with exit code $($buildRun.exit_code)"
  }
}

function Assert-Objc3cNativePerfCompilerSurface {
  param([object]$CompilerPaths)

  if (!(Test-Path -LiteralPath $CompilerPaths.exe -PathType Leaf)) {
    throw "perf-budget FAIL: native compiler executable missing at $($CompilerPaths.exe)"
  }
  if (!(Test-Path -LiteralPath $CompilerPaths.compile_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing compile wrapper at $($CompilerPaths.compile_script)"
  }
}

function Resolve-Objc3cNativePerfCompilerAvailability {
  param(
    [object]$Config,
    [ref]$BuildExecuted,
    [ref]$BuildElapsedMs
  )

  $compilerPaths = Get-Objc3cNativePerfCompilerPaths -Config $Config
  Invoke-Objc3cNativePerfCompilerBuildIfMissing `
    -Config $Config `
    -CompilerPaths $compilerPaths `
    -BuildExecuted $BuildExecuted `
    -BuildElapsedMs $BuildElapsedMs
  Assert-Objc3cNativePerfCompilerSurface -CompilerPaths $compilerPaths

  return [pscustomobject]@{
    exe = $compilerPaths.exe
    compile_script = $compilerPaths.compile_script
  }
}

Export-ModuleMember -Function @(
  "Resolve-Objc3cNativePerfCompilerAvailability"
)
