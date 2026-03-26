param(
  [string]$CaseId = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-replay-proof"
$proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$proofDir = Join-Path $proofRoot $proofRunId
$summaryPath = Join-Path $proofDir "summary.json"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$compileScript = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"
$defaultNativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
$nativeExe = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExe } else { $configuredNativeExe }
$nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)
$shellCommand = (Get-Process -Id $PID).Path
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

$proofCases = @(
  [ordered]@{
    case_id = "canonical-runnable"
    fixture = "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3"
    required_ll_tokens = @()
    required_runtime_sections = @($requiredRuntimeSections)
  },
  [ordered]@{
    case_id = "dispatch-fast-path"
    fixture = "tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3"
    required_ll_tokens = @("@objc3_runtime_dispatch_i32")
    required_runtime_sections = @()
  },
  [ordered]@{
    case_id = "synthesized-accessor"
    fixture = "tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3"
    required_ll_tokens = @(
      "define i32 @objc3_method_Widget_instance_count()",
      "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
      "define i1 @objc3_method_Widget_instance_enabled()",
      "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)"
    )
    required_runtime_sections = @()
  },
  [ordered]@{
    case_id = "metadata-sections"
    fixture = "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3"
    required_ll_tokens = @()
    required_runtime_sections = @($requiredRuntimeSections)
  }
)

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart([char[]]@([char]92, [char]47)).Replace('\\', '/')
  }
  return $fullPath.Replace('\\', '/')
}

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Ensure-NativeCompilerExecutable {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath
  )

  if (Test-Path -LiteralPath $NativeExePath -PathType Leaf) {
    return
  }
  if ($NativeExeExplicit) {
    throw "execution replay proof FAIL: configured native compiler missing at $NativeExePath"
  }
  if (!(Test-Path -LiteralPath $BuildScriptPath -PathType Leaf)) {
    throw "execution replay proof FAIL: native build script missing at $BuildScriptPath"
  }

  & $BuildScriptPath -ExecutionMode binaries-only | Out-Null
  if ($LASTEXITCODE -ne 0) {
    throw "execution replay proof FAIL: native compiler build failed with exit code $LASTEXITCODE"
  }
  if (!(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    throw "execution replay proof FAIL: native compiler executable missing at $NativeExePath"
  }
}

