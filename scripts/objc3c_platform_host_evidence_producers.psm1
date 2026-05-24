Set-StrictMode -Version Latest

function Test-Objc3cDarwinPlatformEvidenceEnabled {
  param(
    [string]$EvidenceRoot = "",
    [string]$PlatformId = ""
  )

  return (-not [string]::IsNullOrWhiteSpace($EvidenceRoot)) -and $PlatformId -eq "darwin-arm64"
}

function Test-Objc3cLinuxPlatformEvidenceEnabled {
  param(
    [string]$EvidenceRoot = "",
    [string]$PlatformId = ""
  )

  return (-not [string]::IsNullOrWhiteSpace($EvidenceRoot)) -and $PlatformId -eq "linux-x64"
}

function Get-Objc3cPlatformEvidenceReportPath {
  param(
    [Parameter(Mandatory = $true)][string]$PlatformId,
    [Parameter(Mandatory = $true)][string]$Suffix
  )

  return ("tmp/reports/platform-host-evidence/{0}/{1}" -f $PlatformId, $Suffix)
}

function Get-Objc3cLinuxEvidenceRecordId {
  param([Parameter(Mandatory = $true)][string]$Field)

  switch ($Field) {
    "object_identity" { return "objc3c.object-identity.linux-x64.release.missing" }
    "debug_identity" { return "objc3c.debug-identity.linux-x64.release.missing" }
    "package_install_identity" { return "objc3c.package-install-identity.linux-x64.release.missing" }
    "runtime_load_link_proof" { return "objc3c.runtime-load-link.linux-x64.release.missing" }
    default { throw ("unknown Linux evidence record field: " + $Field) }
  }
}

function New-Objc3cEvidenceSourceArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string[]]$Paths = @()
  )

  $artifacts = New-Object System.Collections.Generic.List[object]
  foreach ($path in @($Paths)) {
    if ([string]::IsNullOrWhiteSpace($path)) {
      continue
    }
    $artifacts.Add((Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $path)) | Out-Null
  }
  return @($artifacts)
}

function Get-Objc3cLinuxPackageRootLayout {
  return @(
    "bin/objc3c-native",
    "lib/libobjc3-runtime.so",
    "include/objc3/runtime"
  )
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
    [string[]]$Arguments = @(),
    [hashtable]$Environment = @{}
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
  foreach ($key in @($Environment.Keys)) {
    if ([string]::IsNullOrWhiteSpace([string]$key)) {
      continue
    }
    $processInfo.Environment[[string]$key] = [string]$Environment[$key]
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

function Get-Objc3cEvidenceObjectProperty {
  param(
    [object]$InputObject,
    [Parameter(Mandatory = $true)][string]$Name,
    [object]$DefaultValue = $null
  )

  if ($null -eq $InputObject) {
    return $DefaultValue
  }
  $property = $InputObject.PSObject.Properties[$Name]
  if ($null -eq $property) {
    return $DefaultValue
  }
  return $property.Value
}

function ConvertTo-Objc3cEvidenceEnvironmentHashtable {
  param([object]$EnvironmentObject)

  $environment = @{}
  if ($null -eq $EnvironmentObject) {
    return $environment
  }
  if ($EnvironmentObject -is [System.Collections.IDictionary]) {
    foreach ($key in @($EnvironmentObject.Keys)) {
      if (-not [string]::IsNullOrWhiteSpace([string]$key)) {
        $environment[[string]$key] = [string]$EnvironmentObject[$key]
      }
    }
    return $environment
  }
  foreach ($property in @($EnvironmentObject.PSObject.Properties)) {
    if (-not [string]::IsNullOrWhiteSpace([string]$property.Name)) {
      $environment[[string]$property.Name] = [string]$property.Value
    }
  }
  return $environment
}

function Get-Objc3cLinuxElfHeaderField {
  param(
    [object]$ReadElfHeader,
    [Parameter(Mandatory = $true)][string]$FieldName
  )

  if ($null -eq $ReadElfHeader -or [int]$ReadElfHeader.exit_code -ne 0) {
    return ""
  }
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$ReadElfHeader.stdout))) {
    if ($line -match ('^\s*' + [regex]::Escape($FieldName) + ':\s+(.+?)\s*$')) {
      return $Matches[1].Trim()
    }
  }
  return ""
}

