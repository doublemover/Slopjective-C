$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$defaultOutDir = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native"

function Show-UsageAndExit {
  Write-Error "usage: objc3c_native_compile.ps1 <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--use-cache]"
  exit 2
}

function Parse-WrapperArguments {
  param([string[]]$RawArgs)

  if ($RawArgs.Count -lt 1) {
    Show-UsageAndExit
  }

  $useCache = $false
  $compileArgs = New-Object System.Collections.Generic.List[string]
  $outDir = $null

  for ($i = 0; $i -lt $RawArgs.Count; $i++) {
    $token = $RawArgs[$i]
    if ($token -eq "--use-cache") {
      $useCache = $true
      continue
    }

    $compileArgs.Add($token)
    if ($token -eq "--out-dir") {
      if (($i + 1) -ge $RawArgs.Count) {
        Show-UsageAndExit
      }
      $i++
      $value = $RawArgs[$i]
      $compileArgs.Add($value)
      $outDir = $value
    }
  }

  if ($compileArgs.Count -lt 1) {
    Show-UsageAndExit
  }

  if ([string]::IsNullOrWhiteSpace($outDir)) {
    $outDir = $defaultOutDir
    $compileArgs.Add("--out-dir")
    $compileArgs.Add($outDir)
  }

  return [pscustomobject]@{
    use_cache = $useCache
    compile_args = $compileArgs.ToArray()
    out_dir = $outDir
  }
}

function Get-ArgsWithoutOutDir {
  param([string[]]$CompileArgs)

  $result = New-Object System.Collections.Generic.List[string]
  for ($i = 0; $i -lt $CompileArgs.Count; $i++) {
    $token = $CompileArgs[$i]
    if ($token -eq "--out-dir") {
      if (($i + 1) -ge $CompileArgs.Count) {
        break
      }
      $i++
      continue
    }
    $result.Add($token)
  }
  return $result.ToArray()
}

function Get-Sha256HexFromBytes {
  param([byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Get-FileSha256Hex {
  param([string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Get-OptionalFileHash {
  param([string]$Path)

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }
  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    return ""
  }
  return Get-FileSha256Hex -Path $Path
}

function Get-CacheKey {
  param(
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$CompilerSourcePath,
    [string]$WrapperScriptPath
  )

  if ([string]::IsNullOrWhiteSpace($InputPath)) {
    return $null
  }
  if (!(Test-Path -LiteralPath $InputPath -PathType Leaf)) {
    return $null
  }

  $inputHash = Get-FileSha256Hex -Path $InputPath
  $compilerSourceHash = Get-OptionalFileHash -Path $CompilerSourcePath
  $wrapperScriptHash = Get-OptionalFileHash -Path $WrapperScriptPath
  $payload = [ordered]@{
    version = 2
    input_sha256 = $inputHash
    compiler_source_sha256 = $compilerSourceHash
    wrapper_script_sha256 = $wrapperScriptHash
    args = $ArgsWithoutOutDir
  }
  $payloadJson = $payload | ConvertTo-Json -Compress -Depth 6
  $payloadBytes = [System.Text.Encoding]::UTF8.GetBytes($payloadJson)
  return Get-Sha256HexFromBytes -Bytes $payloadBytes
}

function Copy-DirectoryContents {
  param(
    [string]$SourceRoot,
    [string]$DestinationRoot
  )

  if (!(Test-Path -LiteralPath $SourceRoot -PathType Container)) {
    return
  }

  New-Item -ItemType Directory -Force -Path $DestinationRoot | Out-Null
  $resolvedSourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
  $files = Get-ChildItem -LiteralPath $SourceRoot -Recurse -File | Sort-Object -Property FullName

  foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($resolvedSourceRoot.Length).TrimStart('\', '/')
    $destination = Join-Path $DestinationRoot $relativePath
    $parent = Split-Path -Parent $destination
    if (![string]::IsNullOrWhiteSpace($parent)) {
      New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }
    Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
  }
}

function Invoke-BuildNativeCompiler {
  param([string]$RepoRoot)

  $buildScript = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"
  $buildOutput = @(& $buildScript)
  $buildOutputLines = New-Object System.Collections.Generic.List[string]
  $frontendScaffoldRelativePath = $null
  $frontendInvocationLockRelativePath = $null
  foreach ($line in $buildOutput) {
    $lineText = [string]$line
    $buildOutputLines.Add($lineText)
    if ($lineText.StartsWith("frontend_scaffold=")) {
      $frontendScaffoldRelativePath = $lineText.Substring("frontend_scaffold=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_invocation_lock=")) {
      $frontendInvocationLockRelativePath = $lineText.Substring("frontend_invocation_lock=".Length).Trim()
    }
  }
  return [pscustomobject]@{
    exit_code = [int]$LASTEXITCODE
    build_output_lines = $buildOutputLines.ToArray()
    frontend_scaffold_relative_path = $frontendScaffoldRelativePath
    frontend_invocation_lock_relative_path = $frontendInvocationLockRelativePath
  }
}

function Invoke-NativeCompiler {
  param(
    [string]$ExePath,
    [string[]]$Arguments
  )

  $process = Start-Process -FilePath $ExePath -ArgumentList $Arguments -NoNewWindow -Wait -PassThru
  return [int]$process.ExitCode
}

function Resolve-FrontendScaffoldPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_scaffold_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  if ([System.IO.Path]::IsPathRooted($relativePath)) {
    return $relativePath
  }
  return Join-Path $RepoRoot $relativePath
}

function Resolve-FrontendInvocationLockPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_invocation_lock_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  if ([System.IO.Path]::IsPathRooted($relativePath)) {
    return $relativePath
  }
  return Join-Path $RepoRoot $relativePath
}

function Assert-FrontendModuleScaffold {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend modular scaffold artifact missing at $scaffoldPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $scaffoldPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend modular scaffold artifact is not valid JSON at $scaffoldPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend modular scaffold contract id mismatch in $scaffoldPath"
    exit 2
  }

  $modules = @($payload.modules)
  $requiredModules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
  $presentModules = @{}
  foreach ($module in $modules) {
    $moduleName = [string]$module.name
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      Write-Error "frontend modular scaffold has module with missing name in $scaffoldPath"
      exit 2
    }
    $moduleSources = @($module.sources)
    if ($moduleSources.Count -eq 0) {
      Write-Error "frontend modular scaffold module '$moduleName' has no sources in $scaffoldPath"
      exit 2
    }
    $presentModules[$moduleName] = $true
  }
  foreach ($moduleName in $requiredModules) {
    if (-not $presentModules.ContainsKey($moduleName)) {
      Write-Error "frontend modular scaffold missing required module '$moduleName' in $scaffoldPath"
      exit 2
    }
  }

  $sharedSources = @($payload.shared_sources)
  if ($sharedSources.Count -eq 0) {
    Write-Error "frontend modular scaffold shared_sources must be non-empty in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.module_count -ne $modules.Count) {
    Write-Error "frontend modular scaffold module_count mismatch in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.shared_source_count -ne $sharedSources.Count) {
    Write-Error "frontend modular scaffold shared_source_count mismatch in $scaffoldPath"
    exit 2
  }
}

