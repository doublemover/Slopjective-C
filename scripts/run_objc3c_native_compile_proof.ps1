$ErrorActionPreference = "Stop"

$source = "tests/tooling/fixtures/native/hello.objc3"
$proofDir = "artifacts/compilation/objc3c-native/proof_20260226"
$run1 = Join-Path $proofDir "run1"
$run2 = Join-Path $proofDir "run2"

New-Item -ItemType Directory -Force -Path $run1 | Out-Null
New-Item -ItemType Directory -Force -Path $run2 | Out-Null

& powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = "artifacts/bin/objc3c-native.exe"

& $exe $source --out-dir $run1 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $exe $source --out-dir $run2 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$m1 = Get-Content (Join-Path $run1 "module.manifest.json") -Raw
$m2 = Get-Content (Join-Path $run2 "module.manifest.json") -Raw
$d1 = Get-Content (Join-Path $run1 "module.diagnostics.txt") -Raw
$d2 = Get-Content (Join-Path $run2 "module.diagnostics.txt") -Raw
$ll1 = Get-Content (Join-Path $run1 "module.ll") -Raw
$ll2 = Get-Content (Join-Path $run2 "module.ll") -Raw

if ($m1 -ne $m2) { throw "manifest drift across deterministic replay" }
if ($d1 -ne $d2) { throw "diagnostics drift across deterministic replay" }
if ($ll1 -ne $ll2) { throw "llvm ir drift across deterministic replay" }
if ($ll1 -notmatch "define i32 @objc3c_entry") { throw "emitted llvm ir missing objc3c_entry" }

$sha = [System.Security.Cryptography.SHA256]::Create()
$bytes = [System.IO.File]::ReadAllBytes((Join-Path $run1 "module.obj"))
$hashBytes = $sha.ComputeHash($bytes)
$objHash = ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
$digest = [ordered]@{
  source = $source
  deterministic = [ordered]@{
    manifest = $true
    diagnostics = $true
    llvm_ir = $true
  }
  object_sha256 = $objHash
}
$digest | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $proofDir "digest.json") -Encoding utf8

Write-Output "status: PASS"
Write-Output "proof_dir: $proofDir"
