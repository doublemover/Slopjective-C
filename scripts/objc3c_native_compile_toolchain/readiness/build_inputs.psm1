$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-NativeCompilerBuildInputPaths {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)
  # Retained for diagnostics only. Correct freshness is determined by CMake's
  # generated Ninja graph, which owns the complete transitive source/header set.
  return @((Join-Path $RepoRoot "native/objc3c/CMakeLists.txt"))
}
