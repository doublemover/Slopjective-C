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
  $frontendScaffoldRelativePath = $null
  foreach ($line in $buildOutput) {
    $lineText = [string]$line
    Write-Output $lineText
    if ($lineText.StartsWith("frontend_scaffold=")) {
      $frontendScaffoldRelativePath = $lineText.Substring("frontend_scaffold=".Length).Trim()
    }
  }
  return [pscustomobject]@{
    exit_code = [int]$LASTEXITCODE
    frontend_scaffold_relative_path = $frontendScaffoldRelativePath
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

$parsed = Parse-WrapperArguments -RawArgs $args
$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$buildResult = $null

if (-not $parsed.use_cache) {
  $buildResult = Invoke-BuildNativeCompiler -RepoRoot $repoRoot
  $buildExit = [int]$buildResult.exit_code
  if ($buildExit -ne 0) { exit $buildExit }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "native compiler executable missing at $exe"
    exit 2
  }

  Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
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
