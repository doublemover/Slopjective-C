param(
  [string]$FixtureList = "",
  [string]$FixtureGlob = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_execution_smoke_runner.psm1") -Force -DisableNameChecking

# Suite ownership: this script is the authoritative owner for compile/link/run
# execution behavior. Recovery, replay/native-truth, runtime acceptance, and
# negative-fixture header enforcement must stay on their dedicated suites so the
# public runner can remove duplicate heavy recompilation.
# objc3c.execution.runnablesample.surface.v1 anchor: execution smoke remains the
# scalar/core corpus boundary rooted at tests/tooling/fixtures/native/execution.
# Broader object/property/import-module samples stay frozen as separate proof
# families until the canonical runnable sample surface widens the sample set.
# objc3c.execution.canonicalrunnablesamples.v1 anchor: the integrated object/property/category/protocol sample exists as a dedicated proof asset,
# but execution smoke still remains the scalar/core corpus gate in this issue.
# objc3c.execution.runnablecore.compatibilityguard.v1 anchor: advanced unsupported features must fail closed instead of counting as runnable smoke coverage.
# objc3c.execution.unsupportedfeature.diagnostics.v1 anchor: `O3S221` fail-closed diagnostics for accepted advanced surfaces
# must stay outside runnable smoke counts and never be treated as successful runtime coverage.
# objc3c.execution.replayinspection.freeze.v1 anchor: execution smoke remains the
# canonical runnable replay corpus boundary for scalar/core coverage. Broader
# object inspection is frozen onto the dedicated A002 sample instead of widening
# this smoke script.
# objc3c.execution.objectir.replayproof.v1 anchor: scalar/core smoke remains the base
# runtime replay corpus, while canonical object/IR replay plus metadata section
# inspection is delegated to execution replay proof over the A002 runnable
# sample.
# objc3c.execution.toolchainruntime.operations.v1 anchor: execution smoke remains one of
# the frozen runnable-core operations, and installer or cross-platform packaging claims remain outside this freeze until the workflow-package surface widens.
# objc3c.execution.workflowpackage.surface.v1 anchor: this script must run unchanged from a
# staged runnable toolchain bundle root that preserves the current repo-relative
# scripts/artifacts/tests layout under a local package root.
# objc3c.execution.platformbringup.surface.v1 anchor: supported repo-root/package-root execution
# must document the live override surface for `OBJC3C_NATIVE_EXECUTABLE`,
# `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`, `OBJC3C_NATIVE_EXECUTION_LLC_PATH`, and
# `OBJC3C_NATIVE_EXECUTION_RUN_ID` on the supported Windows host baseline.
# objc3c.execution.releasegate.freeze.v1 anchor: execution smoke remains one preserved
# closeout-gate input alongside the canonical runnable sample, unsupported-feature diagnostics, and object replay proof surfaces,
# plus the platform-bringup surface; full matrix expansion remains deferred to the conformance-matrix surface.
# objc3c.execution.conformancematrix.surface.v1 anchor: execution smoke remains one concrete
# command-backed row in the runnable conformance matrix rather than an implicit
# blanket claim about unsupported runtime surfaces.
# objc3c.execution.closeoutsummary.surface.v1 anchor: execution smoke remains one preserved
# operator command in the final execution closeout runbook and sign-off summary.

Invoke-Objc3cNativeExecutionSmoke `
  -ScriptRoot $PSScriptRoot `
  -FixtureList $FixtureList `
  -FixtureGlob $FixtureGlob `
  -ShardIndex $ShardIndex `
  -ShardCount $ShardCount `
  -Limit $Limit
