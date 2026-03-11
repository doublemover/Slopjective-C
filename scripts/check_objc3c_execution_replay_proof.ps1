$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-replay-proof"
$proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$proofDir = Join-Path $proofRoot $proofRunId
# M259-A001 runnable-sample-surface anchor: replay proof preserves the same
# scalar/core corpus carried by execution smoke. Object/property/import-module
# sample families stay tracked through their dedicated summary chains until
# M259-A002 unifies the canonical runnable sample set.
# M259-A002 canonical-runnable-sample-set anchor: the dedicated canonical sample-set proof remains separate from scalar replay in this issue.
# M259-B001 runnable-core-compatibility-guard anchor: advanced unsupported features must fail closed instead of counting as replay-proof runnable coverage.
# M259-B002 unsupported-advanced-feature-diagnostics anchor: `O3S221` fail-closed diagnostics for accepted advanced surfaces
# must stay outside replay-proof runnable counts and never be treated as stable executable coverage.
# M259-C001 replay-inspection-freeze anchor: replay proof remains the
# canonical replay-proof artifact boundary for the current runnable slice.
# Deeper object/metadata inspection is frozen onto the dedicated A002 sample
# instead of widening the scalar replay corpus here.
# M259-C002 object-ir-replay-proof anchor: canonical runnable A002 programs now
# prove direct IR replay, object replay, and metadata-section inspection replay
# on the live execution proof path.
# M259-D001 toolchain-runtime-operations anchor: execution replay proof remains
# a frozen runnable-core operation, while workflow/package expansion remains deferred to M259-D002.
# M259-D002 workflow-package anchor: this script must run unchanged from a
# staged runnable toolchain bundle root that preserves the current repo-relative
# scripts/artifacts/tests layout under a local package root.
# M259-D003 platform-bringup anchor: supported repo-root/package-root replay
# must document the live override surface for `OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`
# and `OBJC3C_NATIVE_EXECUTION_RUN_ID` on the supported Windows host baseline.
$summaryPath = Join-Path $proofDir "summary.json"
$smokeScript = Join-Path $repoRoot "scripts/check_objc3c_native_execution_smoke.ps1"
$executionRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
$run1Id = "${proofRunId}_run1"
$run2Id = "${proofRunId}_run2"
$canonicalRunnableFixture = Join-Path $repoRoot "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3"
$defaultNativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredLlvmReadobj = $env:OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH
$llvmReadobjCommand = if ([string]::IsNullOrWhiteSpace($configuredLlvmReadobj)) { "llvm-readobj" } else { $configuredLlvmReadobj }
$requiredRuntimeSections = @(
  "objc3.runtime.class_descriptors",
  "objc3.runtime.protocol_descriptors",
  "objc3.runtime.category_descriptors",
  "objc3.runtime.property_descriptors",
  "objc3.runtime.ivar_descriptors",
  "objc3.runtime.selector_pool",
  "objc3.runtime.string_pool",
  "objc3.runtime.discovery_root",
  "objc3.runtime.linker_anchor",
  "objc3.runtime.image_root",
  "objc3.runtime.registration_descriptor"
)

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-Sha256HexFromText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Text)
    $hashBytes = $sha256.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $sha256.Dispose()
  }
}

function Get-Sha256HexFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "execution replay proof FAIL: missing file for hashing $Path"
  }
  return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-NormalizedTextFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "execution replay proof FAIL: missing text file $Path"
  }
  return (Get-Content -LiteralPath $Path -Raw).Replace("`r`n", "`n")
}

function Get-NormalizedReadobjSectionText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $normalizedLines = foreach ($line in ($Text -split "`r?`n")) {
    if ($line.StartsWith("File: ")) {
      "File: <canonical-object>"
    }
    else {
      $line
    }
  }
  return (($normalizedLines -join "`n").TrimEnd() + "`n")
}

function Get-ReadobjSectionNames {
  param([Parameter(Mandatory = $true)][string]$Text)

  $names = [System.Collections.Generic.List[string]]::new()
  foreach ($line in ($Text -split "`r?`n")) {
    $trimmed = $line.Trim()
    if ($trimmed.StartsWith("Name: ")) {
      $name = $trimmed.Substring(6)
      $parenIndex = $name.IndexOf(" (", [System.StringComparison]::Ordinal)
      if ($parenIndex -ge 0) {
        $name = $name.Substring(0, $parenIndex)
      }
      $names.Add($name.Trim())
    }
  }
  return @($names)
}

