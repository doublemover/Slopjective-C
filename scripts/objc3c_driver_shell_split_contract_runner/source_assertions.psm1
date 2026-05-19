$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_driver_shell_split_contract_helpers.psm1") -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "assertions.psm1") -Force -DisableNameChecking

function Read-Objc3cDriverShellSplitSourceTexts {
  param([Parameter(Mandatory = $true)]$Config)

  [pscustomobject]@{
    Main = Read-NormalizedText -Path $Config.MainSourcePath
    DriverMainHeader = Read-NormalizedText -Path $Config.DriverMainHeaderPath
    DriverMainImpl = Read-NormalizedText -Path $Config.DriverMainImplPath
    DriverHeader = Read-NormalizedText -Path $Config.DriverHeaderPath
    DriverImpl = Read-NormalizedText -Path $Config.DriverImplPath
    CliOptionsHeader = Read-NormalizedText -Path $Config.CliOptionsHeaderPath
  }
}

function Assert-Objc3cDriverShellSplitFilesExist {
  param([Parameter(Mandatory = $true)]$Config)

  Assert-FileExists -Path $Config.MainSourcePath -Id "source.main.exists" -Description "main source"
  Assert-FileExists -Path $Config.DriverMainHeaderPath -Id "source.driver_main_header.exists" -Description "driver main header"
  Assert-FileExists -Path $Config.DriverMainImplPath -Id "source.driver_main_impl.exists" -Description "driver main implementation"
  Assert-FileExists -Path $Config.DriverHeaderPath -Id "source.driver_header.exists" -Description "driver header"
  Assert-FileExists -Path $Config.DriverImplPath -Id "source.driver_impl.exists" -Description "driver implementation"
  Assert-FileExists -Path $Config.CliOptionsHeaderPath -Id "source.cli_options_header.exists" -Description "cli options header"
  Assert-FileExists -Path $Config.FixturePath -Id "fixture.driver_split_smoke.exists" -Description "driver split smoke fixture"
}

function Get-Objc3cDriverShellSplitProjectIncludes {
  param([Parameter(Mandatory = $true)][string]$MainText)

  @(
    [regex]::Matches($MainText, '(?m)^\s*#\s*include\s+"([^"]+)"\s*$') |
      ForEach-Object { $_.Groups[1].Value }
  )
}

function Assert-Objc3cDriverShellSplitMainShellContract {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)][string]$MainText
  )

  $projectIncludes = @(Get-Objc3cDriverShellSplitProjectIncludes -MainText $MainText)
  $sortedProjectIncludes = @($projectIncludes | Sort-Object)
  $sortedExpectedProjectIncludes = @($Config.ExpectedProjectIncludes | Sort-Object)
  $includeSetMatches = (($sortedProjectIncludes -join "|") -eq ($sortedExpectedProjectIncludes -join "|"))
  Assert-Contract `
    -Condition $includeSetMatches `
    -Id "contract.main.project_includes" `
    -FailureMessage ("main.cpp project includes mismatch: expected={0} actual={1}" -f ($sortedExpectedProjectIncludes -join ","), ($sortedProjectIncludes -join ",")) `
    -PassMessage "main.cpp includes only the driver main shell boundary header" `
    -Evidence @{
      expected = $sortedExpectedProjectIncludes
      actual = $sortedProjectIncludes
    }

  $mainHasOnlyDriverIncludes = @($projectIncludes | Where-Object { $_ -notmatch '^driver/' }).Count -eq 0
  Assert-Contract `
    -Condition $mainHasOnlyDriverIncludes `
    -Id "contract.main.includes_are_driver_only" `
    -FailureMessage "main.cpp includes non-driver project headers" `
    -PassMessage "main.cpp project include boundary stays inside driver/*"

  $flowPattern = '(?s)int\s+main\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*\)\s*\{\s*return\s+RunObjc3DriverMain\(argc,\s*argv\);\s*\}'
  Assert-Objc3cDriverShellSplitRegex `
    -Text $MainText `
    -Pattern $flowPattern `
    -Id "contract.main.delegates_to_driver_main" `
    -FailureMessage "main.cpp no longer delegates directly to RunObjc3DriverMain(argc, argv)" `
    -PassMessage "main.cpp delegates to driver main shell entrypoint"

  $mainForbiddenHits = @($Config.ForbiddenMainTokens | Where-Object { $MainText.IndexOf($_, [System.StringComparison]::Ordinal) -ge 0 })
  Assert-Contract `
    -Condition ($mainForbiddenHits.Count -eq 0) `
    -Id "contract.main.no_compilation_pipeline_calls" `
    -FailureMessage ("main.cpp contains compilation pipeline tokens: {0}" -f ($mainForbiddenHits -join ",")) `
    -PassMessage "main.cpp remains a shell and does not invoke compilation pipeline internals directly" `
    -Evidence @{ forbidden_hits = $mainForbiddenHits }
}

function Assert-Objc3cDriverShellSplitDriverMainContract {
  param([Parameter(Mandatory = $true)]$Texts)

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverMainHeader `
    -Pattern '(?m)^\s*int\s+RunObjc3DriverMain\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*\)\s*;\s*$' `
    -Id "contract.driver_main_header.entry_signature" `
    -FailureMessage "driver main header missing RunObjc3DriverMain(int argc, char **argv) declaration" `
    -PassMessage "driver main header exports RunObjc3DriverMain shell entrypoint"

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverMainImpl `
    -Pattern '(?m)^\s*#\s*include\s+"driver/objc3_cli_options.h"\s*$' `
    -Id "contract.driver_main_impl.imports_cli_options" `
    -FailureMessage "driver main implementation missing include for driver/objc3_cli_options.h" `
    -PassMessage "driver main implementation imports cli options contract surface"

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverMainImpl `
    -Pattern '(?m)^\s*#\s*include\s+"driver/objc3_compilation_driver.h"\s*$' `
    -Id "contract.driver_main_impl.imports_compilation_driver" `
    -FailureMessage "driver main implementation missing include for driver/objc3_compilation_driver.h" `
    -PassMessage "driver main implementation imports compilation driver boundary"

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverMainImpl `
    -Pattern '(?m)^\s*#\s*include\s+"driver/objc3_llvm_capability_routing.h"\s*$' `
    -Id "contract.driver_main_impl.imports_llvm_routing" `
    -FailureMessage "driver main implementation missing include for driver/objc3_llvm_capability_routing.h" `
    -PassMessage "driver main implementation imports LLVM capability routing boundary"

  $driverMainFlowPattern = '(?s)int\s+RunObjc3DriverMain\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*\)\s*\{\s*Objc3CliOptions\s+cli_options;\s*std::string\s+cli_error;\s*if\s*\(!ParseObjc3CliOptions\(argc,\s*argv,\s*cli_options,\s*cli_error\)\)\s*\{\s*std::cerr\s*<<\s*cli_error\s*<<\s*"\\n";\s*return\s+2;\s*\}\s*if\s*\(!ApplyObjc3LLVMCabilityRouting\(cli_options,\s*cli_error\)\)\s*\{\s*std::cerr\s*<<\s*cli_error\s*<<\s*"\\n";\s*return\s+2;\s*\}\s*return\s+RunObjc3CompilationDriver\(cli_options\);\s*\}'
  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverMainImpl `
    -Pattern $driverMainFlowPattern `
    -Id "contract.driver_main_impl.parse_route_delegate_flow" `
    -FailureMessage "driver main implementation no longer matches parse+route fail-exit(2) then RunObjc3CompilationDriver delegation contract" `
    -PassMessage "driver main implementation parse/route/delegate flow matches driver shell split contract"

  Assert-Objc3cDriverShellSplitNoRegex `
    -Text $Texts.DriverMainImpl `
    -Pattern '(?m)^\s*int\s+main\s*\(' `
    -Id "contract.driver_main_impl.no_main_definition" `
    -FailureMessage "driver main implementation defines main(), violating main/driver split boundary" `
    -PassMessage "driver main implementation does not define main()"
}