function Get-Objc3cLinuxElfDynamicEntries {
  param([object]$ReadElfDynamic)

  $needed = New-Object System.Collections.Generic.List[string]
  $rpaths = New-Object System.Collections.Generic.List[string]
  $runpaths = New-Object System.Collections.Generic.List[string]
  $entries = New-Object System.Collections.Generic.List[object]
  $soname = ""
  if ($null -eq $ReadElfDynamic -or [int]$ReadElfDynamic.exit_code -ne 0) {
    return [ordered]@{
      needed_libraries = @()
      soname = ""
      rpaths = @()
      runpaths = @()
      entries = @()
    }
  }

  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$ReadElfDynamic.stdout))) {
    if ($line -match '\(([^)]+)\)\s+(.+)$') {
      $tag = $Matches[1].Trim()
      $value = $Matches[2].Trim()
      $entries.Add([ordered]@{ tag = $tag; value = $value }) | Out-Null
      if ($tag -eq "NEEDED" -and $value -match '\[(.+?)\]') {
        $needed.Add($Matches[1]) | Out-Null
      } elseif ($tag -eq "SONAME" -and $value -match '\[(.+?)\]') {
        $soname = $Matches[1]
      } elseif ($tag -eq "RPATH" -and $value -match '\[(.+?)\]') {
        $rpaths.Add($Matches[1]) | Out-Null
      } elseif ($tag -eq "RUNPATH" -and $value -match '\[(.+?)\]') {
        $runpaths.Add($Matches[1]) | Out-Null
      }
    }
  }

  return [ordered]@{
    needed_libraries = @($needed | Sort-Object -Unique)
    soname = $soname
    rpaths = @($rpaths | Sort-Object -Unique)
    runpaths = @($runpaths | Sort-Object -Unique)
    entries = @($entries)
  }
}

function Get-Objc3cLinuxBuildIds {
  param([object]$ReadElfNotes)

  if ($null -eq $ReadElfNotes -or [int]$ReadElfNotes.exit_code -ne 0) {
    return @()
  }
  $buildIds = New-Object System.Collections.Generic.List[string]
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$ReadElfNotes.stdout))) {
    if ($line -match 'Build ID:\s+([0-9A-Fa-f]+)') {
      $buildIds.Add($Matches[1].ToLowerInvariant()) | Out-Null
    }
  }
  return @($buildIds | Sort-Object -Unique)
}

function Get-Objc3cLinuxDebugSections {
  param([object]$ReadElfSections)

  if ($null -eq $ReadElfSections -or [int]$ReadElfSections.exit_code -ne 0) {
    return @()
  }
  $sections = New-Object System.Collections.Generic.List[string]
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$ReadElfSections.stdout))) {
    if ($line -match '\]\s+(\.(?:z)?debug[^\s]*)\s+') {
      $sections.Add($Matches[1]) | Out-Null
    }
  }
  return @($sections | Sort-Object -Unique)
}

function Get-Objc3cLinuxLddLibraries {
  param([object]$Ldd)

  if ($null -eq $Ldd -or -not [bool]$Ldd.available) {
    return @()
  }
  $libraries = New-Object System.Collections.Generic.List[object]
  foreach ($line in @(Split-Objc3cEvidenceLines -Text ([string]$Ldd.stdout))) {
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed -match '^([^\s]+)\s+=>\s+not found$') {
      $libraries.Add([ordered]@{
          name = $Matches[1]
          resolved_path = ""
          found = $false
        }) | Out-Null
    } elseif ($trimmed -match '^([^\s]+)\s+=>\s+(.+?)\s+\(0x[0-9A-Fa-f]+\)$') {
      $libraries.Add([ordered]@{
          name = $Matches[1]
          resolved_path = $Matches[2]
          found = $true
        }) | Out-Null
    } elseif ($trimmed -match '^(/[^\s]+)\s+\(0x[0-9A-Fa-f]+\)$') {
      $libraries.Add([ordered]@{
          name = Split-Path -Leaf $Matches[1]
          resolved_path = $Matches[1]
          found = $true
        }) | Out-Null
    }
  }
  return @($libraries)
}

