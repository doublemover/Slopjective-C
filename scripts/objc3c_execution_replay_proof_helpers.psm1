Set-StrictMode -Version Latest

function Get-ExecutionReplayProofRuntimeSections {
  return @(
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
}

function Get-ExecutionReplayProofCases {
  $requiredRuntimeSections = @(Get-ExecutionReplayProofRuntimeSections)

  return @(
    [ordered]@{
      case_id = "canonical-runnable"
      fixture = "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3"
      required_ll_tokens = @()
      required_runtime_sections = @($requiredRuntimeSections)
    },
    [ordered]@{
      case_id = "dispatch-fast-path"
      fixture = "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3"
      required_ll_tokens = @("@objc3_runtime_dispatch_i32")
      required_runtime_sections = @()
    },
    [ordered]@{
      case_id = "synthesized-accessor"
      fixture = "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
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
}

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

function Add-StageDuration {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][double]$DurationSeconds
  )

  if (-not $script:stageTimings.Contains($StageKey)) {
    $script:stageTimings[$StageKey] = 0.0
  }
  $script:stageTimings[$StageKey] = [double]$script:stageTimings[$StageKey] + $DurationSeconds
}

function Invoke-TimedLoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $exitCode = Invoke-LoggedCommand -Command $Command -Arguments $Arguments -LogPath $LogPath
  $stopwatch.Stop()
  $durationSeconds = [math]::Round($stopwatch.Elapsed.TotalSeconds, 6)
  Add-StageDuration -StageKey $StageKey -DurationSeconds $durationSeconds
  return [pscustomobject]@{
    exit_code = [int]$exitCode
    duration_seconds = $durationSeconds
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
    provenance_path = Get-RepoRelativePath -Path $provenancePath -Root $script:repoRoot
    registration_manifest_path = Get-RepoRelativePath -Path $registrationManifestPath -Root $script:repoRoot
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
  param(
    [Parameter(Mandatory = $true)][object[]]$Cases,
    [string]$CaseId = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

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
  $fixturePath = Join-Path $script:repoRoot ([string]$Case.fixture).Replace('/', '\\')
  if (!(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing fixture for $caseId at $fixturePath"
  }

  $artifactRoot = Join-Path $script:proofDir ("{0}_{1}" -f $caseId, $RunLabel)
  $outDir = Join-Path $artifactRoot "out"
  $compileLog = Join-Path $artifactRoot "compile.log"
  New-Item -ItemType Directory -Force -Path $artifactRoot | Out-Null
  New-Item -ItemType Directory -Force -Path $outDir | Out-Null

  if (!(Test-Path -LiteralPath $script:compileScript -PathType Leaf)) {
    throw "execution replay proof FAIL: compile wrapper missing at $script:compileScript"
  }
  $compileStep = Invoke-TimedLoggedCommand -StageKey "compile_seconds" -Command $script:shellCommand -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $script:compileScript, $fixturePath, "--out-dir", $outDir, "--emit-prefix", "module") -LogPath $compileLog
  $compileExit = [int]$compileStep.exit_code
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
    $readobjStep = Invoke-TimedLoggedCommand -StageKey "readobj_seconds" -Command $script:llvmReadobjCommand -Arguments @("--sections", $objPath) -LogPath $readobjLogPath
    $readobjExit = [int]$readobjStep.exit_code
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
    $readobjLog = Get-RepoRelativePath -Path $readobjLogPath -Root $script:repoRoot
  }

  return [ordered]@{
    fixture = Get-RepoRelativePath -Path $fixturePath -Root $script:repoRoot
    out_dir = Get-RepoRelativePath -Path $outDir -Root $script:repoRoot
    compile_log = Get-RepoRelativePath -Path $compileLog -Root $script:repoRoot
    manifest = Get-RepoRelativePath -Path $manifestPath -Root $script:repoRoot
    registration_manifest = [string]$provenance.registration_manifest_path
    compile_provenance = [string]$provenance.provenance_path
    diagnostics = Get-RepoRelativePath -Path $diagPath -Root $script:repoRoot
    ir = Get-RepoRelativePath -Path $irPath -Root $script:repoRoot
    object = Get-RepoRelativePath -Path $objPath -Root $script:repoRoot
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
    timing = [ordered]@{
      compile_seconds = [double]$compileStep.duration_seconds
      readobj_seconds = if ($requiredSections.Count -gt 0) { [double]$readobjStep.duration_seconds } else { 0.0 }
    }
  }
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
  $script:llvmReadobjCommand = if ([string]::IsNullOrWhiteSpace($script:configuredLlvmReadobj)) { "llvm-readobj" } else { $script:configuredLlvmReadobj }
}

function Invoke-Objc3cExecutionReplayProof {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$CaseId = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  $ErrorActionPreference = "Stop"
  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  Initialize-ExecutionReplayProofContext -ScriptRoot $ScriptRoot
  $proofCases = @(Get-ExecutionReplayProofCases)

  New-Item -ItemType Directory -Force -Path $script:proofDir | Out-Null
  Push-Location $script:repoRoot
  try {
    $suiteStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $script:stageTimings = [ordered]@{
      compile_seconds = 0.0
      readobj_seconds = 0.0
      comparison_seconds = 0.0
      output_report_seconds = 0.0
    }
    Ensure-NativeCompilerExecutable -NativeExePath $script:nativeExe -NativeExeExplicit $script:nativeExeExplicit -BuildScriptPath $script:buildScript

    $selectedProofCases = @(Select-ProofCases -Cases $proofCases -CaseId $CaseId -ShardIndex $ShardIndex -ShardCount $ShardCount -Limit $Limit)
    Write-Output ("selection: cases={0}" -f $selectedProofCases.Count)
    $caseSummaries = @()
    $caseTimings = @()
    $caseIndex = 0
    $lastCompletedCase = "none"
    foreach ($case in $selectedProofCases) {
      $caseIndex += 1
      $caseStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
      Write-Output ("execution-replay-progress: [{0}/{1}] START case={2} elapsed={3:n3}s last={4}" -f $caseIndex, $selectedProofCases.Count, $case.case_id, $suiteStopwatch.Elapsed.TotalSeconds, $lastCompletedCase)
      $run1 = Invoke-ReplayCompile -Case $case -RunLabel "run1" -NativeExePath $script:nativeExe
      $run2 = Invoke-ReplayCompile -Case $case -RunLabel "run2" -NativeExePath $script:nativeExe

      $comparisonStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
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
      $comparisonStopwatch.Stop()
      Add-StageDuration -StageKey "comparison_seconds" -DurationSeconds ([math]::Round($comparisonStopwatch.Elapsed.TotalSeconds, 6))
      $caseStopwatch.Stop()

      $caseSummaries += [ordered]@{
        case_id = [string]$case.case_id
        claim_class = "compile-coupled-replay-proof"
        run1 = $run1
        run2 = $run2
        timing = [ordered]@{
          duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
          run1_compile_seconds = [double]$run1.timing.compile_seconds
          run2_compile_seconds = [double]$run2.timing.compile_seconds
          run1_readobj_seconds = [double]$run1.timing.readobj_seconds
          run2_readobj_seconds = [double]$run2.timing.readobj_seconds
          comparison_seconds = [math]::Round($comparisonStopwatch.Elapsed.TotalSeconds, 6)
        }
        status = "PASS"
      }
      $caseTimings += [ordered]@{
        case_id = [string]$case.case_id
        duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
        compile_seconds = [math]::Round(([double]$run1.timing.compile_seconds + [double]$run2.timing.compile_seconds), 6)
        readobj_seconds = [math]::Round(([double]$run1.timing.readobj_seconds + [double]$run2.timing.readobj_seconds), 6)
        comparison_seconds = [math]::Round($comparisonStopwatch.Elapsed.TotalSeconds, 6)
      }
      $lastCompletedCase = [string]$case.case_id
      Write-Output ("execution-replay-progress: [{0}/{1}] DONE case={2} duration={3:n3}s elapsed={4:n3}s" -f $caseIndex, $selectedProofCases.Count, $case.case_id, $caseStopwatch.Elapsed.TotalSeconds, $suiteStopwatch.Elapsed.TotalSeconds)
    }

    $reportStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $summary = [ordered]@{
      proof_run_id = $script:proofRunId
      native_exe = if (Test-Path -LiteralPath $script:nativeExe -PathType Leaf) { Get-RepoRelativePath -Path $script:nativeExe -Root $script:repoRoot } else { $script:nativeExe }
      claim_boundary = [ordered]@{
        contract_id = "objc3c.runtime.execution.claim.boundary.v1"
        authoritative_claim_class = "compile-coupled-replay-proof"
        proof_corpus_model = "canonical-native-truth-corpus"
        canonical_case_ids = @($proofCases | ForEach-Object { [string]$_.case_id })
        execution_smoke_rerun_removed = $true
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
          "non-authoritative test surfaces without the emitted object and runtime-backed probe path",
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
      timing = [ordered]@{
        elapsed_seconds = [math]::Round($suiteStopwatch.Elapsed.TotalSeconds, 6)
        stage_totals = $script:stageTimings
        slowest_cases = @($caseTimings | Sort-Object -Property duration_seconds -Descending | Select-Object -First 10)
        case_timings = @($caseTimings)
      }
      status = "PASS"
    }
    $reportStopwatch.Stop()
    Add-StageDuration -StageKey "output_report_seconds" -DurationSeconds ([math]::Round($reportStopwatch.Elapsed.TotalSeconds, 6))
    $summary.timing.stage_totals = $script:stageTimings
    $summary.timing.elapsed_seconds = [math]::Round($suiteStopwatch.Elapsed.TotalSeconds, 6)
    $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $script:summaryPath -Encoding utf8
    Write-Output "summary_path: $(Get-RepoRelativePath -Path $script:summaryPath -Root $script:repoRoot)"
    Write-Output "status: PASS"
  }
  finally {
    Pop-Location
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cExecutionReplayProof"
)
