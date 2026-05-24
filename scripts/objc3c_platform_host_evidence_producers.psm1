Set-StrictMode -Version Latest

function Test-Objc3cDarwinPlatformEvidenceEnabled {
  param(
    [string]$EvidenceRoot = "",
    [string]$PlatformId = ""
  )

  return (-not [string]::IsNullOrWhiteSpace($EvidenceRoot)) -and $PlatformId -eq "darwin-arm64"
}

function ConvertTo-Objc3cEvidenceHostPath {
  param([Parameter(Mandatory = $true)][string]$RelativePath)

  return $RelativePath.Replace('/', [System.IO.Path]::DirectorySeparatorChar)
}

function Get-Objc3cEvidenceRepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  if (Test-Path -LiteralPath $TargetPath) {
    $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  } else {
    $resolvedTarget = [System.IO.Path]::GetFullPath($TargetPath)
  }
  return ([System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)).Replace('\', '/')
}

function Split-Objc3cEvidenceLines {
  param([string]$Text = "")

  if ([string]::IsNullOrWhiteSpace($Text)) {
    return @()
  }
  return @($Text -split "`r?`n" | Where-Object { $_ -ne "" })
}

function Get-Objc3cEvidenceFileDigest {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $relativePath = Get-Objc3cEvidenceRepoRelativePath -RootPath $RootPath -TargetPath $TargetPath
  if (!(Test-Path -LiteralPath $TargetPath -PathType Leaf)) {
    return [ordered]@{
      path = $relativePath
      exists = $false
    }
  }

  $item = Get-Item -LiteralPath $TargetPath
  return [ordered]@{
    path = $relativePath
    exists = $true
    size_bytes = [int64]$item.Length
    sha256 = (Get-FileHash -LiteralPath $TargetPath -Algorithm SHA256).Hash.ToLowerInvariant()
  }
}

function Invoke-Objc3cPlatformEvidenceTool {
  param(
    [Parameter(Mandatory = $true)][string]$Tool,
    [string[]]$Arguments = @()
  )

  $command = Get-Command -Name $Tool -ErrorAction SilentlyContinue
  if ($null -eq $command -or [string]::IsNullOrWhiteSpace($command.Source)) {
    return [ordered]@{
      tool = $Tool
      arguments = @($Arguments)
      available = $false
      exit_code = -1
      stdout = ""
      stderr = "tool-not-found"
    }
  }

  $processInfo = [System.Diagnostics.ProcessStartInfo]::new()
  $processInfo.FileName = $command.Source
  $processInfo.UseShellExecute = $false
  $processInfo.RedirectStandardOutput = $true
  $processInfo.RedirectStandardError = $true
  foreach ($argument in @($Arguments)) {
    [void]$processInfo.ArgumentList.Add([string]$argument)
  }

  try {
    $process = [System.Diagnostics.Process]::Start($processInfo)
    $stdout = $process.StandardOutput.ReadToEnd()
    $stderr = $process.StandardError.ReadToEnd()
    $process.WaitForExit()
    return [ordered]@{
      tool = $Tool
      resolved_tool = $command.Source
      arguments = @($Arguments)
      available = $true
      exit_code = [int]$process.ExitCode
      stdout = $stdout.TrimEnd()
      stderr = $stderr.TrimEnd()
    }
  } catch {
    return [ordered]@{
      tool = $Tool
      resolved_tool = $command.Source
      arguments = @($Arguments)
      available = $true
      exit_code = -1
      stdout = ""
      stderr = $_.Exception.Message
    }
  }
}

function Get-Objc3cDarwinInstallName {
  param([object]$OtoolD)

  if ($null -eq $OtoolD -or [int]$OtoolD.exit_code -ne 0) {
    return ""
  }
  $lines = @(Split-Objc3cEvidenceLines -Text ([string]$OtoolD.stdout))
  if ($lines.Count -lt 2) {
    return ""
  }
  return $lines[1].Trim()
}

