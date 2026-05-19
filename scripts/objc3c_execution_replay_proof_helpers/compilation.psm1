Set-StrictMode -Version Latest

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
