$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$source = Join-Path $repoRoot "tests/tooling/fixtures/native/hello.objc3"
$proofRoot = Join-Path $repoRoot "tmp/reports/objc3c-native/compile-proof"
$proofDir = Join-Path $proofRoot (Get-Date -Format "yyyyMMdd_HHmmss_fff")
$run1 = Join-Path $proofDir "run1"
$run2 = Join-Path $proofDir "run2"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"

if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
  throw "missing source fixture: $source"
}
if (-not (Test-Path -LiteralPath $buildScript -PathType Leaf)) {
  throw "missing build script: $buildScript"
}

New-Item -ItemType Directory -Force -Path $run1 | Out-Null
New-Item -ItemType Directory -Force -Path $run2 | Out-Null

& powershell -NoProfile -ExecutionPolicy Bypass -File $buildScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
  throw "missing native executable after build: $exe"
}

& $exe $source --out-dir $run1 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exe $source --out-dir $run2 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$m1 = Get-Content -LiteralPath (Join-Path $run1 "module.manifest.json") -Raw
$m2 = Get-Content -LiteralPath (Join-Path $run2 "module.manifest.json") -Raw
$d1 = Get-Content -LiteralPath (Join-Path $run1 "module.diagnostics.txt") -Raw
$d2 = Get-Content -LiteralPath (Join-Path $run2 "module.diagnostics.txt") -Raw
$ll1 = Get-Content -LiteralPath (Join-Path $run1 "module.ll") -Raw
$ll2 = Get-Content -LiteralPath (Join-Path $run2 "module.ll") -Raw

if ($m1 -ne $m2) { throw "manifest drift across deterministic replay" }
if ($d1 -ne $d2) { throw "diagnostics drift across deterministic replay" }
if ($ll1 -ne $ll2) { throw "llvm ir drift across deterministic replay" }
if ($ll1 -notmatch "define i32 @objc3c_entry") { throw "emitted llvm ir missing objc3c_entry" }

$sha = [System.Security.Cryptography.SHA256]::Create()
$bytes = [System.IO.File]::ReadAllBytes((Join-Path $run1 "module.obj"))
$hashBytes = $sha.ComputeHash($bytes)
$objHash = ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
$digest = [ordered]@{
  source = [System.IO.Path]::GetRelativePath($repoRoot, $source).Replace("\", "/")
  deterministic = [ordered]@{
    manifest = $true
    diagnostics = $true
    llvm_ir = $true
  }
  object_sha256 = $objHash
  proof_dir = [System.IO.Path]::GetRelativePath($repoRoot, $proofDir).Replace("\", "/")
}
$digest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $proofDir "digest.json") -Encoding utf8

Write-Output "status: PASS"
Write-Output ("proof_dir: " + $digest.proof_dir)