function Get-Objc3cDarwinLinkedLibraries {
  param([object]$OtoolL)

  if ($null -eq $OtoolL -or [int]$OtoolL.exit_code -ne 0) {
    return @()
  }
  $libraries = New-Object System.Collections.Generic.List[string]
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$OtoolL.stdout) | Select-Object -Skip 1)) {
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    $library = ($trimmed -split '\s+\(')[0].Trim()
    if (-not [string]::IsNullOrWhiteSpace($library)) {
      $libraries.Add($library) | Out-Null
    }
  }
  return @($libraries)
}

function Get-Objc3cDarwinRpaths {
  param([object]$OtoolLoadCommands)

  if ($null -eq $OtoolLoadCommands -or [int]$OtoolLoadCommands.exit_code -ne 0) {
    return @()
  }
  $rpaths = New-Object System.Collections.Generic.List[string]
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$OtoolLoadCommands.stdout))) {
    $trimmed = $line.Trim()
    if ($trimmed -match '^path\s+(.+?)\s+\(offset') {
      $rpaths.Add($Matches[1]) | Out-Null
    }
  }
  return @($rpaths | Sort-Object -Unique)
}

function Get-Objc3cDarwinUuidRecords {
  param([object]$DwarfDump)

  if ($null -eq $DwarfDump -or [int]$DwarfDump.exit_code -ne 0) {
    return @()
  }
  $records = New-Object System.Collections.Generic.List[object]
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$DwarfDump.stdout))) {
    if ($line -match '^UUID:\s+([0-9A-Fa-f-]+)\s+\(([^)]+)\)\s+(.+)$') {
      $records.Add([ordered]@{
          uuid = $Matches[1].ToLowerInvariant()
          arch = $Matches[2]
          path = $Matches[3]
        }) | Out-Null
    }
  }
  return @($records)
}

function Test-Objc3cDarwinUuidRecordsMatch {
  param(
    [object[]]$BinaryRecords = @(),
    [object[]]$DsymRecords = @()
  )

  if (@($BinaryRecords).Count -eq 0 -or @($DsymRecords).Count -eq 0) {
    return $false
  }
  $binaryKeys = @($BinaryRecords | ForEach-Object { "$($_.arch):$($_.uuid)" } | Sort-Object -Unique)
  $dsymKeys = @($DsymRecords | ForEach-Object { "$($_.arch):$($_.uuid)" } | Sort-Object -Unique)
  return (($binaryKeys -join "`0") -eq ($dsymKeys -join "`0"))
}

function Get-Objc3cDarwinMachOIdentity {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$Path,
    [string]$ExpectedArch = "arm64",
    [switch]$IncludeInstallName
  )

  $digest = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $Path
  if (-not [bool]$digest.exists) {
    return [ordered]@{
      artifact = $digest
      expected_arch = $ExpectedArch
      archs = @()
      expected_arch_present = $false
      mach_o_present = $false
    }
  }

  $file = Invoke-Objc3cPlatformEvidenceTool -Tool "file" -Arguments @("-b", $Path)
  $lipo = Invoke-Objc3cPlatformEvidenceTool -Tool "lipo" -Arguments @("-archs", $Path)
  $otoolHeader = Invoke-Objc3cPlatformEvidenceTool -Tool "otool" -Arguments @("-hv", $Path)
  $otoolLoadCommands = Invoke-Objc3cPlatformEvidenceTool -Tool "otool" -Arguments @("-l", $Path)
  $otoolLibraries = Invoke-Objc3cPlatformEvidenceTool -Tool "otool" -Arguments @("-L", $Path)
  $codesign = Invoke-Objc3cPlatformEvidenceTool -Tool "codesign" -Arguments @("-dv", "--verbose=4", $Path)
  $otoolInstallName = $null
  if ($IncludeInstallName.IsPresent) {
    $otoolInstallName = Invoke-Objc3cPlatformEvidenceTool -Tool "otool" -Arguments @("-D", $Path)
  }

  $archs = @()
  if ([bool]$lipo.available -and [int]$lipo.exit_code -eq 0) {
    $archs = @(([string]$lipo.stdout).Trim() -split '\s+' | Where-Object { $_ })
  } elseif (([string]$file.stdout) -match '\barm64\b') {
    $archs = @("arm64")
  }

  return [ordered]@{
    artifact = $digest
    expected_format = "Mach-O"
    expected_arch = $ExpectedArch
    archs = @($archs)
    expected_arch_present = @($archs) -contains $ExpectedArch
    mach_o_present = ([string]$file.stdout) -match 'Mach-O'
    file = $file
    mach_header = $otoolHeader
    load_commands = [ordered]@{
      tool = $otoolLoadCommands
      rpaths = @(Get-Objc3cDarwinRpaths -OtoolLoadCommands $otoolLoadCommands)
    }
    linked_libraries = @(Get-Objc3cDarwinLinkedLibraries -OtoolL $otoolLibraries)
    install_name = if ($IncludeInstallName.IsPresent) { Get-Objc3cDarwinInstallName -OtoolD $otoolInstallName } else { "" }
    install_name_tool = if ($IncludeInstallName.IsPresent) { $otoolInstallName } else { $null }
    codesign = $codesign
  }
}