function Resolve-NativeExeForReplay {
  param([Parameter(Mandatory = $true)][string]$SummaryPath)

  $summary = Get-Content -LiteralPath $SummaryPath -Raw | ConvertFrom-Json
  if ($summary.PSObject.Properties.Name -contains "native_exe") {
    $nativeRelative = "$($summary.native_exe)"
    if (![string]::IsNullOrWhiteSpace($nativeRelative)) {
      $candidate = Join-Path $repoRoot ($nativeRelative.Replace('/', '\'))
      if (Test-Path -LiteralPath $candidate -PathType Leaf) {
        return $candidate
      }
    }
  }
  if (Test-Path -LiteralPath $defaultNativeExe -PathType Leaf) {
    return $defaultNativeExe
  }
  throw "execution replay proof FAIL: unable to resolve native executable for canonical runnable replay"
}

function Invoke-CanonicalRunnableReplayArtifact {
  param(
    [Parameter(Mandatory = $true)][string]$RunLabel,
    [Parameter(Mandatory = $true)][string]$NativeExe
  )

  $artifactRoot = Join-Path $proofDir "canonical_runnable_${RunLabel}"
  $outDir = Join-Path $artifactRoot "out"
  $compileLog = Join-Path $artifactRoot "compile.log"
  $readobjLog = Join-Path $artifactRoot "readobj-sections.log"
  New-Item -ItemType Directory -Force -Path $artifactRoot | Out-Null
  New-Item -ItemType Directory -Force -Path $outDir | Out-Null

  & $NativeExe $canonicalRunnableFixture --out-dir $outDir --emit-prefix module *> $compileLog
  $compileExit = [int]$LASTEXITCODE
  if ($compileExit -ne 0) {
    throw "execution replay proof FAIL: canonical runnable compile failed for $RunLabel (exit=$compileExit)"
  }

  $backendPath = Join-Path $outDir "module.object-backend.txt"
  $manifestPath = Join-Path $outDir "module.manifest.json"
  $irPath = Join-Path $outDir "module.ll"
  $objPath = Join-Path $outDir "module.obj"
  foreach ($artifactPath in @($backendPath, $manifestPath, $irPath, $objPath)) {
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "execution replay proof FAIL: missing canonical runnable artifact $artifactPath"
    }
  }

  $backend = (Get-Content -LiteralPath $backendPath -Raw).Trim()
  if ($backend -ne "llvm-direct") {
    throw "execution replay proof FAIL: canonical runnable backend drift ($RunLabel backend=$backend)"
  }

  & $llvmReadobjCommand --sections $objPath *> $readobjLog
  $readobjExit = [int]$LASTEXITCODE
  if ($readobjExit -ne 0) {
    throw "execution replay proof FAIL: llvm-readobj --sections failed for $RunLabel (exit=$readobjExit)"
  }

  $sectionText = Get-NormalizedTextFromFile -Path $readobjLog
  $normalizedSectionText = Get-NormalizedReadobjSectionText -Text $sectionText
  $sectionNames = @(Get-ReadobjSectionNames -Text $sectionText)
  $missingSections = @($requiredRuntimeSections | Where-Object { $sectionNames -notcontains $_ })
  if ($missingSections.Count -gt 0) {
    throw "execution replay proof FAIL: canonical runnable object is missing required runtime sections for $RunLabel (missing=$($missingSections -join '|'))"
  }

  return [ordered]@{
    out_dir = Get-RepoRelativePath -Path $outDir -Root $repoRoot
    compile_log = Get-RepoRelativePath -Path $compileLog -Root $repoRoot
    backend = $backend
    manifest = Get-RepoRelativePath -Path $manifestPath -Root $repoRoot
    ir = Get-RepoRelativePath -Path $irPath -Root $repoRoot
    object = Get-RepoRelativePath -Path $objPath -Root $repoRoot
    readobj_sections_log = Get-RepoRelativePath -Path $readobjLog -Root $repoRoot
    ir_sha256 = Get-Sha256HexFromText -Text (Get-NormalizedTextFromFile -Path $irPath)
    object_sha256 = Get-Sha256HexFromFile -Path $objPath
    section_inspection_sha256 = Get-Sha256HexFromText -Text $normalizedSectionText
    section_names = @($sectionNames)
  }
}

function Invoke-SmokeRun {
  param(
    [Parameter(Mandatory = $true)][string]$RunId,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousRunId = $env:OBJC3C_NATIVE_EXECUTION_RUN_ID
  try {
    $env:OBJC3C_NATIVE_EXECUTION_RUN_ID = $RunId
    & $smokeScript *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    if ($null -eq $previousRunId) {
      $env:OBJC3C_NATIVE_EXECUTION_RUN_ID = $null
    }
    else {
      $env:OBJC3C_NATIVE_EXECUTION_RUN_ID = $previousRunId
    }
  }
}

function Get-CanonicalSummaryJson {
  param([Parameter(Mandatory = $true)][string]$SummaryPath)

  if (!(Test-Path -LiteralPath $SummaryPath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing summary $SummaryPath"
  }

  $summary = Get-Content -LiteralPath $SummaryPath -Raw | ConvertFrom-Json
  $results = @(
    $summary.results |
      Sort-Object -Property kind, fixture |
      ForEach-Object {
        [ordered]@{
          kind = $_.kind
          fixture = $_.fixture
          expectation = $_.expectation
          stage = if ($_.PSObject.Properties.Name -contains "stage") { $_.stage } else { "" }
          requires_live_runtime_dispatch = if ($_.PSObject.Properties.Name -contains "requires_live_runtime_dispatch") { [bool]$_.requires_live_runtime_dispatch } else { $false }
          runtime_dispatch_symbol = if ($_.PSObject.Properties.Name -contains "runtime_dispatch_symbol") { "$($_.runtime_dispatch_symbol)" } else { "" }
          compile_exit = [int]$_.compile_exit
          link_exit = [int]$_.link_exit
          run_exit = if ($_.PSObject.Properties.Name -contains "run_exit") { [int]$_.run_exit } else { -1 }
          expected_exit = if ($_.PSObject.Properties.Name -contains "expected_exit") { [int]$_.expected_exit } else { -1 }
          required_link_tokens = if ($_.PSObject.Properties.Name -contains "required_link_tokens") { @($_.required_link_tokens) } else { @() }
          missing_link_tokens = if ($_.PSObject.Properties.Name -contains "missing_link_tokens") { @($_.missing_link_tokens) } else { @() }
          passed = [bool]$_.passed
        }
      }
  )

  $canonical = [ordered]@{
    native_exe = if ($summary.PSObject.Properties.Name -contains "native_exe") { "$($summary.native_exe)" } else { "" }
    runtime_library = if ($summary.PSObject.Properties.Name -contains "runtime_library") { "$($summary.runtime_library)" } else { "" }
    compatibility_runtime_shim = if ($summary.PSObject.Properties.Name -contains "compatibility_runtime_shim") { "$($summary.compatibility_runtime_shim)" } else { "" }
    live_runtime_dispatch_default_symbol = if ($summary.PSObject.Properties.Name -contains "live_runtime_dispatch_default_symbol") { "$($summary.live_runtime_dispatch_default_symbol)" } else { "" }
    clang = "$($summary.clang)"
    total = [int]$summary.total
    passed = [int]$summary.passed
    failed = [int]$summary.failed
    status = "$($summary.status)"
    results = $results
  }
  return ($canonical | ConvertTo-Json -Depth 8 -Compress)
}

New-Item -ItemType Directory -Force -Path $proofDir | Out-Null
Push-Location $repoRoot
try {
  $run1Log = Join-Path $proofDir "run1.log"
  $run2Log = Join-Path $proofDir "run2.log"
  $run1Exit = Invoke-SmokeRun -RunId $run1Id -LogPath $run1Log
  if ($run1Exit -ne 0) {
    throw "execution replay proof FAIL: run1 failed with exit $run1Exit"
  }
  $run2Exit = Invoke-SmokeRun -RunId $run2Id -LogPath $run2Log
  if ($run2Exit -ne 0) {
    throw "execution replay proof FAIL: run2 failed with exit $run2Exit"
  }

  $run1Dir = Join-Path $executionRoot $run1Id
  $run2Dir = Join-Path $executionRoot $run2Id
  $run1SummaryPath = Join-Path $run1Dir "summary.json"
  $run2SummaryPath = Join-Path $run2Dir "summary.json"
  $run1Canonical = Get-CanonicalSummaryJson -SummaryPath $run1SummaryPath
  $run2Canonical = Get-CanonicalSummaryJson -SummaryPath $run2SummaryPath
  $run1Hash = Get-Sha256HexFromText -Text $run1Canonical
  $run2Hash = Get-Sha256HexFromText -Text $run2Canonical
  if ($run1Hash -ne $run2Hash) {
    throw "execution replay proof FAIL: canonical summary drift across replay (run1=$run1Hash run2=$run2Hash)"
  }

  $nativeExeForReplay = Resolve-NativeExeForReplay -SummaryPath $run1SummaryPath
  $canonicalRun1 = Invoke-CanonicalRunnableReplayArtifact -RunLabel "run1" -NativeExe $nativeExeForReplay
  $canonicalRun2 = Invoke-CanonicalRunnableReplayArtifact -RunLabel "run2" -NativeExe $nativeExeForReplay
  if ($canonicalRun1.ir_sha256 -ne $canonicalRun2.ir_sha256) {
    throw "execution replay proof FAIL: canonical runnable IR drift across replay (run1=$($canonicalRun1.ir_sha256) run2=$($canonicalRun2.ir_sha256))"
  }
  if ($canonicalRun1.object_sha256 -ne $canonicalRun2.object_sha256) {
    throw "execution replay proof FAIL: canonical runnable object drift across replay (run1=$($canonicalRun1.object_sha256) run2=$($canonicalRun2.object_sha256))"
  }
  if ($canonicalRun1.section_inspection_sha256 -ne $canonicalRun2.section_inspection_sha256) {
    throw "execution replay proof FAIL: canonical runnable metadata inspection drift across replay (run1=$($canonicalRun1.section_inspection_sha256) run2=$($canonicalRun2.section_inspection_sha256))"
  }
  if (($canonicalRun1.section_names -join "|") -ne ($canonicalRun2.section_names -join "|")) {
    throw "execution replay proof FAIL: canonical runnable section inventory drift across replay"
  }

  $summary = [ordered]@{
    proof_run_id = $proofRunId
    run1_id = $run1Id
    run2_id = $run2Id
    run1_summary = Get-RepoRelativePath -Path $run1SummaryPath -Root $repoRoot
    run2_summary = Get-RepoRelativePath -Path $run2SummaryPath -Root $repoRoot
    run1_sha256 = $run1Hash
    run2_sha256 = $run2Hash
    canonical_runnable_replay = [ordered]@{
      fixture = Get-RepoRelativePath -Path $canonicalRunnableFixture -Root $repoRoot
      native_exe = Get-RepoRelativePath -Path $nativeExeForReplay -Root $repoRoot
      required_runtime_sections = @($requiredRuntimeSections)
      run1 = $canonicalRun1
      run2 = $canonicalRun2
      ir_sha256 = $canonicalRun1.ir_sha256
      object_sha256 = $canonicalRun1.object_sha256
      section_inspection_sha256 = $canonicalRun1.section_inspection_sha256
      status = "PASS"
    }
    status = "PASS"
  }
  $summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $summaryPath -Encoding utf8
  Write-Output "run1_sha256: $run1Hash"
  Write-Output "run2_sha256: $run2Hash"
  Write-Output "canonical_runnable_ir_sha256: $($canonicalRun1.ir_sha256)"
  Write-Output "canonical_runnable_object_sha256: $($canonicalRun1.object_sha256)"
  Write-Output "canonical_runnable_section_inspection_sha256: $($canonicalRun1.section_inspection_sha256)"
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
  Write-Output "status: PASS"
}
finally {
  Pop-Location
}
