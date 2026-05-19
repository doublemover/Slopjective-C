Set-StrictMode -Version Latest

function Write-Objc3cNativePerfFixtureSetLine {
  param([object]$FixtureSet)

  Write-Output ("fixture-set: kind={0} root={1} count={2}" -f $FixtureSet.fixture_kind, $FixtureSet.fixture_root, $FixtureSet.fixture_count)
}

function Write-Objc3cNativePerfCompileResultLine {
  param([object]$CompileResult)

  Write-Output ("[{0}] {1} elapsed_ms={2} ({3})" -f $CompileResult.status_token, $CompileResult.fixture_rel, $CompileResult.result.elapsed_ms, $CompileResult.detail)
}

function Write-Objc3cNativePerfCacheProofLine {
  param(
    [string]$FixtureRel,
    [string]$FixtureKind,
    [bool]$Run1Hit,
    [bool]$Run2Hit
  )

  Write-Output ("cache-proof PASS fixture={0} fixture_kind={1} run1_hit={2} run2_hit={3}" -f $FixtureRel, $FixtureKind, $Run1Hit, $Run2Hit)
}

function Write-Objc3cNativePerfCacheInvalidationLine {
  param(
    [string]$FixtureRel,
    [bool]$Run1Hit,
    [bool]$Run2Hit
  )

  Write-Output ("cache-invalidation PASS fixture={0} run1_hit={1} run2_hit={2}" -f $FixtureRel, $Run1Hit, $Run2Hit)
}

function Write-Objc3cNativePerfMacroHostLine {
  param(
    [string]$FixtureRel,
    [bool]$Run1Hit,
    [bool]$Run2Hit
  )

  Write-Output ("macro-host-cache PASS fixture={0} run1_hit={1} run2_hit={2}" -f $FixtureRel, $Run1Hit, $Run2Hit)
}

function Write-Objc3cNativePerfDocsGenerationLine {
  param(
    [double]$NativeDocsElapsedMs,
    [double]$CommandSurfaceElapsedMs
  )

  Write-Output ("docs-generation PASS native_docs_ms={0} command_surface_ms={1}" -f $NativeDocsElapsedMs, $CommandSurfaceElapsedMs)
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativePerfCacheInvalidationLine",
  "Write-Objc3cNativePerfCacheProofLine",
  "Write-Objc3cNativePerfCompileResultLine",
  "Write-Objc3cNativePerfDocsGenerationLine",
  "Write-Objc3cNativePerfFixtureSetLine",
  "Write-Objc3cNativePerfMacroHostLine"
)