function New-Objc3cDarwinDsymIdentity {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$EvidenceRoot,
    [Parameter(Mandatory = $true)][string]$BinaryPath
  )

  $binaryDigest = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $BinaryPath
  $binaryUuidTool = if ([bool]$binaryDigest.exists) {
    Invoke-Objc3cPlatformEvidenceTool -Tool "dwarfdump" -Arguments @("--uuid", $BinaryPath)
  } else {
    [ordered]@{
      tool = "dwarfdump"
      arguments = @("--uuid", $BinaryPath)
      available = $false
      exit_code = -1
      stdout = ""
      stderr = "binary-missing"
    }
  }
  $binaryUuidRecords = @(Get-Objc3cDarwinUuidRecords -DwarfDump $binaryUuidTool)

  $dsymPath = Join-Path (Join-Path $EvidenceRoot "build/dsym") ((Split-Path -Leaf $BinaryPath) + ".dSYM")
  $dsymutil = if ([bool]$binaryDigest.exists) {
    $dsymParent = Split-Path -Parent $dsymPath
    New-Item -ItemType Directory -Force -Path $dsymParent | Out-Null
    Invoke-Objc3cPlatformEvidenceTool -Tool "dsymutil" -Arguments @($BinaryPath, "-o", $dsymPath)
  } else {
    [ordered]@{
      tool = "dsymutil"
      arguments = @($BinaryPath, "-o", $dsymPath)
      available = $false
      exit_code = -1
      stdout = ""
      stderr = "binary-missing"
    }
  }
  $dsymExists = Test-Path -LiteralPath $dsymPath -PathType Container
  $dsymUuidTool = if ($dsymExists) {
    Invoke-Objc3cPlatformEvidenceTool -Tool "dwarfdump" -Arguments @("--uuid", $dsymPath)
  } else {
    [ordered]@{
      tool = "dwarfdump"
      arguments = @("--uuid", $dsymPath)
      available = $false
      exit_code = -1
      stdout = ""
      stderr = "dsym-missing"
    }
  }
  $dsymUuidRecords = @(Get-Objc3cDarwinUuidRecords -DwarfDump $dsymUuidTool)

  return [ordered]@{
    binary = $binaryDigest
    binary_uuid_tool = $binaryUuidTool
    binary_uuids = @($binaryUuidRecords)
    dsym_path = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $dsymPath
    dsym_exists = $dsymExists
    dsymutil = $dsymutil
    dsym_uuid_tool = $dsymUuidTool
    dsym_uuids = @($dsymUuidRecords)
    uuid_match = Test-Objc3cDarwinUuidRecordsMatch -BinaryRecords $binaryUuidRecords -DsymRecords $dsymUuidRecords
  }
}