function Assert-Objc3cDriverShellSplitCompilationDriverContract {
  param([Parameter(Mandatory = $true)]$Texts)

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverHeader `
    -Pattern '(?m)^\s*#\s*include\s+"driver/objc3_cli_options.h"\s*$' `
    -Id "contract.driver_header.imports_cli_options" `
    -FailureMessage "driver header missing include for driver/objc3_cli_options.h" `
    -PassMessage "driver header imports cli options contract surface"

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverHeader `
    -Pattern '(?m)^\s*int\s+RunObjc3CompilationDriver\s*\(\s*const\s+Objc3CliOptions\s*&\s*cli_options\s*\)\s*;\s*$' `
    -Id "contract.driver_header.run_signature" `
    -FailureMessage "driver header missing RunObjc3CompilationDriver(const Objc3CliOptions&) declaration" `
    -PassMessage "driver header exports RunObjc3CompilationDriver contract signature"

  Assert-Objc3cDriverShellSplitNoRegex `
    -Text $Texts.DriverImpl `
    -Pattern '(?m)^\s*int\s+main\s*\(' `
    -Id "contract.driver_impl.no_main_definition" `
    -FailureMessage "driver implementation defines main(), violating main/driver split boundary" `
    -PassMessage "driver implementation does not define main()"

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.DriverImpl `
    -Pattern '(?m)^\s*int\s+RunObjc3CompilationDriver\s*\(\s*const\s+Objc3CliOptions\s*&\s*cli_options\s*\)\s*\{' `
    -Id "contract.driver_impl.run_entry_exists" `
    -FailureMessage "driver implementation missing RunObjc3CompilationDriver entrypoint" `
    -PassMessage "driver implementation exposes RunObjc3CompilationDriver entrypoint"
}

function Assert-Objc3cDriverShellSplitCliOptionsContract {
  param([Parameter(Mandatory = $true)]$Texts)

  Assert-Objc3cDriverShellSplitRegex `
    -Text $Texts.CliOptionsHeader `
    -Pattern '(?m)^\s*bool\s+ParseObjc3CliOptions\s*\(\s*int\s+argc\s*,\s*char\s*\*\*argv\s*,\s*Objc3CliOptions\s*&\s*options\s*,\s*std::string\s*&\s*error\s*\)\s*;\s*$' `
    -Id "contract.cli_options.parse_signature" `
    -FailureMessage "cli options header missing ParseObjc3CliOptions(argc, argv, options, error) declaration" `
    -PassMessage "cli options header exports parse signature consumed by shell"
}

function Invoke-Objc3cDriverShellSplitContractSourceAssertions {
  param([Parameter(Mandatory = $true)]$Config)

  Assert-Objc3cDriverShellSplitFilesExist -Config $Config
  $texts = Read-Objc3cDriverShellSplitSourceTexts -Config $Config
  Assert-Objc3cDriverShellSplitMainShellContract -Config $Config -MainText $texts.Main
  Assert-Objc3cDriverShellSplitDriverMainContract -Texts $texts
  Assert-Objc3cDriverShellSplitCompilationDriverContract -Texts $texts
  Assert-Objc3cDriverShellSplitCliOptionsContract -Texts $texts
}

Export-ModuleMember -Function "Invoke-Objc3cDriverShellSplitContractSourceAssertions"