function Assert-FrontendInvocationLock {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $lockPath -PathType Leaf)) {
    Write-Error "frontend invocation lock artifact missing at $lockPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend invocation lock artifact is not valid JSON at $lockPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend invocation lock contract id mismatch in $lockPath"
    exit 2
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
  if ([string]$payload.scaffold_contract_id -ne $expectedScaffoldContractId) {
    Write-Error "frontend invocation lock scaffold contract id mismatch in $lockPath"
    exit 2
  }

  $scaffold = $payload.scaffold
  if ($null -eq $scaffold) {
    Write-Error "frontend invocation lock scaffold metadata missing in $lockPath"
    exit 2
  }

  $scaffoldRelativePath = [string]$scaffold.path
  $scaffoldExpectedHash = [string]$scaffold.sha256
  if ([string]::IsNullOrWhiteSpace($scaffoldRelativePath) -or [string]::IsNullOrWhiteSpace($scaffoldExpectedHash)) {
    Write-Error "frontend invocation lock scaffold metadata invalid in $lockPath"
    exit 2
  }

  $scaffoldPath = $scaffoldRelativePath
  if (-not [System.IO.Path]::IsPathRooted($scaffoldPath)) {
    $scaffoldPath = Join-Path $RepoRoot $scaffoldPath
  }
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend invocation lock scaffold path missing at $scaffoldPath"
    exit 2
  }
  $scaffoldActualHash = Get-FileSha256Hex -Path $scaffoldPath
  if ($scaffoldActualHash -ne $scaffoldExpectedHash.ToLowerInvariant()) {
    Write-Error "frontend invocation lock scaffold sha256 mismatch in $lockPath"
    exit 2
  }

  $binaries = @($payload.binaries)
  if ($binaries.Count -lt 2) {
    Write-Error "frontend invocation lock binaries list must include native and c-api runner entries in $lockPath"
    exit 2
  }

  $expectedBinaries = [ordered]@{
    "objc3c-native" = "artifacts/bin/objc3c-native.exe"
    "objc3c-frontend-c-api-runner" = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  }
  $binaryIndex = @{}
  foreach ($binary in $binaries) {
    $binaryName = [string]$binary.name
    $binaryPath = [string]$binary.path
    $binaryHash = [string]$binary.sha256
    if ([string]::IsNullOrWhiteSpace($binaryName) -or
        [string]::IsNullOrWhiteSpace($binaryPath) -or
        [string]::IsNullOrWhiteSpace($binaryHash)) {
      Write-Error "frontend invocation lock binary entry is invalid in $lockPath"
      exit 2
    }
    if ($binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock contains duplicate binary entry '$binaryName' in $lockPath"
      exit 2
    }
    $binaryIndex[$binaryName] = $binary
  }

  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock missing binary '$binaryName' in $lockPath"
      exit 2
    }
    $binary = $binaryIndex[$binaryName]
    $expectedRelativePath = [string]$expectedBinaries[$binaryName]
    $manifestRelativePath = ([string]$binary.path).Replace('\', '/')
    if ($manifestRelativePath -ne $expectedRelativePath) {
      Write-Error "frontend invocation lock binary path mismatch for '$binaryName' in $lockPath"
      exit 2
    }
    $binaryPath = Join-Path $RepoRoot $expectedRelativePath
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      Write-Error "frontend invocation lock binary path missing at $binaryPath"
      exit 2
    }
    $binaryActualHash = Get-FileSha256Hex -Path $binaryPath
    if ($binaryActualHash -ne ([string]$binary.sha256).ToLowerInvariant()) {
      Write-Error "frontend invocation lock binary sha256 mismatch for '$binaryName' in $lockPath"
      exit 2
    }
  }
}

