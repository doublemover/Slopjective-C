$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/driver-shell-split"
$configuredRunId = $env:OBJC3C_DRIVER_SHELL_SPLIT_RUN_ID
$runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$runDirRel = "tmp/artifacts/objc3c-native/driver-shell-split/$runId"
$summaryRel = "$runDirRel/summary.json"

$mainSourcePath = Join-Path $repoRoot "native/objc3c/src/main.cpp"
$driverMainHeaderPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_driver_main.h"
$driverMainImplPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_driver_main.cpp"
$driverHeaderPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_compilation_driver.h"
$driverImplPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_compilation_driver.cpp"
$cliOptionsHeaderPath = Join-Path $repoRoot "native/objc3c/src/driver/objc3_cli_options.h"
$buildScriptPath = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
$nativeExePath = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExePath } else { $configuredNativeExe }
$nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)
$fixturePath = Join-Path $repoRoot "tests/tooling/fixtures/native/driver_split/smoke_compile_driver_shell_split.objc3"

$checks = New-Object 'System.Collections.Generic.List[object]'
$hadFatalError = $false
$fatalErrorMessage = ""
$smokeCompile = $null

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $pathText = "$Path"
  $rootText = "$Root"
  $fullPath = [System.IO.Path]::GetFullPath($pathText)
  $fullRoot = [System.IO.Path]::GetFullPath($rootText)
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart([char[]]@('\', '/')).Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
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

function Add-Check {
  param(
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][string]$Detail,
    [Parameter()][object]$Evidence = $null
  )

  $checks.Add([pscustomobject]@{
      id = $Id
      passed = $Passed
      detail = $Detail
      evidence = $Evidence
    }) | Out-Null
}

function Assert-Contract {
  param(
    [Parameter(Mandatory = $true)][bool]$Condition,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage,
    [Parameter()][object]$Evidence = $null
  )

  if (-not $Condition) {
    Add-Check -Id $Id -Passed $false -Detail $FailureMessage -Evidence $Evidence
    throw "driver shell split FAIL: $FailureMessage"
  }
  Add-Check -Id $Id -Passed $true -Detail $PassMessage -Evidence $Evidence
}

function Read-NormalizedText {
  param([Parameter(Mandatory = $true)][string]$Path)

  $text = Get-Content -LiteralPath $Path -Raw
  $text = $text -replace "`r`n", "`n"
  $text = $text -replace "`r", "`n"
  return $text
}

function Get-FileSha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Assert-FileExists {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$Description
  )

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $repoRoot
  Assert-Contract `
    -Condition $exists `
    -Id $Id `
    -FailureMessage "missing $Description at $relative" `
    -PassMessage "found $Description at $relative" `
    -Evidence @{ path = $relative }
}

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

Push-Location $repoRoot
try {
  Assert-FileExists -Path $mainSourcePath -Id "source.main.exists" -Description "main source"
  Assert-FileExists -Path $driverMainHeaderPath -Id "source.driver_main_header.exists" -Description "driver main header"
  Assert-FileExists -Path $driverMainImplPath -Id "source.driver_main_impl.exists" -Description "driver main implementation"
  Assert-FileExists -Path $driverHeaderPath -Id "source.driver_header.exists" -Description "driver header"
  Assert-FileExists -Path $driverImplPath -Id "source.driver_impl.exists" -Description "driver implementation"
  Assert-FileExists -Path $cliOptionsHeaderPath -Id "source.cli_options_header.exists" -Description "cli options header"
  Assert-FileExists -Path $fixturePath -Id "fixture.driver_split_smoke.exists" -Description "driver split smoke fixture"

  $mainText = Read-NormalizedText -Path $mainSourcePath
  $driverMainHeaderText = Read-NormalizedText -Path $driverMainHeaderPath
  $driverMainImplText = Read-NormalizedText -Path $driverMainImplPath
  $driverHeaderText = Read-NormalizedText -Path $driverHeaderPath
  $driverImplText = Read-NormalizedText -Path $driverImplPath
  $cliOptionsHeaderText = Read-NormalizedText -Path $cliOptionsHeaderPath

  $projectIncludes = @(
    [regex]::Matches($mainText, '(?m)^\s*#\s*include\s+"([^"]+)"\s*$') |
      ForEach-Object { $_.Groups[1].Value }
  )
  $sortedProjectIncludes = @($projectIncludes | Sort-Object)
  $expectedProjectIncludes = @(
    "driver/objc3_driver_main.h"
  )
  $sortedExpectedProjectIncludes = @($expectedProjectIncludes | Sort-Object)
  $includeSetMatches = (($sortedProjectIncludes -join "|") -eq ($sortedExpectedProjectIncludes -join "|"))
  Assert-Contract `
    -Condition $includeSetMatches `
    -Id "contract.main.project_includes" `
    -FailureMessage ("main.cpp project includes mismatch: expected={0} actual={1}" -f ($sortedExpectedProjectIncludes -join ","), ($sortedProjectIncludes -join ",")) `
    -PassMessage "main.cpp includes only the driver main shell boundary header" `
    -Evidence @{
      expected = $sortedExpectedProjectIncludes
      actual = $sortedProjectIncludes
    }

  $mainHasOnlyDriverIncludes = @($projectIncludes | Where-Object { $_ -notmatch '^driver/' }).Count -eq 0
  Assert-Contract `
    -Condition $mainHasOnlyDriverIncludes `
    -Id "contract.main.includes_are_driver_only" `
    -FailureMessage "main.cpp includes non-driver project headers" `
    -PassMessage "main.cpp project include boundary stays inside driver/*"

  $flowPattern = '(?s)int\s+main\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*\)\s*\{\s*return\s+RunObjc3DriverMain\(argc,\s*argv\);\s*\}'
  Assert-Contract `
    -Condition ([regex]::IsMatch($mainText, $flowPattern)) `
    -Id "contract.main.delegates_to_driver_main" `
    -FailureMessage "main.cpp no longer delegates directly to RunObjc3DriverMain(argc, argv)" `
    -PassMessage "main.cpp delegates to driver main shell entrypoint"

  $forbiddenMainTokens = @(
    "ParseObjc3CliOptions(",
    "ApplyObjc3LLVMCabilityRouting(",
    "RunObjc3CompilationDriver(",
    "CompileObjc3SourceForCli(",
    "RunObjectiveCCompile(",
    "WriteManifestArtifact(",
    "WriteDiagnosticsArtifacts(",
    "RunIRCompile("
  )
  $mainForbiddenHits = @($forbiddenMainTokens | Where-Object { $mainText.IndexOf($_, [System.StringComparison]::Ordinal) -ge 0 })
  Assert-Contract `
    -Condition ($mainForbiddenHits.Count -eq 0) `
    -Id "contract.main.no_compilation_pipeline_calls" `
    -FailureMessage ("main.cpp contains compilation pipeline tokens: {0}" -f ($mainForbiddenHits -join ",")) `
    -PassMessage "main.cpp remains a shell and does not invoke compilation pipeline internals directly" `
    -Evidence @{ forbidden_hits = $mainForbiddenHits }

  Assert-Contract `
    -Condition ([regex]::IsMatch($driverMainHeaderText, '(?m)^\s*int\s+RunObjc3DriverMain\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*\)\s*;\s*$')) `
    -Id "contract.driver_main_header.entry_signature" `
    -FailureMessage "driver main header missing RunObjc3DriverMain(int argc, char **argv) declaration" `
    -PassMessage "driver main header exports RunObjc3DriverMain shell entrypoint"

  Assert-Contract `
    -Condition ([regex]::IsMatch($driverMainImplText, '(?m)^\s*#\s*include\s+"driver/objc3_cli_options.h"\s*$')) `
    -Id "contract.driver_main_impl.imports_cli_options" `
    -FailureMessage "driver main implementation missing include for driver/objc3_cli_options.h" `
    -PassMessage "driver main implementation imports cli options contract surface"

  Assert-Contract `
    -Condition ([regex]::IsMatch($driverMainImplText, '(?m)^\s*#\s*include\s+"driver/objc3_compilation_driver.h"\s*$')) `
    -Id "contract.driver_main_impl.imports_compilation_driver" `
    -FailureMessage "driver main implementation missing include for driver/objc3_compilation_driver.h" `
    -PassMessage "driver main implementation imports compilation driver boundary"

  Assert-Contract `
    -Condition ([regex]::IsMatch($driverMainImplText, '(?m)^\s*#\s*include\s+"driver/objc3_llvm_capability_routing.h"\s*$')) `
    -Id "contract.driver_main_impl.imports_llvm_routing" `
    -FailureMessage "driver main implementation missing include for driver/objc3_llvm_capability_routing.h" `
    -PassMessage "driver main implementation imports LLVM capability routing boundary"

  $driverMainFlowPattern = '(?s)int\s+RunObjc3DriverMain\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*\)\s*\{\s*Objc3CliOptions\s+cli_options;\s*std::string\s+cli_error;\s*if\s*\(!ParseObjc3CliOptions\(argc,\s*argv,\s*cli_options,\s*cli_error\)\)\s*\{\s*std::cerr\s*<<\s*cli_error\s*<<\s*"\\n";\s*return\s+2;\s*\}\s*if\s*\(!ApplyObjc3LLVMCabilityRouting\(cli_options,\s*cli_error\)\)\s*\{\s*std::cerr\s*<<\s*cli_error\s*<<\s*"\\n";\s*return\s+2;\s*\}\s*return\s+RunObjc3CompilationDriver\(cli_options\);\s*\}'
  Assert-Contract `
    -Condition ([regex]::IsMatch($driverMainImplText, $driverMainFlowPattern)) `
    -Id "contract.driver_main_impl.parse_route_delegate_flow" `
    -FailureMessage "driver main implementation no longer matches parse+route fail-exit(2) then RunObjc3CompilationDriver delegation contract" `
    -PassMessage "driver main implementation parse/route/delegate flow matches driver shell split contract"

  $driverMainDefinesMain = [regex]::IsMatch($driverMainImplText, '(?m)^\s*int\s+main\s*\(')
  Assert-Contract `
    -Condition (-not $driverMainDefinesMain) `
    -Id "contract.driver_main_impl.no_main_definition" `
    -FailureMessage "driver main implementation defines main(), violating main/driver split boundary" `
    -PassMessage "driver main implementation does not define main()"

  Assert-Contract `
    -Condition ([regex]::IsMatch($driverHeaderText, '(?m)^\s*#\s*include\s+"driver/objc3_cli_options.h"\s*$')) `
    -Id "contract.driver_header.imports_cli_options" `
    -FailureMessage "driver header missing include for driver/objc3_cli_options.h" `
    -PassMessage "driver header imports cli options contract surface"

  Assert-Contract `
    -Condition ([regex]::IsMatch($driverHeaderText, '(?m)^\s*int\s+RunObjc3CompilationDriver\s*\(\s*const\s+Objc3CliOptions\s*&\s*cli_options\s*\)\s*;\s*$')) `
    -Id "contract.driver_header.run_signature" `
    -FailureMessage "driver header missing RunObjc3CompilationDriver(const Objc3CliOptions&) declaration" `
    -PassMessage "driver header exports RunObjc3CompilationDriver contract signature"

  $driverDefinesMain = [regex]::IsMatch($driverImplText, '(?m)^\s*int\s+main\s*\(')
  Assert-Contract `
    -Condition (-not $driverDefinesMain) `
    -Id "contract.driver_impl.no_main_definition" `
    -FailureMessage "driver implementation defines main(), violating main/driver split boundary" `
    -PassMessage "driver implementation does not define main()"

  $driverExportsEntry = [regex]::IsMatch($driverImplText, '(?m)^\s*int\s+RunObjc3CompilationDriver\s*\(\s*const\s+Objc3CliOptions\s*&\s*cli_options\s*\)\s*\{')
  Assert-Contract `
    -Condition $driverExportsEntry `
    -Id "contract.driver_impl.run_entry_exists" `
    -FailureMessage "driver implementation missing RunObjc3CompilationDriver entrypoint" `
    -PassMessage "driver implementation exposes RunObjc3CompilationDriver entrypoint"

  $cliOptionsHasParseDecl = [regex]::IsMatch($cliOptionsHeaderText, '(?m)^\s*bool\s+ParseObjc3CliOptions\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*,\s*Objc3CliOptions\s*&\s*options\s*,\s*std::string\s*&\s*error\s*\)\s*;\s*$')
  Assert-Contract `
    -Condition $cliOptionsHasParseDecl `
    -Id "contract.cli_options.parse_signature" `
    -FailureMessage "cli options header missing ParseObjc3CliOptions(argc, argv, options, error) declaration" `
    -PassMessage "cli options header exports parse signature consumed by shell"

  if (-not $nativeExeExplicit -and !(Test-Path -LiteralPath $nativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "smoke.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $repoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $repoRoot) }
  }

  Assert-FileExists -Path $nativeExePath -Id "smoke.native_executable.exists" -Description "native executable"

  $parseProbeLog = Join-Path $runDir "parse_probe.log"
  $parseProbeExit = Invoke-LoggedCommand `
    -Command $nativeExePath `
    -Arguments @($fixturePath, "--driver-shell-split-invalid-flag") `
    -LogPath $parseProbeLog
  $parseProbeText = if (Test-Path -LiteralPath $parseProbeLog -PathType Leaf) { Read-NormalizedText -Path $parseProbeLog } else { "" }
  $parseProbeToken = "unknown arg: --driver-shell-split-invalid-flag"
  Assert-Contract `
    -Condition ($parseProbeExit -eq 2) `
    -Id "smoke.parse_probe.exit_code" `
    -FailureMessage ("parse probe expected exit=2 got exit={0}" -f $parseProbeExit) `
    -PassMessage "parse probe enforces shell parse failure exit mapping (exit=2)" `
    -Evidence @{ exit_code = $parseProbeExit; log = (Get-RepoRelativePath -Path $parseProbeLog -Root $repoRoot) }
  Assert-Contract `
    -Condition ($parseProbeText.IndexOf($parseProbeToken, [System.StringComparison]::Ordinal) -ge 0) `
    -Id "smoke.parse_probe.diagnostic_token" `
    -FailureMessage ("parse probe missing diagnostic token '{0}'" -f $parseProbeToken) `
    -PassMessage "parse probe emits deterministic unknown-arg diagnostic token"

  $compileRun1Dir = Join-Path $runDir "compile_run1"
  $compileRun2Dir = Join-Path $runDir "compile_run2"
  New-Item -ItemType Directory -Force -Path $compileRun1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $compileRun2Dir | Out-Null
  $compileRun1Log = Join-Path $runDir "compile_run1.log"
  $compileRun2Log = Join-Path $runDir "compile_run2.log"

  $compileArgsRun1 = @($fixturePath, "--out-dir", $compileRun1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $compileArgsRun2 = @($fixturePath, "--out-dir", $compileRun2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $compileRun1Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $compileArgsRun1 -LogPath $compileRun1Log
  $compileRun2Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $compileArgsRun2 -LogPath $compileRun2Log

  Assert-Contract `
    -Condition ($compileRun1Exit -eq 0) `
    -Id "smoke.compile_run1.exit_code" `
    -FailureMessage ("compile run1 failed with exit={0}" -f $compileRun1Exit) `
    -PassMessage "compile run1 succeeded" `
    -Evidence @{ exit_code = $compileRun1Exit; log = (Get-RepoRelativePath -Path $compileRun1Log -Root $repoRoot) }
  Assert-Contract `
    -Condition ($compileRun2Exit -eq 0) `
    -Id "smoke.compile_run2.exit_code" `
    -FailureMessage ("compile run2 failed with exit={0}" -f $compileRun2Exit) `
    -PassMessage "compile run2 succeeded" `
    -Evidence @{ exit_code = $compileRun2Exit; log = (Get-RepoRelativePath -Path $compileRun2Log -Root $repoRoot) }

  $expectedArtifacts = @(
    "module.manifest.json",
    "module.diagnostics.txt",
    "module.ll",
    "module.obj",
    "module.object-backend.txt"
  )

  $artifactDigests = [ordered]@{}
  foreach ($artifactName in $expectedArtifacts) {
    $run1ArtifactPath = Join-Path $compileRun1Dir $artifactName
    $run2ArtifactPath = Join-Path $compileRun2Dir $artifactName
    $run1Exists = Test-Path -LiteralPath $run1ArtifactPath -PathType Leaf
    $run2Exists = Test-Path -LiteralPath $run2ArtifactPath -PathType Leaf
    Assert-Contract `
      -Condition ($run1Exists -and $run2Exists) `
      -Id ("smoke.artifact.exists.{0}" -f $artifactName) `
      -FailureMessage ("expected artifact missing across replay for {0}" -f $artifactName) `
      -PassMessage ("artifact present across replay: {0}" -f $artifactName)

    if ($artifactName -eq "module.obj") {
      $run1Size = (Get-Item -LiteralPath $run1ArtifactPath).Length
      $run2Size = (Get-Item -LiteralPath $run2ArtifactPath).Length
      Assert-Contract `
        -Condition ($run1Size -gt 0 -and $run2Size -gt 0) `
        -Id "smoke.artifact.nonempty.module.obj" `
        -FailureMessage "module.obj is empty in one or more compile runs" `
        -PassMessage "module.obj is non-empty across replay" `
        -Evidence @{ run1_bytes = $run1Size; run2_bytes = $run2Size }
    }

    $run1Hash = Get-FileSha256Hex -Path $run1ArtifactPath
    $run2Hash = Get-FileSha256Hex -Path $run2ArtifactPath
    $artifactDigests[$artifactName] = [ordered]@{
      run1_sha256 = $run1Hash
      run2_sha256 = $run2Hash
      deterministic = ($run1Hash -eq $run2Hash)
    }
    if ($artifactName -eq "module.obj") {
      # COFF object output can embed non-deterministic metadata in this environment.
      # Keep a non-empty size gate for module.obj and treat hash capture as informational.
      Add-Check `
        -Id "smoke.artifact.hash_recorded.module.obj" `
        -Passed $true `
        -Detail "module.obj hashes captured for diagnostics; determinism is not enforced in this gate" `
        -Evidence @{ run1_sha256 = $run1Hash; run2_sha256 = $run2Hash }
      continue
    }
    Assert-Contract `
      -Condition ($run1Hash -eq $run2Hash) `
      -Id ("smoke.artifact.deterministic_sha256.{0}" -f $artifactName) `
      -FailureMessage ("artifact hash drift across replay for {0}" -f $artifactName) `
      -PassMessage ("artifact hash stable across replay: {0}" -f $artifactName) `
      -Evidence @{ run1_sha256 = $run1Hash; run2_sha256 = $run2Hash }
  }

  $llText = Read-NormalizedText -Path (Join-Path $compileRun1Dir "module.ll")
  Assert-Contract `
    -Condition ($llText.IndexOf("define i32 @objc3c_entry", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "smoke.ll.contains_objc3c_entry" `
    -FailureMessage "module.ll missing objc3c_entry symbol in smoke compile" `
    -PassMessage "module.ll contains objc3c_entry entrypoint marker"

  $diagText = Read-NormalizedText -Path (Join-Path $compileRun1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($diagText)) `
    -Id "smoke.diagnostics.empty_on_success" `
    -FailureMessage "smoke compile emitted diagnostics for successful compile path" `
    -PassMessage "smoke compile diagnostics artifact is empty on success"

  $smokeCompile = [ordered]@{
    fixture = Get-RepoRelativePath -Path $fixturePath -Root $repoRoot
    compile_run1_exit = $compileRun1Exit
    compile_run2_exit = $compileRun2Exit
    compile_run1_dir = Get-RepoRelativePath -Path $compileRun1Dir -Root $repoRoot
    compile_run2_dir = Get-RepoRelativePath -Path $compileRun2Dir -Root $repoRoot
    artifact_digests = $artifactDigests
  }
}
catch {
  $hadFatalError = $true
  $fatalErrorMessage = $_.Exception.Message
  Write-Output ("error: {0}" -f $fatalErrorMessage)
}
finally {
  Pop-Location
}

$checkArray = $checks.ToArray()
$totalChecks = $checkArray.Count
$passedChecks = @($checkArray | Where-Object { $_.passed }).Count
$failedChecks = $totalChecks - $passedChecks
$status = if (-not $hadFatalError -and $totalChecks -gt 0 -and $failedChecks -eq 0) { "PASS" } else { "FAIL" }
$nativeExecutableSummary = "$nativeExePath"
if (Test-Path -LiteralPath $nativeExePath -PathType Leaf) {
  try {
    $nativeExecutableSummary = Get-RepoRelativePath -Path $nativeExePath -Root $repoRoot
  }
  catch {
    $nativeExecutableSummary = "$nativeExePath"
  }
}

$summary = @{
  contract = "objc3c-driver-shell-split-v1"
  run_id = $runId
  run_dir = $runDirRel
  summary_path = $summaryRel
  native_executable = $nativeExecutableSummary
  status = $status
  total = $totalChecks
  passed = $passedChecks
  failed = $failedChecks
  fatal_error = $fatalErrorMessage
  checks = $checkArray
  smoke_compile = $smokeCompile
}
$summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output ("summary_path: {0}" -f $summaryRel)
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
