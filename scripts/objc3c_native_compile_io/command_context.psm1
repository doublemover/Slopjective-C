$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileIoScriptRoot = Split-Path -Parent $PSScriptRoot
$hashIoModule = Join-Path $compileIoScriptRoot "objc3c_native_compile_provenance/hash_io.psm1"
Import-Module $hashIoModule -Force -DisableNameChecking

function Get-ResourceDirectoryIdentity {
  param([string[]]$Arguments)
  $resourcePath = ""
  for ($index = 0; $index -lt $Arguments.Count; $index++) {
    $argument = [string]$Arguments[$index]
    if ($argument -eq "-resource-dir") {
      if ($index + 1 -ge $Arguments.Count) { return $null }
      $resourcePath = [string]$Arguments[$index + 1]
      $index++
      continue
    }
    if ($argument.StartsWith("-resource-dir=", [StringComparison]::Ordinal)) {
      $resourcePath = $argument.Substring("-resource-dir=".Length)
      continue
    }
  }
  if ([string]::IsNullOrWhiteSpace($resourcePath)) {
    return [ordered]@{ path = ""; sha256 = "" }
  }
  if (!(Test-Path -LiteralPath $resourcePath -PathType Container)) { return $null }
  $fullRoot = [System.IO.Path]::GetFullPath($resourcePath)
  $files = @(Get-ChildItem -LiteralPath $fullRoot -Recurse -File -Force |
    Sort-Object -Property FullName)
  $material = [System.Text.StringBuilder]::new()
  foreach ($file in $files) {
    if (($file.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) { return $null }
    $relative = [System.IO.Path]::GetRelativePath($fullRoot, $file.FullName).Replace('\','/')
    if ([System.IO.Path]::IsPathRooted($relative) -or $relative.Split('/') -contains '..') { return $null }
    $digest = Get-FileSha256Hex -Path $file.FullName
    [void]$material.Append($relative.Length).Append(':').Append($relative).Append($digest).Append("`n")
  }
  $identity = Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes($material.ToString()))
  return [ordered]@{ path = $fullRoot; sha256 = $identity }
}

function Get-Objc3cNativeCompileCacheKey {
  param([string]$InputPath,[string[]]$ArgsWithoutOutDir,[string]$CompilerSourcePath,[string]$WrapperScriptPath,[string]$CompilerBinaryPath,[string]$RepoRoot)
  if ([string]::IsNullOrWhiteSpace($InputPath) -or !(Test-Path -LiteralPath $InputPath -PathType Leaf)) { return $null }
  $inputFull = [System.IO.Path]::GetFullPath($InputPath)
  $sourceText = Get-Content -LiteralPath $inputFull -Raw
  # There is no compiler-emitted import closure artifact in this wrapper yet.
  # Until that exists, imported sources must compile without cache reuse.
  if ($sourceText -match '#|(?i)\b(?:import|include)\b' -or
      @($ArgsWithoutOutDir | Where-Object { $_ -match '(?i)import|include|^@' }).Count -gt 0) { return $null }
  $rootFull = if ($RepoRoot) { [System.IO.Path]::GetFullPath($RepoRoot) } else { (Get-Location).Path }
  $wrapperRoot = Split-Path -Parent $WrapperScriptPath
  $wrapperPaths = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
  if (Test-Path -LiteralPath $WrapperScriptPath -PathType Leaf) { [void]$wrapperPaths.Add([System.IO.Path]::GetFullPath($WrapperScriptPath)) }
  foreach ($pattern in @('objc3c_native_compile*.psm1','objc3c_native_compile*.ps1')) {
    foreach ($file in Get-ChildItem -LiteralPath $wrapperRoot -Filter $pattern -File -ErrorAction SilentlyContinue) { [void]$wrapperPaths.Add($file.FullName) }
  }
  foreach ($relative in @('objc3c_native_compile_io','objc3c_native_compile_wrapper','objc3c_native_compile_frontend_guards','objc3c_native_compile_toolchain','objc3c_native_compile_provenance','objc3c_native_compile_arguments')) {
    $dependencyRoot = Join-Path $rootFull ('scripts/' + $relative)
    if (Test-Path -LiteralPath $dependencyRoot -PathType Container) {
      foreach ($file in Get-ChildItem -LiteralPath $dependencyRoot -Recurse -File) { [void]$wrapperPaths.Add($file.FullName) }
    }
  }
  $wrapperFiles = @($wrapperPaths | Sort-Object | ForEach-Object { [ordered]@{ path = [System.IO.Path]::GetRelativePath($rootFull, $_); sha256 = Get-FileSha256Hex -Path $_ } })
  $resourceIdentity = Get-ResourceDirectoryIdentity -Arguments $ArgsWithoutOutDir
  if ($null -eq $resourceIdentity) { return $null }
  $compilerIdentity = [ordered]@{ path = $CompilerBinaryPath; sha256 = Get-OptionalFileHash -Path $CompilerBinaryPath }
  if ([string]::IsNullOrWhiteSpace([string]$compilerIdentity.sha256)) { return $null }
  $compilerSourceIdentity = [ordered]@{ path = $CompilerSourcePath; sha256 = Get-OptionalFileHash -Path $CompilerSourcePath }
  $payload = [ordered]@{
    version = 6
    compiler = $compilerIdentity
    resource_directory = $resourceIdentity
    source = [ordered]@{ path = $inputFull; sha256 = Get-FileSha256Hex -Path $inputFull }
    compiler_source = $compilerSourceIdentity
    imported_semantic_inputs = @()
    wrapper_files = $wrapperFiles
    args = @($ArgsWithoutOutDir)
    path_semantics = [ordered]@{ cwd = $rootFull; input = $inputFull }
  }
  return Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($payload | ConvertTo-Json -Compress -Depth 12)))
}

function New-Objc3cNativeCompileCacheContext {
  param([string]$RepoRoot,[string]$InputPath,[string[]]$ArgsWithoutOutDir,[string]$WrapperScriptPath,[string]$CompilerBinaryPath)
  $cacheRoot = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/cache"
  $compilerSourcePath = Join-Path $RepoRoot "native/objc3c/src/main.cpp"
  $cacheKey = Get-Objc3cNativeCompileCacheKey -InputPath $InputPath -ArgsWithoutOutDir $ArgsWithoutOutDir -CompilerSourcePath $compilerSourcePath -WrapperScriptPath $WrapperScriptPath -CompilerBinaryPath $CompilerBinaryPath -RepoRoot $RepoRoot
  return [pscustomobject]@{ cache_root = $cacheRoot; compiler_source_path = $compilerSourcePath; entry_contract_id = "objc3c-native-cache-entry/canonical-provenance-v4"; cache_key = $cacheKey; allowed_destination_root = [System.IO.Path]::GetFullPath($RepoRoot) }
}
Export-ModuleMember -Function @("Get-Objc3cNativeCompileCacheKey","New-Objc3cNativeCompileCacheContext")