$parsed = Parse-WrapperArguments -RawArgs $args
$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$buildResult = $null

if (-not $parsed.use_cache) {
  $buildResult = Invoke-BuildNativeCompiler -RepoRoot $repoRoot
  foreach ($lineText in @($buildResult.build_output_lines)) {
    Write-Output $lineText
  }
  $buildExit = [int]$buildResult.exit_code
  if ($buildExit -ne 0) { exit $buildExit }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "native compiler executable missing at $exe"
    exit 2
  }

  Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
  Assert-FrontendInvocationLock -RepoRoot $repoRoot -BuildResult $buildResult
  $compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments $parsed.compile_args
  exit $compileExit
}

$argsWithoutOutDir = Get-ArgsWithoutOutDir -CompileArgs $parsed.compile_args
$inputPath = $null
if ($argsWithoutOutDir.Count -gt 0) {
  $inputCandidate = $argsWithoutOutDir[0]
  if (-not [string]::IsNullOrWhiteSpace($inputCandidate)) {
    $inputPath = [System.IO.Path]::GetFullPath($inputCandidate)
  }
}

$cacheRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/cache"
$compilerSourcePath = Join-Path $repoRoot "native/objc3c/src/main.cpp"
$cacheKey = Get-CacheKey `
  -InputPath $inputPath `
  -ArgsWithoutOutDir $argsWithoutOutDir `
  -CompilerSourcePath $compilerSourcePath `
  -WrapperScriptPath $PSCommandPath

if ($null -ne $cacheKey) {
  $entryDir = Join-Path $cacheRoot $cacheKey
  $filesDir = Join-Path $entryDir "files"
  $exitPath = Join-Path $entryDir "exit_code.txt"
  $readyPath = Join-Path $entryDir "ready.marker"

  if ((Test-Path -LiteralPath $readyPath -PathType Leaf) -and
      (Test-Path -LiteralPath $exitPath -PathType Leaf) -and
      (Test-Path -LiteralPath $filesDir -PathType Container)) {
    try {
      Copy-DirectoryContents -SourceRoot $filesDir -DestinationRoot $parsed.out_dir
      $cachedExitCode = [int](Get-Content -LiteralPath $exitPath -Raw)
      Write-Output "cache_hit=true"
      exit $cachedExitCode
    } catch {
      # fall through to cache miss path
    }
  }
}

if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
  $buildResult = Invoke-BuildNativeCompiler -RepoRoot $repoRoot
  foreach ($lineText in @($buildResult.build_output_lines)) {
    Write-Output $lineText
  }
  $buildExit = [int]$buildResult.exit_code
  if ($buildExit -ne 0) {
    Write-Output "cache_hit=false"
    exit $buildExit
  }
}

if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
  Write-Error "native compiler executable missing at $exe"
  exit 2
}

Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
Assert-FrontendInvocationLock -RepoRoot $repoRoot -BuildResult $buildResult
$compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments $parsed.compile_args

if ($null -ne $cacheKey) {
  try {
    New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
    $stagingDir = Join-Path $cacheRoot ("_stage_" + [Guid]::NewGuid().ToString("N"))
    $stageFilesDir = Join-Path $stagingDir "files"
    New-Item -ItemType Directory -Force -Path $stageFilesDir | Out-Null

    Copy-DirectoryContents -SourceRoot $parsed.out_dir -DestinationRoot $stageFilesDir
    Set-Content -LiteralPath (Join-Path $stagingDir "exit_code.txt") -Value "$compileExit" -Encoding ascii
    Set-Content -LiteralPath (Join-Path $stagingDir "ready.marker") -Value "ready" -Encoding ascii

    $entryDir = Join-Path $cacheRoot $cacheKey
    if (Test-Path -LiteralPath $entryDir -PathType Container) {
      # Preserve existing entry and retain this write as a traceable collision artifact.
      $collisionDir = Join-Path $cacheRoot ("_collision_" + $cacheKey + "_" + [Guid]::NewGuid().ToString("N"))
      Move-Item -LiteralPath $stagingDir -Destination $collisionDir -Force
    } else {
      Move-Item -LiteralPath $stagingDir -Destination $entryDir -Force
    }
  } catch {
    # Fail closed: cache population must never block compile wrapper.
  }
}

Write-Output "cache_hit=false"
exit $compileExit