function Get-Objc3cLinuxElfIdentity {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$Path,
    [string]$ExpectedMachinePattern = 'X86-64|x86-64|Advanced Micro Devices X86-64'
  )

  $digest = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $Path
  if (-not [bool]$digest.exists) {
    return [ordered]@{
      artifact = $digest
      expected_format = "ELF"
      expected_arch = "x86_64"
      elf_present = $false
      expected_arch_present = $false
      dynamic_section = [ordered]@{
        needed_libraries = @()
        soname = ""
        rpaths = @()
        runpaths = @()
        entries = @()
      }
      notes = [ordered]@{
        build_ids = @()
      }
      sections = [ordered]@{
        debug_sections = @()
        dwarf_present = $false
      }
    }
  }

  $file = Invoke-Objc3cPlatformEvidenceTool -Tool "file" -Arguments @("-b", $Path)
  $readelfHeader = Invoke-Objc3cPlatformEvidenceTool -Tool "readelf" -Arguments @("-h", $Path)
  $readelfDynamic = Invoke-Objc3cPlatformEvidenceTool -Tool "readelf" -Arguments @("-d", $Path)
  $readelfNotes = Invoke-Objc3cPlatformEvidenceTool -Tool "readelf" -Arguments @("-n", $Path)
  $readelfSections = Invoke-Objc3cPlatformEvidenceTool -Tool "readelf" -Arguments @("-S", $Path)
  $objdumpPrivateHeaders = Invoke-Objc3cPlatformEvidenceTool -Tool "objdump" -Arguments @("-p", $Path)
  $machine = Get-Objc3cLinuxElfHeaderField -ReadElfHeader $readelfHeader -FieldName "Machine"
  $elfClass = Get-Objc3cLinuxElfHeaderField -ReadElfHeader $readelfHeader -FieldName "Class"
  $elfType = Get-Objc3cLinuxElfHeaderField -ReadElfHeader $readelfHeader -FieldName "Type"
  $elfData = Get-Objc3cLinuxElfHeaderField -ReadElfHeader $readelfHeader -FieldName "Data"
  $osAbi = Get-Objc3cLinuxElfHeaderField -ReadElfHeader $readelfHeader -FieldName "OS/ABI"
  $dynamic = Get-Objc3cLinuxElfDynamicEntries -ReadElfDynamic $readelfDynamic
  $buildIds = @(Get-Objc3cLinuxBuildIds -ReadElfNotes $readelfNotes)
  $debugSections = @(Get-Objc3cLinuxDebugSections -ReadElfSections $readelfSections)

  return [ordered]@{
    artifact = $digest
    expected_format = "ELF"
    expected_arch = "x86_64"
    elf_present = (([string]$file.stdout) -match 'ELF') -or (([string]$readelfHeader.stdout) -match 'ELF Header')
    expected_arch_present = ($machine -match $ExpectedMachinePattern) -or (([string]$file.stdout) -match 'x86-64')
    elf_class = $elfClass
    elf_type = $elfType
    machine = $machine
    data_encoding = $elfData
    os_abi = $osAbi
    file = $file
    elf_header = $readelfHeader
    dynamic_section = [ordered]@{
      tool = $readelfDynamic
      needed_libraries = @($dynamic.needed_libraries)
      soname = $dynamic.soname
      rpaths = @($dynamic.rpaths)
      runpaths = @($dynamic.runpaths)
      entries = @($dynamic.entries)
    }
    notes = [ordered]@{
      tool = $readelfNotes
      build_ids = @($buildIds)
    }
    sections = [ordered]@{
      tool = $readelfSections
      debug_sections = @($debugSections)
      dwarf_present = @($debugSections).Count -gt 0
    }
    objdump_private_headers = $objdumpPrivateHeaders
  }
}

function New-Objc3cLinuxDwarfIdentity {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$BinaryPath
  )

  $identity = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $BinaryPath
  return [ordered]@{
    binary = $identity.artifact
    identity = $identity
    build_ids = @($identity.notes.build_ids)
    debug_sections = @($identity.sections.debug_sections)
    dwarf_present = [bool]$identity.sections.dwarf_present
    build_id_present = @($identity.notes.build_ids).Count -gt 0
  }
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

  if (Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId) {
    Write-Objc3cLinuxObjectDebugIdentityEvidence @PSBoundParameters
    return
  }

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

  if (Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId) {
    Write-Objc3cLinuxRuntimeLibraryManifestEvidence @PSBoundParameters
    return
  }

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

  if (Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId) {
    Write-Objc3cLinuxRuntimeLoadProbeEvidence @PSBoundParameters
    return
  }

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

