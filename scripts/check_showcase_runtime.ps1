param(
  [string[]]$Example = @(),
  [string[]]$Capability = @()
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$showcaseSurfaceScript = Join-Path $repoRoot "scripts/check_showcase_surface.py"
$runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
$showcaseSummaryPath = Join-Path $repoRoot "tmp/reports/showcase/summary.json"
$runtimeSummaryPath = Join-Path $repoRoot "tmp/reports/showcase/runtime-summary.json"
$configuredClangPath = $env:OBJC3C_NATIVE_EXECUTION_CLANG_PATH
$clangCommand = if ([string]::IsNullOrWhiteSpace($configuredClangPath)) { "clang" } else { $configuredClangPath }

if (!(Test-Path -LiteralPath $showcaseSurfaceScript -PathType Leaf)) {
  throw "showcase runtime FAIL: missing showcase surface script at $showcaseSurfaceScript"
}
if (!(Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
  throw "showcase runtime FAIL: missing runtime launch contract helper at $runtimeLaunchContractScript"
}

. $runtimeLaunchContractScript

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

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = [System.IO.Path]::GetFullPath($Path)
  $fullRoot = [System.IO.Path]::GetFullPath($Root).TrimEnd('\', '/')
  $rootPrefix = $fullRoot + [System.IO.Path]::DirectorySeparatorChar
  if ($fullPath -eq $fullRoot) {
    return "."
  }
  if ($fullPath.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    return [System.IO.Path]::GetRelativePath($fullRoot, $fullPath).Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

$showcaseArgs = New-Object System.Collections.Generic.List[string]
$showcaseArgs.Add($showcaseSurfaceScript) | Out-Null
foreach ($value in $Example) {
  $showcaseArgs.Add("--example") | Out-Null
  $showcaseArgs.Add($value) | Out-Null
}
foreach ($value in $Capability) {
  $showcaseArgs.Add("--capability") | Out-Null
  $showcaseArgs.Add($value) | Out-Null
}

& python @($showcaseArgs.ToArray())
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

if (!(Test-Path -LiteralPath $showcaseSummaryPath -PathType Leaf)) {
  throw "showcase runtime FAIL: missing showcase summary at $showcaseSummaryPath"
}

$showcaseSummary = Get-Content -LiteralPath $showcaseSummaryPath -Raw | ConvertFrom-Json
if ("$($showcaseSummary.contract_id)".Trim() -ne "objc3c.showcase.surface.summary.v1") {
  throw "showcase runtime FAIL: showcase summary contract drifted at $showcaseSummaryPath"
}

$results = New-Object System.Collections.Generic.List[object]
foreach ($exampleRecord in @($showcaseSummary.examples)) {
  $exampleId = "$($exampleRecord.example_id)".Trim()
  $workspaceManifestPath = Join-Path $repoRoot ("$($exampleRecord.workspace_manifest)".Replace('/', '\'))
  if (!(Test-Path -LiteralPath $workspaceManifestPath -PathType Leaf)) {
    throw "showcase runtime FAIL: missing workspace manifest for $exampleId at $workspaceManifestPath"
  }
  $workspace = Get-Content -LiteralPath $workspaceManifestPath -Raw | ConvertFrom-Json
  if ("$($workspace.contract_id)".Trim() -ne "objc3c.showcase.example.workspace.v1") {
    throw "showcase runtime FAIL: workspace contract drifted for $exampleId"
  }

  $compileDir = Join-Path $repoRoot ("$($exampleRecord.out_dir)".Replace('/', '\'))
  $objectPath = Join-Path $repoRoot ("$($exampleRecord.artifacts.object)".Replace('/', '\'))
  if (!(Test-Path -LiteralPath $objectPath -PathType Leaf)) {
    throw "showcase runtime FAIL: missing object artifact for $exampleId at $objectPath"
  }

  $runtimeSurface = $workspace.runtime_surface
  $runtimeDir = Join-Path $repoRoot ("$($runtimeSurface.runtime_output_root)".Replace('/', '\'))
  New-Item -ItemType Directory -Force -Path $runtimeDir | Out-Null

  $launchContract = Get-Objc3cRuntimeLaunchContract -CompileDir $compileDir -RepoRoot $repoRoot -EmitPrefix "module"
  if ("$($runtimeSurface.launch_contract_helper)".Trim() -ne "scripts/objc3c_runtime_launch_contract.ps1") {
    throw "showcase runtime FAIL: launch contract helper drifted for $exampleId"
  }
  if ("$($runtimeSurface.runtime_library_resolution_model)".Trim() -ne "$($launchContract.runtime_library_resolution_model)".Trim()) {
    throw "showcase runtime FAIL: runtime library resolution model drifted for $exampleId"
  }
  if ("$($runtimeSurface.driver_linker_flag_consumption_model)".Trim() -ne "$($launchContract.driver_linker_flag_consumption_model)".Trim()) {
    throw "showcase runtime FAIL: driver linker flag consumption model drifted for $exampleId"
  }

  $exePath = Join-Path $runtimeDir "module.exe"
  $linkLog = Join-Path $runtimeDir "link.log"
  $runLog = Join-Path $runtimeDir "run.log"
  $linkArgs = @($objectPath, $launchContract.runtime_library_path) + @($launchContract.driver_linker_flags) + @("-o", $exePath, "-fno-color-diagnostics")
  $linkExit = Invoke-LoggedCommand -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
  if ($linkExit -ne 0) {
    throw "showcase runtime FAIL: link failed for $exampleId (exit=$linkExit)"
  }
  if (!(Test-Path -LiteralPath $exePath -PathType Leaf)) {
    throw "showcase runtime FAIL: missing module.exe for $exampleId at $exePath"
  }

  $runExit = Invoke-LoggedCommand -Command $exePath -Arguments @() -LogPath $runLog
  $expectedExit = [int]$runtimeSurface.expected_exit_code
  if ($runExit -ne $expectedExit) {
    throw "showcase runtime FAIL: unexpected exit for $exampleId (expected=$expectedExit actual=$runExit)"
  }

  $results.Add([ordered]@{
    example_id = $exampleId
    presentation = [ordered]@{
      title = "$($workspace.presentation.title)"
      headline = "$($workspace.presentation.headline)"
    }
    compile_dir = Get-RepoRelativePath -Path $compileDir -Root $repoRoot
    runtime_output_root = Get-RepoRelativePath -Path $runtimeDir -Root $repoRoot
    executable = Get-RepoRelativePath -Path $exePath -Root $repoRoot
    link_log = Get-RepoRelativePath -Path $linkLog -Root $repoRoot
    run_log = Get-RepoRelativePath -Path $runLog -Root $repoRoot
    expected_exit_code = $expectedExit
    actual_exit_code = $runExit
    runtime_library = Get-RepoRelativePath -Path $launchContract.runtime_library_path -Root $repoRoot
    driver_linker_flags = @($launchContract.driver_linker_flags)
  }) | Out-Null
  Write-Output ("out_dir: " + (Get-RepoRelativePath -Path $runtimeDir -Root $repoRoot))
}

$summaryPayload = [pscustomobject]@{
  contract_id = "objc3c.showcase.runtime.summary.v1"
  schema_version = 1
  showcase_summary = "tmp/reports/showcase/summary.json"
  examples = @($results.ToArray())
}
$runtimeSummaryDir = Split-Path -Parent $runtimeSummaryPath
New-Item -ItemType Directory -Force -Path $runtimeSummaryDir | Out-Null
$summaryPayload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $runtimeSummaryPath -Encoding utf8

Write-Output ("summary_path: " + (Get-RepoRelativePath -Path $runtimeSummaryPath -Root $repoRoot))
Write-Output ("showcase-runtime: OK (" + (($results | ForEach-Object { $_.example_id }) -join ", ") + ")")