function Write-Objc3cPlatformEvidenceJson {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)]$Payload
  )

  $parent = Split-Path -Parent $Path
  if ($parent) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  $Payload | ConvertTo-Json -Depth 30 | Set-Content -LiteralPath $Path -Encoding utf8
}

function Write-Objc3cDarwinObjectDebugIdentityEvidence {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EvidenceRoot = $env:OBJC3C_PLATFORM_EVIDENCE_ROOT,
    [Parameter(Mandatory = $true)][string]$PlatformId,
    [Parameter(Mandatory = $true)][string]$TargetTriple,
    [Parameter(Mandatory = $true)][string]$ObjectFormat,
    [Parameter(Mandatory = $true)][string]$DebugFormat,
    [Parameter(Mandatory = $true)][string]$NativeExecutablePath,
    [Parameter(Mandatory = $true)][string]$CapiRunnerPath,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryPath,
    [Parameter(Mandatory = $true)][string]$BuildSummaryPath
  )

  if (!(Test-Objc3cDarwinPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $artifacts = [ordered]@{
    native_executable = Get-Objc3cDarwinMachOIdentity -RepoRoot $RepoRoot -Path $NativeExecutablePath
    frontend_c_api_runner = Get-Objc3cDarwinMachOIdentity -RepoRoot $RepoRoot -Path $CapiRunnerPath
    runtime_library = Get-Objc3cDarwinMachOIdentity -RepoRoot $RepoRoot -Path $RuntimeLibraryPath -IncludeInstallName
  }
  $artifactValues = @($artifacts.Values)
  $objectStatus = if (@($artifactValues | Where-Object { -not [bool]$_.artifact.exists -or -not [bool]$_.mach_o_present -or -not [bool]$_.expected_arch_present }).Count -eq 0) {
    "GENERATED_MACHO_ARM64_IDENTITY"
  } else {
    "GENERATED_MACHO_ARM64_IDENTITY_INCOMPLETE"
  }

  $objectPayload = [ordered]@{
    contract_id = "objc3c.platform.darwin.object-identity.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8229
    target_triple = $TargetTriple
    expected_object_format = "Mach-O"
    observed_object_format = $ObjectFormat
    expected_arch = "arm64"
    support_truth = $false
    native_execution_claimed = $false
    build_summary = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $BuildSummaryPath
    status = $objectStatus
    artifacts = $artifacts
  }
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path (Join-Path $EvidenceRoot "build") "object-identity.json") `
    -Payload $objectPayload

  $debugRecords = [ordered]@{
    native_executable = New-Objc3cDarwinDsymIdentity -RepoRoot $RepoRoot -EvidenceRoot $EvidenceRoot -BinaryPath $NativeExecutablePath
    frontend_c_api_runner = New-Objc3cDarwinDsymIdentity -RepoRoot $RepoRoot -EvidenceRoot $EvidenceRoot -BinaryPath $CapiRunnerPath
    runtime_library = New-Objc3cDarwinDsymIdentity -RepoRoot $RepoRoot -EvidenceRoot $EvidenceRoot -BinaryPath $RuntimeLibraryPath
  }
  $debugValues = @($debugRecords.Values)
  $debugStatus = if (@($debugValues | Where-Object { @($_.binary_uuids).Count -eq 0 -or -not [bool]$_.uuid_match }).Count -eq 0) {
    "GENERATED_DSYM_UUID_IDENTITY"
  } else {
    "GENERATED_DSYM_UUID_IDENTITY_INCOMPLETE"
  }
  $debugPayload = [ordered]@{
    contract_id = "objc3c.platform.darwin.debug-identity.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8229
    target_triple = $TargetTriple
    expected_debug_format = "DWARF/dSYM"
    observed_debug_format = $DebugFormat
    expected_arch = "arm64"
    support_truth = $false
    native_execution_claimed = $false
    status = $debugStatus
    artifacts = $debugRecords
  }
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path (Join-Path $EvidenceRoot "build") "debug-identity.json") `
    -Payload $debugPayload
}