function Write-Objc3cLinuxObjectDebugIdentityEvidence {
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

  if (!(Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $artifacts = [ordered]@{
    native_executable = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $NativeExecutablePath
    frontend_c_api_runner = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $CapiRunnerPath
    runtime_library = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $RuntimeLibraryPath
  }
  $artifactValues = @($artifacts.Values)
  $objectStatus = if (@($artifactValues | Where-Object { -not [bool]$_.artifact.exists -or -not [bool]$_.elf_present -or -not [bool]$_.expected_arch_present }).Count -eq 0) {
    "GENERATED_ELF_X86_64_IDENTITY"
  } else {
    "GENERATED_ELF_X86_64_IDENTITY_INCOMPLETE"
  }
  $objectPass = $objectStatus -eq "GENERATED_ELF_X86_64_IDENTITY"
  $buildSummaryExists = Test-Path -LiteralPath $BuildSummaryPath -PathType Leaf
  $objectGeneratedStatus = if (-not $buildSummaryExists) {
    "missing-source-generated-fail-closed"
  } elseif ($objectPass) {
    "generated-host-artifact-present"
  } else {
    "identity-mismatch-generated-fail-closed"
  }

  $objectPayload = [ordered]@{
    contract_id = "objc3c.platform.hosted-object-identity.generated.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8228
    record_id = Get-Objc3cLinuxEvidenceRecordId -Field "object_identity"
    generated_report_path = Get-Objc3cPlatformEvidenceReportPath -PlatformId $PlatformId -Suffix "build/object-identity.json"
    source_summary_path = "tmp/build-objc3c-native/native_build_summary.json"
    reviewed_source_required = $true
    support_truth = $false
    promotion_allowed_from_generated_evidence = $false
    status = $objectGeneratedStatus
    expected_identity = [ordered]@{
      target_platform_id = $PlatformId
      target_triple = $TargetTriple
      arch = "x64"
      object_format = "ELF"
    }
    actual_identity = [ordered]@{
      target_platform_id = if ($objectPass) { $PlatformId } else { "" }
      target_triple = if ($objectPass) { $TargetTriple } else { "" }
      object_format = if ($objectPass) { "ELF" } else { "" }
      producer_observed_object_format = $ObjectFormat
    }
    build_artifacts = [ordered]@{
      native_executable = $artifacts.native_executable.artifact
      frontend_c_api_runner = $artifacts.frontend_c_api_runner.artifact
      runtime_library = $artifacts.runtime_library.artifact
    }
    producer_evidence = [ordered]@{
      contract_id = "objc3c.platform.linux.object-identity.v1"
      status = $objectStatus
      artifacts = $artifacts
    }
    source_artifacts = New-Objc3cEvidenceSourceArtifacts -RepoRoot $RepoRoot -Paths @($BuildSummaryPath)
  }
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path (Join-Path $EvidenceRoot "build") "object-identity.json") `
    -Payload $objectPayload

  $debugRecords = [ordered]@{
    native_executable = New-Objc3cLinuxDwarfIdentity -RepoRoot $RepoRoot -BinaryPath $NativeExecutablePath
    frontend_c_api_runner = New-Objc3cLinuxDwarfIdentity -RepoRoot $RepoRoot -BinaryPath $CapiRunnerPath
    runtime_library = New-Objc3cLinuxDwarfIdentity -RepoRoot $RepoRoot -BinaryPath $RuntimeLibraryPath
  }
  $debugValues = @($debugRecords.Values)
  $debugStatus = if (@($debugValues | Where-Object { -not [bool]$_.binary.exists -or -not [bool]$_.identity.elf_present -or -not [bool]$_.identity.expected_arch_present -or -not [bool]$_.dwarf_present }).Count -eq 0) {
    "GENERATED_DWARF_ELF_IDENTITY"
  } else {
    "GENERATED_DWARF_ELF_IDENTITY_INCOMPLETE"
  }
  $debugPass = $debugStatus -eq "GENERATED_DWARF_ELF_IDENTITY"
  $debugGeneratedStatus = if (-not $buildSummaryExists) {
    "missing-source-generated-fail-closed"
  } elseif ($debugPass) {
    "generated-host-artifact-present"
  } else {
    "identity-mismatch-generated-fail-closed"
  }
  $debugPayload = [ordered]@{
    contract_id = "objc3c.platform.hosted-debug-identity.generated.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8228
    record_id = Get-Objc3cLinuxEvidenceRecordId -Field "debug_identity"
    generated_report_path = Get-Objc3cPlatformEvidenceReportPath -PlatformId $PlatformId -Suffix "build/debug-identity.json"
    source_summary_path = "tmp/build-objc3c-native/native_build_summary.json"
    reviewed_source_required = $true
    support_truth = $false
    promotion_allowed_from_generated_evidence = $false
    status = $debugGeneratedStatus
    expected_identity = [ordered]@{
      target_platform_id = $PlatformId
      target_triple = $TargetTriple
      arch = "x64"
      debug_format = "DWARF"
    }
    actual_identity = [ordered]@{
      target_platform_id = if ($debugPass) { $PlatformId } else { "" }
      target_triple = if ($debugPass) { $TargetTriple } else { "" }
      debug_format = if ($debugPass) { "DWARF" } else { "" }
      producer_observed_debug_format = $DebugFormat
    }
    debug_artifacts = [ordered]@{
      native_executable = $debugRecords.native_executable.binary
      frontend_c_api_runner = $debugRecords.frontend_c_api_runner.binary
      runtime_library = $debugRecords.runtime_library.binary
    }
    producer_evidence = [ordered]@{
      contract_id = "objc3c.platform.linux.debug-identity.v1"
      status = $debugStatus
      artifacts = $debugRecords
    }
    source_artifacts = New-Objc3cEvidenceSourceArtifacts -RepoRoot $RepoRoot -Paths @($BuildSummaryPath)
  }
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path (Join-Path $EvidenceRoot "build") "debug-identity.json") `
    -Payload $debugPayload
}

function Write-Objc3cLinuxRuntimeLibraryManifestEvidence {
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

  if (!(Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $runtimeLibraryPath = Join-Path $PackageRoot (ConvertTo-Objc3cEvidenceHostPath -RelativePath $RuntimeLibraryRelativePath)
  $identity = Get-Objc3cLinuxElfIdentity -RepoRoot $PackageRoot -Path $runtimeLibraryPath
  $manifestStatus = if ([bool]$identity.artifact.exists -and [bool]$identity.elf_present -and [bool]$identity.expected_arch_present) {
    "GENERATED_ELF_RUNTIME_LIBRARY_MANIFEST"
  } else {
    "GENERATED_ELF_RUNTIME_LIBRARY_MANIFEST_INCOMPLETE"
  }
  $packageManifestPayload = @{}
  if (Test-Path -LiteralPath $PackageManifestPath -PathType Leaf) {
    $packageManifestPayload = Get-Content -LiteralPath $PackageManifestPath -Raw | ConvertFrom-Json
  }
  $packageTargetPlatformId = [string](Get-Objc3cEvidenceObjectProperty -InputObject $packageManifestPayload -Name "target_platform_id" -DefaultValue "")
  $runtimeArtifact = Get-Objc3cEvidenceFileDigest -RootPath $PackageRoot -TargetPath $runtimeLibraryPath
  $packageManifestArtifact = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $PackageManifestPath
  $runtimeGeneratedStatus = if (-not [bool]$packageManifestArtifact.exists -or -not [bool]$runtimeArtifact.exists) {
    "missing-source-generated-fail-closed"
  } elseif (-not [string]::IsNullOrWhiteSpace($packageTargetPlatformId) -and $packageTargetPlatformId -ne $PlatformId) {
    "package-target-mismatch-generated-fail-closed"
  } elseif ([bool]$identity.elf_present -and [bool]$identity.expected_arch_present) {
    "generated-host-artifact-present"
  } else {
    "missing-source-generated-fail-closed"
  }
  $payload = [ordered]@{
    contract_id = "objc3c.platform.hosted-runtime-library-manifest.generated.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8228
    generated_report_path = Get-Objc3cPlatformEvidenceReportPath -PlatformId $PlatformId -Suffix "package/runtime-library-manifest.json"
    source_package_manifest_path = "artifacts/package/objc3c-runnable-toolchain-package.json"
    support_truth = $false
    native_execution_claimed = $false
    promotion_allowed_from_generated_evidence = $false
    status = $runtimeGeneratedStatus
    target_platform_id = $PlatformId
    source_package_target_platform_id = $packageTargetPlatformId
    target_triple = $TargetTriple
    runtime_library_kind = "shared"
    runtime_library_names = @($RuntimeLibraryName)
    runtime_library_artifacts = @($runtimeArtifact)
    loader_path_policy = "ELF rpath, RUNPATH, or package-root loader resolution must be proven before support"
    package_root = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $PackageRoot
    package_root_layout = Get-Objc3cLinuxPackageRootLayout
    package_manifest_artifact = $packageManifestArtifact
    producer_evidence = [ordered]@{
      contract_id = "objc3c.platform.linux.runtime-library-manifest.v1"
      status = $manifestStatus
      object_format = $ObjectFormat
      debug_format = $DebugFormat
      runtime_library_artifacts = @(
        [ordered]@{
          runtime_library_id = "objc3-runtime"
          artifact = $RuntimeLibraryRelativePath
          source_file_name = $RuntimeLibraryName
          install_required = $true
          identity = $identity
        }
      )
      soname = $identity.dynamic_section.soname
      needed_libraries = @($identity.dynamic_section.needed_libraries)
      rpaths = @($identity.dynamic_section.rpaths)
      runpaths = @($identity.dynamic_section.runpaths)
      build_ids = @($identity.notes.build_ids)
      missing_runtime_behavior = "fail-closed-before-package-install"
    }
    source_artifacts = New-Objc3cEvidenceSourceArtifacts -RepoRoot $RepoRoot -Paths @($PackageManifestPath)
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

function Write-Objc3cLinuxInstallReceiptEvidence {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EvidenceRoot = $env:OBJC3C_PLATFORM_EVIDENCE_ROOT,
    [Parameter(Mandatory = $true)][string]$PlatformId,
    [Parameter(Mandatory = $true)][string]$InstallReceiptPath,
    [string]$SourceSummaryPath = ""
  )

  if (!(Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $digest = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $InstallReceiptPath
  $receiptPayload = @{}
  if ([bool]$digest.exists) {
    $receiptPayload = Get-Content -LiteralPath $InstallReceiptPath -Raw | ConvertFrom-Json
  }
  $receiptPlatform = [string](Get-Objc3cEvidenceObjectProperty -InputObject $receiptPayload -Name "target_platform_id" -DefaultValue "")
  $packageRuntimeModel = Get-Objc3cEvidenceObjectProperty -InputObject $receiptPayload -Name "package_runtime_model" -DefaultValue $null
  if ([string]::IsNullOrWhiteSpace($receiptPlatform)) {
    $receiptPlatform = [string](Get-Objc3cEvidenceObjectProperty -InputObject $packageRuntimeModel -Name "target_platform_id" -DefaultValue "")
  }
  $status = if (-not [bool]$digest.exists) {
    "LINUX_INSTALL_RECEIPT_MISSING"
  } elseif (-not [string]::IsNullOrWhiteSpace($receiptPlatform) -and $receiptPlatform -ne $PlatformId) {
    "LINUX_INSTALL_RECEIPT_TARGET_MISMATCH"
  } else {
    "LINUX_INSTALL_RECEIPT_ROUTED"
  }
  $sourceSummaryPath = if ([string]::IsNullOrWhiteSpace($SourceSummaryPath)) { "tmp/reports/package-channels/end-to-end-summary.json" } else { $SourceSummaryPath }
  $packageManifestPath = Join-Path $RepoRoot (ConvertTo-Objc3cEvidenceHostPath -RelativePath "artifacts/package/objc3c-runnable-toolchain-package.json")
  $packageChannelsSummaryPath = Join-Path $RepoRoot (ConvertTo-Objc3cEvidenceHostPath -RelativePath "tmp/reports/package-channels/end-to-end-summary.json")
  $generatedStatus = if (-not [bool]$digest.exists) {
    "missing-source-generated-fail-closed"
  } elseif (-not [string]::IsNullOrWhiteSpace($receiptPlatform) -and $receiptPlatform -ne $PlatformId) {
    "install-receipt-target-mismatch-generated-fail-closed"
  } else {
    "generated-host-artifact-present"
  }
  $payload = [ordered]@{
    contract_id = "objc3c.platform.hosted-install-receipt.generated.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8228
    record_id = Get-Objc3cLinuxEvidenceRecordId -Field "package_install_identity"
    generated_report_path = Get-Objc3cPlatformEvidenceReportPath -PlatformId $PlatformId -Suffix "install/install-receipt.json"
    source_summary_path = "tmp/reports/package-channels/end-to-end-summary.json"
    source_install_receipt_path = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $InstallReceiptPath
    reviewed_source_required = $true
    support_truth = $false
    native_execution_claimed = $false
    promotion_allowed_from_generated_evidence = $false
    status = $generatedStatus
    target_platform_id = $PlatformId
    target_triple = "x86_64-unknown-linux-gnu"
    package_root = ""
    package_root_layout = Get-Objc3cLinuxPackageRootLayout
    package_manifest = "artifacts/package/objc3c-runnable-toolchain-package.json"
    package_manifest_artifact = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $packageManifestPath
    package_channels_summary_artifact = Get-Objc3cEvidenceFileDigest -RootPath $RepoRoot -TargetPath $packageChannelsSummaryPath
    source_install_receipt_artifact = $digest
    source_install_receipt = $receiptPayload
    producer_evidence = [ordered]@{
      contract_id = "objc3c.platform.linux.install-receipt.v1"
      status = $status
      target_platform_id = $receiptPlatform
      source_summary = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $sourceSummaryPath
    }
  }
  $installEvidenceRoot = Join-Path $EvidenceRoot "install"
  New-Item -ItemType Directory -Force -Path $installEvidenceRoot | Out-Null
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path $installEvidenceRoot "install-receipt.json") `
    -Payload $payload
  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path $installEvidenceRoot "install-receipt-routing.json") `
    -Payload $payload
}

function Write-Objc3cLinuxRuntimeLoadProbeEvidence {
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

  if (!(Test-Objc3cLinuxPlatformEvidenceEnabled -EvidenceRoot $EvidenceRoot -PlatformId $PlatformId)) {
    return
  }

  $summary = @{}
  if (Test-Path -LiteralPath $SummaryPath -PathType Leaf) {
    $summary = Get-Content -LiteralPath $SummaryPath -Raw | ConvertFrom-Json
  }

  $runtimeIdentity = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $RuntimeLibraryPath
  $resultsValue = Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "results" -DefaultValue @()
  $results = @($resultsValue)
  $loadEnvironment = ConvertTo-Objc3cEvidenceEnvironmentHashtable -EnvironmentObject (Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "runtime_load_environment" -DefaultValue $null)
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
    $exeIdentity = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $exePath
    $objectIdentity = Get-Objc3cLinuxElfIdentity -RepoRoot $RepoRoot -Path $objectPath
    $ldd = if (Test-Path -LiteralPath $exePath -PathType Leaf) {
      Invoke-Objc3cPlatformEvidenceTool -Tool "ldd" -Arguments @($exePath) -Environment $loadEnvironment
    } else {
      [ordered]@{
        tool = "ldd"
        arguments = @($exePath)
        available = $false
        exit_code = -1
        stdout = ""
        stderr = "executable-missing"
      }
    }
    $lddLibraries = @(Get-Objc3cLinuxLddLibraries -Ldd $ldd)
    foreach ($library in @($lddLibraries | Where-Object { [bool]$_.found -and [string]$_.name -match 'libobjc3-runtime\.so' })) {
      if (-not [string]::IsNullOrWhiteSpace([string]$library.resolved_path)) {
        $resolvedRuntimePaths.Add([string]$library.resolved_path) | Out-Null
      }
    }
    $neededRuntimeReferences = @(
      @($exeIdentity.dynamic_section.needed_libraries) |
        Where-Object {
          $_ -eq $RuntimeLibraryRelativePath -or
          $_ -match 'libobjc3-runtime\.so'
        }
    )
    $lddRuntimeReferences = @(
      $lddLibraries |
        Where-Object {
          [string]$_.name -match 'libobjc3-runtime\.so'
        }
    )
    $runExit = [int](Get-Objc3cEvidenceObjectProperty -InputObject $result -Name "run_exit" -DefaultValue -2147483648)
    $expectedExit = [int](Get-Objc3cEvidenceObjectProperty -InputObject $result -Name "expected_exit" -DefaultValue -2147483648)
    $driverLinkerFlags = @(Get-Objc3cEvidenceObjectProperty -InputObject $result -Name "driver_linker_flags" -DefaultValue @())
    $executableProbes.Add([ordered]@{
        fixture = [string](Get-Objc3cEvidenceObjectProperty -InputObject $result -Name "fixture" -DefaultValue "")
        object_artifact = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $objectPath
        object_identity = $objectIdentity
        executable = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $exePath
        run_exit = $runExit
        expected_exit = $expectedExit
        driver_linker_flags = @($driverLinkerFlags)
        runtime_library = [string](Get-Objc3cEvidenceObjectProperty -InputObject $result -Name "runtime_library" -DefaultValue "")
        needed_runtime_references = @($neededRuntimeReferences)
        loader_runtime_references = @($lddRuntimeReferences)
        runtime_reference_present = @($neededRuntimeReferences).Count -gt 0
        runtime_resolved_by_loader = ([bool]$ldd.available -and [int]$ldd.exit_code -eq 0 -and @($lddRuntimeReferences | Where-Object { [bool]$_.found }).Count -gt 0)
        ldd = $ldd
        identity = $exeIdentity
      }) | Out-Null
  }

  $summaryFlags = @(Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "driver_linker_flags" -DefaultValue @())
  $linkerFlags = @(
    @($summaryFlags) +
    @($results | ForEach-Object { @(Get-Objc3cEvidenceObjectProperty -InputObject $_ -Name "driver_linker_flags" -DefaultValue @()) })
  ) | Where-Object { -not [string]::IsNullOrWhiteSpace([string]$_) } | Sort-Object -Unique

  $summaryStatus = [string](Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "status" -DefaultValue "")
  $probeFailures = @(
    $executableProbes |
      Where-Object {
        -not [bool]$_.identity.artifact.exists -or
        -not [bool]$_.identity.elf_present -or
        -not [bool]$_.identity.expected_arch_present -or
        -not [bool]$_.runtime_reference_present -or
        -not [bool]$_.runtime_resolved_by_loader -or
        [int]$_.run_exit -ne [int]$_.expected_exit
      }
  )
  $loadProbeExitCode = if (
    $summaryStatus -eq "PASS" -and
    [bool]$runtimeIdentity.artifact.exists -and
    [bool]$runtimeIdentity.elf_present -and
    [bool]$runtimeIdentity.expected_arch_present -and
    @($executableProbes).Count -gt 0 -and
    @($probeFailures).Count -eq 0
  ) {
    0
  } else {
    1
  }
  $skipReason = [string](Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "skip_reason" -DefaultValue "")
  $nativeGeneratedStatus = if (-not (Test-Path -LiteralPath $SummaryPath -PathType Leaf)) {
    "missing-source-generated-fail-closed"
  } elseif (-not [string]::IsNullOrWhiteSpace($skipReason) -or $summaryStatus -in @("UNAVAILABLE", "SKIP", "SKIPPED")) {
    "runtime-load-unavailable-generated-fail-closed"
  } elseif ($loadProbeExitCode -eq 0 -and $summaryStatus -eq "PASS") {
    "generated-host-artifact-present"
  } else {
    "runtime-load-failed-generated-fail-closed"
  }
  $payload = [ordered]@{
    contract_id = "objc3c.platform.hosted-runtime-load-probe.generated.v1"
    schema_version = 1
    platform_id = $PlatformId
    issue_ref = 8228
    record_id = Get-Objc3cLinuxEvidenceRecordId -Field "runtime_load_link_proof"
    generated_report_path = Get-Objc3cPlatformEvidenceReportPath -PlatformId $PlatformId -Suffix "execution/runtime-load-probe.json"
    source_summary_path = "tmp/reports/objc3c-native-execution-smoke/summary.json"
    reviewed_source_required = $true
    support_truth = $false
    native_execution_claimed = $false
    promotion_allowed_from_generated_evidence = $false
    status = $nativeGeneratedStatus
    target_platform_id = $PlatformId
    target_triple = $TargetTriple
    runtime_library_names = @("libobjc3-runtime.so")
    runtime_library = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $RuntimeLibraryPath
    runtime_library_kind = "shared"
    runtime_load_environment_variable = "LD_LIBRARY_PATH"
    loader_path_policy = $LoaderPathPolicy
    resolved_runtime_paths = @($resolvedRuntimePaths | Sort-Object -Unique)
    driver_linker_flags = @($linkerFlags)
    hosted_execution_status = ""
    native_execution_status = $summaryStatus
    skip_reason = $skipReason
    load_probe_exit_code = $loadProbeExitCode
    producer_evidence = [ordered]@{
      contract_id = "objc3c.platform.linux.runtime-load-link-proof.v1"
      status = if ($loadProbeExitCode -eq 0) { "PASS" } else { "INCOMPLETE" }
      execution_summary = Get-Objc3cEvidenceRepoRelativePath -RootPath $RepoRoot -TargetPath $SummaryPath
      runtime_library_identity = $runtimeIdentity
      runtime_library_manifest_path = "tmp/reports/platform-host-evidence/linux-x64/package/runtime-library-manifest.json"
      link_command = [string](Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "link_command" -DefaultValue "")
      loader_policy = $LoaderPathPolicy
      load_path = @(Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "load_path" -DefaultValue @())
      runtime_load_environment = Get-Objc3cEvidenceObjectProperty -InputObject $summary -Name "runtime_load_environment" -DefaultValue @{}
      executable_probes = @($executableProbes)
    }
    source_artifacts = New-Objc3cEvidenceSourceArtifacts -RepoRoot $RepoRoot -Paths @($SummaryPath)
  }

  Write-Objc3cPlatformEvidenceJson `
    -Path (Join-Path (Join-Path $EvidenceRoot "execution") "runtime-load-probe.json") `
    -Payload $payload
}

Export-ModuleMember -Function @(
  "Write-Objc3cDarwinObjectDebugIdentityEvidence",
  "Write-Objc3cDarwinRuntimeLibraryManifestEvidence",
  "Write-Objc3cDarwinRuntimeLoadProbeEvidence",
  "Write-Objc3cLinuxObjectDebugIdentityEvidence",
  "Write-Objc3cLinuxRuntimeLibraryManifestEvidence",
  "Write-Objc3cLinuxInstallReceiptEvidence",
  "Write-Objc3cLinuxRuntimeLoadProbeEvidence"
)