function Get-Sha256HexFromText {
  param([Parameter(Mandatory = $true)][AllowEmptyString()][string]$Text)

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

function Get-Sha256HexFromBytes {
  param([Parameter(Mandatory = $true)][byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
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
  return ("$((Get-Content -LiteralPath $Path -Raw))").Replace("`r`n", "`n")
}

function Read-JsonHashtable {
  param([Parameter(Mandatory = $true)][string]$Path)

  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json -AsHashtable)
}

function Assert-CompileOutputProvenance {
  param(
    [Parameter(Mandatory = $true)][string]$CaseId,
    [Parameter(Mandatory = $true)][string]$RunDir
  )

  $provenancePath = Join-Path $RunDir "module.compile-provenance.json"
  if (!(Test-Path -LiteralPath $provenancePath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing compile provenance artifact for $CaseId"
  }
  $registrationManifestPath = Join-Path $RunDir "module.runtime-registration-manifest.json"
  if (!(Test-Path -LiteralPath $registrationManifestPath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing runtime registration manifest for $CaseId"
  }

  $provenanceText = Get-NormalizedTextFromFile -Path $provenancePath
  if ([string]::IsNullOrWhiteSpace($provenanceText)) {
    throw "execution replay proof FAIL: empty compile provenance artifact for $CaseId"
  }

  $provenance = Read-JsonHashtable -Path $provenancePath
  $registrationManifest = Read-JsonHashtable -Path $registrationManifestPath
  if ([string]$provenance["contract_id"] -ne "objc3c.native.compile.output.provenance.v1") {
    throw "execution replay proof FAIL: unexpected compile provenance contract id for $CaseId"
  }
  if ([string]$registrationManifest["compile_output_provenance_artifact"] -ne "module.compile-provenance.json") {
    throw "execution replay proof FAIL: runtime registration manifest missing compile provenance binding for $CaseId"
  }

  $truthfulness = $provenance["compile_output_truthfulness"]
  if ($null -eq $truthfulness -or [string]$truthfulness["contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "execution replay proof FAIL: missing compile output truthfulness envelope for $CaseId"
  }
  if (-not [bool]$truthfulness["truthful"]) {
    throw "execution replay proof FAIL: compile output truthfulness envelope did not certify emitted artifacts for $CaseId"
  }
  if ([string]$registrationManifest["compile_output_truthfulness_contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "execution replay proof FAIL: runtime registration manifest missing compile output truthfulness contract id for $CaseId"
  }
  if (-not [bool]$registrationManifest["compile_output_truthful"]) {
    throw "execution replay proof FAIL: runtime registration manifest did not certify truthful compile output for $CaseId"
  }

  $artifactEntries = @($provenance["emitted_artifacts"])
  if ($artifactEntries.Count -lt 4) {
    throw "execution replay proof FAIL: compile provenance emitted_artifacts too small for $CaseId"
  }

  $digestLines = New-Object System.Collections.Generic.List[string]
  foreach ($entry in $artifactEntries) {
    $artifactPath = Join-Path $RunDir ([string]$entry["path"])
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "execution replay proof FAIL: compile provenance referenced missing artifact '$artifactPath' for $CaseId"
    }
    $actualHash = Get-Sha256HexFromFile -Path $artifactPath
    if ($actualHash -ne ([string]$entry["sha256"]).ToLowerInvariant()) {
      throw "execution replay proof FAIL: compile provenance sha256 mismatch for '$artifactPath' in $CaseId"
    }
    $actualSize = (Get-Item -LiteralPath $artifactPath).Length
    if ([long]$actualSize -ne [long]$entry["byte_count"]) {
      throw "execution replay proof FAIL: compile provenance byte_count mismatch for '$artifactPath' in $CaseId"
    }
    $digestLines.Add(("{0}|{1}|{2}" -f [string]$entry["path"], [string]$entry["byte_count"], [string]$entry["sha256"]))
  }

  $actualArtifactSetDigest = Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($digestLines -join "`n")))
  if ($actualArtifactSetDigest -ne ([string]$provenance["artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "execution replay proof FAIL: compile provenance artifact set digest mismatch for $CaseId"
  }
  if ($actualArtifactSetDigest -ne ([string]$registrationManifest["compile_output_artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "execution replay proof FAIL: runtime registration manifest compile output digest mismatch for $CaseId"
  }

  return [ordered]@{
    provenance_path = Get-RepoRelativePath -Path $provenancePath -Root $repoRoot
    registration_manifest_path = Get-RepoRelativePath -Path $registrationManifestPath -Root $repoRoot
    provenance_sha256 = Get-Sha256HexFromText -Text $provenanceText
    registration_manifest_sha256 = Get-Sha256HexFromText -Text (Get-NormalizedTextFromFile -Path $registrationManifestPath)
    artifact_set_digest_sha256 = $actualArtifactSetDigest
  }
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

function Assert-RequiredTextTokens {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$Tokens,
    [Parameter(Mandatory = $true)][string]$CaseId
  )

  foreach ($token in $Tokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "execution replay proof FAIL: missing required token '$token' for $CaseId"
    }
  }
}

function Select-ProofCases {
  param([object[]]$Cases)

  if ($Limit -lt 0) {
    throw "execution replay proof FAIL: limit must be non-negative"
  }
  if ($ShardCount -lt 0) {
    throw "execution replay proof FAIL: shard-count must be non-negative"
  }
  if (($ShardIndex -ge 0) -and ($ShardCount -le 0)) {
    throw "execution replay proof FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCount -gt 0) -and (($ShardIndex -lt 0) -or ($ShardIndex -ge $ShardCount))) {
    throw "execution replay proof FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
  }

  $selected = @($Cases)
  if (-not [string]::IsNullOrWhiteSpace($CaseId)) {
    $selected = @($selected | Where-Object { [string]$_.case_id -eq $CaseId })
    if ($selected.Count -eq 0) {
      throw "execution replay proof FAIL: no replay proof case matched case-id '$CaseId'"
    }
  }

  if ($ShardCount -gt 0) {
    $sharded = New-Object System.Collections.Generic.List[object]
    for ($index = 0; $index -lt $selected.Count; $index++) {
      if (($index % $ShardCount) -eq $ShardIndex) {
        $sharded.Add($selected[$index]) | Out-Null
      }
    }
    $selected = @($sharded)
  }

  if (($Limit -gt 0) -and ($selected.Count -gt $Limit)) {
    $selected = @($selected | Select-Object -First $Limit)
  }

  if ($selected.Count -eq 0) {
    throw "execution replay proof FAIL: no replay proof cases matched the requested selection"
  }

  return $selected
}

function Invoke-ReplayCompile {
  param(
    [Parameter(Mandatory = $true)][hashtable]$Case,
    [Parameter(Mandatory = $true)][string]$RunLabel,
    [Parameter(Mandatory = $true)][string]$NativeExePath
  )

  $caseId = [string]$Case.case_id
  $fixturePath = Join-Path $repoRoot ([string]$Case.fixture).Replace('/', '\\')
  if (!(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing fixture for $caseId at $fixturePath"
  }

  $artifactRoot = Join-Path $proofDir ("{0}_{1}" -f $caseId, $RunLabel)
  $outDir = Join-Path $artifactRoot "out"
  $compileLog = Join-Path $artifactRoot "compile.log"
  New-Item -ItemType Directory -Force -Path $artifactRoot | Out-Null
  New-Item -ItemType Directory -Force -Path $outDir | Out-Null

  if (!(Test-Path -LiteralPath $compileScript -PathType Leaf)) {
    throw "execution replay proof FAIL: compile wrapper missing at $compileScript"
  }
  $compileExit = Invoke-LoggedCommand -Command $shellCommand -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $compileScript, $fixturePath, "--out-dir", $outDir, "--emit-prefix", "module") -LogPath $compileLog
  if ($compileExit -ne 0) {
    throw "execution replay proof FAIL: compile failed for $caseId $RunLabel (exit=$compileExit)"
  }

  $manifestPath = Join-Path $outDir "module.manifest.json"
  $diagPath = Join-Path $outDir "module.diagnostics.txt"
  $irPath = Join-Path $outDir "module.ll"
  $objPath = Join-Path $outDir "module.obj"
  foreach ($artifactPath in @($manifestPath, $diagPath, $irPath, $objPath)) {
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "execution replay proof FAIL: missing replay artifact $artifactPath"
    }
  }
  $provenance = Assert-CompileOutputProvenance -CaseId $caseId -RunDir $outDir

  $llText = Get-NormalizedTextFromFile -Path $irPath
  $requiredLlTokens = @($Case.required_ll_tokens)
  if ($requiredLlTokens.Count -gt 0) {
    Assert-RequiredTextTokens -Text $llText -Tokens $requiredLlTokens -CaseId $caseId
  }

  $sectionNames = @()
  $sectionInspectionSha = ""
  $readobjLog = ""
  $requiredSections = @($Case.required_runtime_sections)
  if ($requiredSections.Count -gt 0) {
    $readobjLogPath = Join-Path $artifactRoot "readobj-sections.log"
    $readobjExit = Invoke-LoggedCommand -Command $llvmReadobjCommand -Arguments @("--sections", $objPath) -LogPath $readobjLogPath
    if ($readobjExit -ne 0) {
      throw "execution replay proof FAIL: llvm-readobj --sections failed for $caseId $RunLabel (exit=$readobjExit)"
    }
    $sectionText = Get-NormalizedTextFromFile -Path $readobjLogPath
    $sectionNames = @(Get-ReadobjSectionNames -Text $sectionText)
    $missingSections = @($requiredSections | Where-Object { $sectionNames -notcontains $_ })
    if ($missingSections.Count -gt 0) {
      throw "execution replay proof FAIL: missing runtime sections for $caseId $RunLabel (missing=$($missingSections -join '|'))"
    }
    $sectionInspectionSha = Get-Sha256HexFromText -Text (Get-NormalizedReadobjSectionText -Text $sectionText)
    $readobjLog = Get-RepoRelativePath -Path $readobjLogPath -Root $repoRoot
  }

  return [ordered]@{
    fixture = Get-RepoRelativePath -Path $fixturePath -Root $repoRoot
    out_dir = Get-RepoRelativePath -Path $outDir -Root $repoRoot
    compile_log = Get-RepoRelativePath -Path $compileLog -Root $repoRoot
    manifest = Get-RepoRelativePath -Path $manifestPath -Root $repoRoot
    registration_manifest = [string]$provenance.registration_manifest_path
    compile_provenance = [string]$provenance.provenance_path
    diagnostics = Get-RepoRelativePath -Path $diagPath -Root $repoRoot
    ir = Get-RepoRelativePath -Path $irPath -Root $repoRoot
    object = Get-RepoRelativePath -Path $objPath -Root $repoRoot
    readobj_sections_log = $readobjLog
    required_ll_tokens = @($requiredLlTokens)
    required_runtime_sections = @($requiredSections)
    manifest_sha256 = Get-Sha256HexFromText -Text (Get-NormalizedTextFromFile -Path $manifestPath)
    registration_manifest_sha256 = [string]$provenance.registration_manifest_sha256
    provenance_sha256 = [string]$provenance.provenance_sha256
    diagnostics_sha256 = Get-Sha256HexFromText -Text (Get-NormalizedTextFromFile -Path $diagPath)
    ir_sha256 = Get-Sha256HexFromText -Text $llText
    object_sha256 = Get-Sha256HexFromFile -Path $objPath
    artifact_set_digest_sha256 = [string]$provenance.artifact_set_digest_sha256
    section_inspection_sha256 = $sectionInspectionSha
    section_names = @($sectionNames)
  }
}

New-Item -ItemType Directory -Force -Path $proofDir | Out-Null
Push-Location $repoRoot
try {
  Ensure-NativeCompilerExecutable -NativeExePath $nativeExe -NativeExeExplicit $nativeExeExplicit -BuildScriptPath $buildScript

  $selectedProofCases = @(Select-ProofCases -Cases $proofCases)
  Write-Output ("selection: cases={0}" -f $selectedProofCases.Count)
  $caseSummaries = @()
  foreach ($case in $selectedProofCases) {
    $run1 = Invoke-ReplayCompile -Case $case -RunLabel "run1" -NativeExePath $nativeExe
    $run2 = Invoke-ReplayCompile -Case $case -RunLabel "run2" -NativeExePath $nativeExe

    foreach ($field in @("manifest_sha256", "registration_manifest_sha256", "provenance_sha256", "diagnostics_sha256", "ir_sha256", "object_sha256", "artifact_set_digest_sha256")) {
      if ([string]$run1[$field] -ne [string]$run2[$field]) {
        throw "execution replay proof FAIL: $field drift across replay for $($case.case_id) (run1=$($run1[$field]) run2=$($run2[$field]))"
      }
    }
    if ((@($case.required_runtime_sections)).Count -gt 0) {
      if ([string]$run1.section_inspection_sha256 -ne [string]$run2.section_inspection_sha256) {
        throw "execution replay proof FAIL: section inspection drift across replay for $($case.case_id)"
      }
      if (($run1.section_names -join "|") -ne ($run2.section_names -join "|")) {
        throw "execution replay proof FAIL: section inventory drift across replay for $($case.case_id)"
      }
    }

    $caseSummaries += [ordered]@{
      case_id = [string]$case.case_id
      claim_class = "compile-coupled-replay-proof"
      run1 = $run1
      run2 = $run2
      status = "PASS"
    }
  }

  $summary = [ordered]@{
    proof_run_id = $proofRunId
    native_exe = if (Test-Path -LiteralPath $nativeExe -PathType Leaf) { Get-RepoRelativePath -Path $nativeExe -Root $repoRoot } else { $nativeExe }
    claim_boundary = [ordered]@{
      contract_id = "objc3c.runtime.execution.claim.boundary.v1"
      authoritative_claim_class = "compile-coupled-replay-proof"
      compile_output_truthfulness_contract_id = "objc3c.native.compile.output.truthfulness.v1"
      registration_manifest_truth_required = $true
      authoritative_evidence = @(
        "emitted object coupled to compile provenance",
        "runtime registration manifest bound to the same artifact digest",
        "deterministic replay digest equality across two real compile runs",
        "runtime section inspection derived from the emitted object when required"
      )
      non_authoritative_inputs = @(
        "hand-authored llvm ir without the emitted object",
        "sidecar-only reports or manifests without coupled compile output",
        "compatibility shims without the emitted object and runtime-backed probe path",
        "replay text alone without compile provenance and registration-manifest coupling"
      )
    }
    selection = [ordered]@{
      case_id = $CaseId
      shard_index = $ShardIndex
      shard_count = $ShardCount
      limit = $Limit
      selected_cases = $selectedProofCases.Count
    }
    cases = $caseSummaries
    status = "PASS"
  }
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
  Write-Output "status: PASS"
}
finally {
  Pop-Location
}