function Write-Objc3cDarwinRuntimeLibraryManifestEvidence {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EvidenceRoot = $env:OBJC3C_PLATFORM_EVIDENCE_ROOT,
    [Parameter(Mandatory = $true)][string]$PlatformId,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$PackageManifestPath,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryRelativePath,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryName,
    [Parameter(Mandatory = $true)][string]$TargetTriple,
    [Parameter(Mandatory = $true)][string]$ObjectFormat,
    [Parameter(Mandatory = $true)][string]$DebugFormat
  )

  if (!(Test-Objc3cDarwinPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $runtimeLibraryPath = Join-Path $PackageRoot (ConvertTo-Objc3cEvidenceHostPath -RelativePath $RuntimeLibraryRelativePath)
  $identity = Get-Objc3cDarwinMachOIdentity -RepoRoot $PackageRoot -Path $runtimeLibraryPath -IncludeInstallName
  $payload = [ordered]@{
    contract_id = "objc3c.platform.darwin.runtime-library-manifest.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8229
    package_manifest = Get-Objc3cEvidenceRepoRelativePath -RootPath $PackageRoot -TargetPath $PackageManifestPath
    package_root = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $PackageRoot
    target_triple = $TargetTriple
    object_format = $ObjectFormat
    debug_format = $DebugFormat
    runtime_library_ids = @("objc3-runtime")
    runtime_library_names = @($RuntimeLibraryName)
    runtime_library_root_kind = "objc3c-release-darwin-arm64-package-root"
    runtime_library_artifacts = @(
      [ordered]@{
        runtime_library_id = "objc3-runtime"
        artifact = $RuntimeLibraryRelativePath
        source_file_name = $RuntimeLibraryName
        install_required = $true
        identity = $identity
      }
    )
    install_name = $identity.install_name
    linked_libraries = @($identity.linked_libraries)
    rpaths = @($identity.load_commands.rpaths)
    codesign = $identity.codesign
    missing_runtime_behavior = "fail-closed-before-package-install"
    support_truth = $false
    native_execution_claimed = $false
  }

  $packageEvidenceRoot = Join-Path $EvidenceRoot "package"
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path $packageEvidenceRoot "runtime-library-manifest.json") `
    -Payload $payload
  if (Test-Path -LiteralPath $PackageManifestPath -PathType Leaf) {
    Copy-Item `
      -LiteralPath $PackageManifestPath `
      -Destination (Join-Path $packageEvidenceRoot "objc3c-runnable-toolchain-package.json") `
      -Force
  }
}

function Get-Objc3cRuntimeLoadProbeExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$Result
  )

  $outDir = [string]$Result.out_dir
  if ([string]::IsNullOrWhiteSpace($outDir)) {
    return ""
  }
  return Join-Path (Join-Path $RepoRoot (ConvertTo-Objc3cEvidenceHostPath -RelativePath $outDir)) "module.exe"
}

