$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$source = Join-Path $repoRoot "tests/tooling/fixtures/native/hello.objc3"
$proofRoot = Join-Path $repoRoot "tmp/reports/objc3c-native/compile-proof"
$proofDir = Join-Path $proofRoot (Get-Date -Format "yyyyMMdd_HHmmss_fff")
$run1 = Join-Path $proofDir "run1"
$run2 = Join-Path $proofDir "run2"
$compileWrapper = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"
$runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"

if (-not (Test-Path -LiteralPath $compileWrapper -PathType Leaf)) {
  throw "missing compile wrapper: $compileWrapper"
}
if (-not (Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
  throw "missing runtime launch contract helper: $runtimeLaunchContractScript"
}
. $runtimeLaunchContractScript

function Get-RepoRelativePathCompat {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path

  if ($resolvedRoot.EndsWith('\') -or $resolvedRoot.EndsWith('/')) {
    $rootWithSeparator = $resolvedRoot
  } else {
    $rootWithSeparator = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  }

  $relativePath = $null
  $getRelativeMethod = [System.IO.Path].GetMethod("GetRelativePath", [Type[]]@([string], [string]))
  if ($null -ne $getRelativeMethod) {
    $relativePath = [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)
  } else {
    $rootUri = New-Object System.Uri($rootWithSeparator)
    $targetUri = New-Object System.Uri($resolvedTarget)
    $relativeUri = $rootUri.MakeRelativeUri($targetUri)
    $relativePath = [System.Uri]::UnescapeDataString($relativeUri.ToString())
  }

  return $relativePath.Replace('\', '/')
}

if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
  throw "missing source fixture: $source"
}

New-Item -ItemType Directory -Force -Path $run1 | Out-Null
New-Item -ItemType Directory -Force -Path $run2 | Out-Null

& $compileWrapper $source --out-dir $run1 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $compileWrapper $source --out-dir $run2 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$m1 = Get-Content -LiteralPath (Join-Path $run1 "module.manifest.json") -Raw
$m2 = Get-Content -LiteralPath (Join-Path $run2 "module.manifest.json") -Raw
$r1 = Get-Content -LiteralPath (Join-Path $run1 "module.runtime-registration-manifest.json") -Raw
$r2 = Get-Content -LiteralPath (Join-Path $run2 "module.runtime-registration-manifest.json") -Raw
$d1 = Get-Content -LiteralPath (Join-Path $run1 "module.diagnostics.txt") -Raw
$d2 = Get-Content -LiteralPath (Join-Path $run2 "module.diagnostics.txt") -Raw
$ll1 = Get-Content -LiteralPath (Join-Path $run1 "module.ll") -Raw
$ll2 = Get-Content -LiteralPath (Join-Path $run2 "module.ll") -Raw
$launchContract1 = Get-Objc3cRuntimeLaunchContract -CompileDir $run1 -RepoRoot $repoRoot -EmitPrefix "module"
$launchContract2 = Get-Objc3cRuntimeLaunchContract -CompileDir $run2 -RepoRoot $repoRoot -EmitPrefix "module"

if ($m1 -ne $m2) { throw "manifest drift across deterministic replay" }
if ($r1 -ne $r2) { throw "runtime registration manifest drift across deterministic replay" }
if ($d1 -ne $d2) { throw "diagnostics drift across deterministic replay" }
if ($ll1 -ne $ll2) { throw "llvm ir drift across deterministic replay" }
if ($ll1 -notmatch "define i32 @objc3c_entry") { throw "emitted llvm ir missing objc3c_entry" }
if ($launchContract1.launch_integration_contract_id -ne $launchContract2.launch_integration_contract_id) {
  throw "launch integration contract drift across deterministic replay"
}
if ($launchContract1.runtime_library_relative_path -ne $launchContract2.runtime_library_relative_path) {
  throw "runtime library drift across deterministic replay"
}
if ((ConvertTo-Json @($launchContract1.driver_linker_flags) -Compress) -ne (ConvertTo-Json @($launchContract2.driver_linker_flags) -Compress)) {
  throw "driver linker flags drift across deterministic replay"
}

$sha = [System.Security.Cryptography.SHA256]::Create()
$bytes = [System.IO.File]::ReadAllBytes((Join-Path $run1 "module.obj"))
$hashBytes = $sha.ComputeHash($bytes)
$objHash = ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
$digest = [ordered]@{
  source = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $source
  deterministic = [ordered]@{
    manifest = $true
    runtime_registration_manifest = $true
    diagnostics = $true
    llvm_ir = $true
    launch_contract = $true
    driver_linker_flags = $true
  }
  launch_integration_contract_id = $launchContract1.launch_integration_contract_id
  runtime_library = $launchContract1.runtime_library_relative_path
  driver_linker_flags = @($launchContract1.driver_linker_flags)
  object_sha256 = $objHash
  proof_dir = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $proofDir
}
$digest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $proofDir "digest.json") -Encoding utf8

Write-Output "status: PASS"
Write-Output ("proof_dir: " + $digest.proof_dir)
