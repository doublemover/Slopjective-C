$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$runtimeDir = Join-Path $repoRoot 'native\objc3c\src\runtime\concurrency'
$sourceDir = Join-Path $repoRoot 'native\objc3c\src'
$publicDir = Join-Path $repoRoot 'native\objc3c\src\runtime\public'
$fixture = Join-Path $PSScriptRoot 'fixtures\clang_foundation\services\task_services.cpp'
$cFixture = Join-Path $PSScriptRoot 'fixtures\clang_foundation\services\task_services_abi.c'
$cCompiler = (Get-Command clang -ErrorAction Stop).Source
$compiler = (Get-Command clang++ -ErrorAction Stop).Source
$tempRoot = [System.IO.Path]::GetFullPath([System.IO.Path]::GetTempPath()).TrimEnd('\', '/')
$outputDir = Join-Path $tempRoot ('objc3-task-services-' + [guid]::NewGuid().ToString('N'))
$resolvedOutputDir = [System.IO.Path]::GetFullPath($outputDir)
if (-not $resolvedOutputDir.StartsWith($tempRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'Refusing to use a test output path outside the temporary directory.'
}
$executable = Join-Path $outputDir 'task-services-tests.exe'

New-Item -ItemType Directory -Path $outputDir | Out-Null
try {
    & $cCompiler -std=c18 -fsyntax-only -I $publicDir $cFixture
    if ($LASTEXITCODE -ne 0) { throw "C18 ABI header check failed with exit code $LASTEXITCODE" }

    & $compiler -std=c++17 -pthread -DOBJC3_RUNTIME_SERVICE_TESTING `
        -I $publicDir -I $sourceDir `
        $fixture `
        (Join-Path $runtimeDir 'service_task.cpp') `
        (Join-Path $runtimeDir 'service_executor.cpp') `
        (Join-Path $runtimeDir 'service_actor.cpp') `
        (Join-Path $runtimeDir 'service_group.cpp') `
        (Join-Path $sourceDir 'runtime\services\error_service.cpp') `
        -o $executable
    if ($LASTEXITCODE -ne 0) { throw "clang++ failed with exit code $LASTEXITCODE" }

    & $executable
    if ($LASTEXITCODE -ne 0) { throw "task services tests failed with exit code $LASTEXITCODE" }
}
finally {
    if ($resolvedOutputDir.StartsWith($tempRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
        Remove-Item -LiteralPath $resolvedOutputDir -Recurse -Force -ErrorAction SilentlyContinue
    }
}