function Write-Objc3cDarwinRuntimeLoadProbeEvidence {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EvidenceRoot = $env:OBJC3C_PLATFORM_EVIDENCE_ROOT,
    [Parameter(Mandatory = $true)][string]$PlatformId,
    [Parameter(Mandatory = $true)][string]$TargetTriple,
    [Parameter(Mandatory = $true)][string]$SummaryPath,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryPath,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryRelativePath,
    [Parameter(Mandatory = $true)][string]$LoaderPathPolicy
  )

  if (!(Test-Objc3cDarwinPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $summary = @{}
  if (Test-Path -LiteralPath $SummaryPath -PathType Leaf) {
    $summary = Get-Content -LiteralPath $SummaryPath -Raw | ConvertFrom-Json
  }

  $runtimeIdentity = Get-Objc3cDarwinMachOIdentity -RepoRoot $RepoRoot -Path $RuntimeLibraryPath -IncludeInstallName
  $results = @($summary.results)
  $executableProbes = New-Object System.Collections.Generic.List[object]
  $resolvedRuntimePaths = New-Object System.Collections.Generic.List[string]
  if (Test-Path -LiteralPath $RuntimeLibraryPath -PathType Leaf) {
    $resolvedRuntimePaths.Add((Resolve-Path -LiteralPath $RuntimeLibraryPath).Path) | Out-Null
  }

  foreach ($result in $results) {
    $exePath = Get-Objc3cRuntimeLoadProbeExecutablePath -RepoRoot $RepoRoot -Result $result
    if ([string]::IsNullOrWhiteSpace($exePath)) {
      continue
    }
    $objectPath = Join-Path (Split-Path -Parent $exePath) (Join-Path "compile" "module.o")
    $exeIdentity = Get-Objc3cDarwinMachOIdentity -RepoRoot $RepoRoot -Path $exePath
    $objectIdentity = Get-Objc3cDarwinMachOIdentity -RepoRoot $RepoRoot -Path $objectPath
    $runtimeReferences = @(
      @($exeIdentity.linked_libraries) |
        Where-Object {
          $_ -eq $RuntimeLibraryRelativePath -or
          $_ -match 'libobjc3-runtime\.dylib' -or
          $_ -match '@rpath/libobjc3-runtime\.dylib'
        }
    )
    $executableProbes.Add([ordered]@{
        fixture = [string]$result.fixture
        object_artifact = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $objectPath
        object_identity = $objectIdentity
        executable = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $exePath
        run_exit = [int]$result.run_exit
        expected_exit = [int]$result.expected_exit
        driver_linker_flags = @($result.driver_linker_flags)
        runtime_library = [string]$result.runtime_library
        runtime_references = @($runtimeReferences)
        runtime_reference_present = @($runtimeReferences).Count -gt 0
        identity = $exeIdentity
      }) | Out-Null
  }

  $linkerFlags = @(
    @($summary.driver_linker_flags) +
    @($results | ForEach-Object { @($_.driver_linker_flags) })
  ) | Where-Object { -not [string]::IsNullOrWhiteSpace([string]$_) } | Sort-Object -Unique

  $loadProbeExitCode = if ([string]$summary.status -eq "PASS" -and @($executableProbes | Where-Object { [bool]$_.runtime_reference_present }).Count -gt 0) {
    0
  } else {
    1
  }
  $payload = [ordered]@{
    contract_id = "objc3c.platform.darwin.runtime-load-link-proof.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8229
    target_triple = $TargetTriple
    status = if ($loadProbeExitCode -eq 0) { "PASS" } else { "INCOMPLETE" }
    execution_summary = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $SummaryPath
    runtime_library_names = @("libobjc3-runtime.dylib")
    runtime_library = $runtimeIdentity
    runtime_library_manifest_path = "tmp/reports/platform-host-evidence/darwin-arm64/package/runtime-library-manifest.json"
    link_command = [string]$summary.link_command
    linker_flags = @($linkerFlags)
    loader_policy = $LoaderPathPolicy
    load_path = @($summary.load_path)
    runtime_load_environment = $summary.runtime_load_environment
    resolved_runtime_paths = @($resolvedRuntimePaths)
    load_probe_exit_code = $loadProbeExitCode
    executable_probes = @($executableProbes)
    support_truth = $false
    native_execution_claimed = $false
  }

  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path (Join-Path $EvidenceRoot "execution") "runtime-load-probe.json") `
    -Payload $payload
}

Export-ModuleMember -Function @(
  "Write-Objc3cDarwinObjectDebugIdentityEvidence",
  "Write-Objc3cDarwinRuntimeLibraryManifestEvidence",
  "Write-Objc3cDarwinRuntimeLoadProbeEvidence"
)
