$ErrorActionPreference = "Stop"

function New-Objc3cNativeFrontendConformanceMatrixAcceptanceRows {
  param(
    [Parameter(Mandatory = $true)]
    [string[]]$CacheModes,
    [Parameter(Mandatory = $true)]
    [string[]]$BackendModes,
    [Parameter(Mandatory = $true)]
    [string[]]$SummaryModes
  )

  $acceptRows = New-Object System.Collections.Generic.List[object]
  $caseOrdinal = 1
  foreach ($cacheMode in $CacheModes) {
    foreach ($backendMode in $BackendModes) {
      foreach ($summaryMode in $SummaryModes) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $acceptRows.Add([ordered]@{
          case_id = ("D009-C{0:D3}" -f $caseOrdinal)
          profile_key = $profileKey
          expected_result = "accept"
          cache_mode = $cacheMode
          backend_mode = $backendMode
          routing_mode = "manual"
          capability_summary_mode = $summaryMode
        })
        $caseOrdinal++
      }

      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $acceptRows.Add([ordered]@{
        case_id = ("D009-C{0:D3}" -f $caseOrdinal)
        profile_key = $profileKey
        expected_result = "accept"
        cache_mode = $cacheMode
        backend_mode = $backendMode
        routing_mode = "capability-route"
        capability_summary_mode = "present"
      })
      $caseOrdinal++
    }
  }

  return $acceptRows.ToArray()
}

function New-Objc3cNativeFrontendConformanceMatrixRejectionRows {
  return @(
    [ordered]@{
      case_id = "D009-R001"
      profile_key = "any|any|capability-route|none"
      expected_result = "reject"
      required_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R002"
      profile_key = "any|unsupported-backend|any|any"
      expected_result = "reject"
      required_diagnostic = "unsupported value '<backend>' for --objc3-ir-object-backend"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R003"
      profile_key = "any|any|any|any"
      expected_result = "reject"
      required_diagnostic = "--objc3-ir-object-backend can be provided at most once"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R004"
      profile_key = "any|any|any|path-parent-segment"
      expected_result = "reject"
      required_diagnostic = "--llvm-capabilities-summary must not contain '..' relative segments"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R005"
      profile_key = "any|any|duplicate-route-flag|any"
      expected_result = "reject"
      required_diagnostic = "--objc3-route-backend-from-capabilities can be provided at most once"
      fail_closed_exit_code = 2
    }
  )
}

function New-Objc3cNativeFrontendConformanceMatrixPayload {
  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds
  $dimensions = Get-Objc3cNativeFrontendConformanceProfileDimensions
  $cacheModes = $dimensions.CacheModes
  $backendModes = $dimensions.BackendModes
  $summaryModes = $dimensions.SummaryModes
  $acceptRows = @(
    New-Objc3cNativeFrontendConformanceMatrixAcceptanceRows `
      -CacheModes $cacheModes `
      -BackendModes $backendModes `
      -SummaryModes $summaryModes
  )
  $rejectRows = @(New-Objc3cNativeFrontendConformanceMatrixRejectionRows)

  return [ordered]@{
    contract_id = $contracts.ConformanceMatrix
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.RecoveryDeterminismHardening
      $contracts.EdgeCompatCompletion
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    matrix_dimensions = [ordered]@{
      cache_modes = $cacheModes
      backend_modes = $backendModes
      routing_modes = @("manual", "capability-route")
      capability_summary_modes = $summaryModes
    }
    acceptance_profile_count = $acceptRows.Count
    rejection_profile_count = $rejectRows.Count
    acceptance_matrix = $acceptRows
    rejection_matrix = $rejectRows
  }
}

function New-Objc3cNativeFrontendConformanceCorpusAcceptanceRows {
  param(
    [Parameter(Mandatory = $true)]
    $MatrixPayload
  )

  $pathConstants = Get-Objc3cNativeFrontendConformanceCorpusPathConstants
  $acceptRows = New-Object System.Collections.Generic.List[object]
  $acceptOrdinal = 1
  foreach ($row in @($MatrixPayload.acceptance_matrix)) {
    $profileKey = [string]$row.profile_key
    if ([string]::IsNullOrWhiteSpace($profileKey)) {
      continue
    }
    $segments = $profileKey.Split("|")
    if ($segments.Length -ne 4) {
      continue
    }
    $cacheMode = [string]$segments[0]
    $backendMode = [string]$segments[1]
    $routingMode = [string]$segments[2]
    $summaryMode = [string]$segments[3]
    $compileArgs = New-Object System.Collections.Generic.List[string]
    $compileArgs.Add($pathConstants.FixtureSource)
    $compileArgs.Add("--out-dir=$($pathConstants.OutputDirectory)")
    if ($backendMode -ne "default") {
      $compileArgs.Add("--objc3-ir-object-backend=$backendMode")
    }
    if ($summaryMode -eq "present") {
      $compileArgs.Add("--llvm-capabilities-summary=$($pathConstants.CapabilitiesSummary)")
    }
    if ($routingMode -eq "capability-route") {
      $compileArgs.Add("--objc3-route-backend-from-capabilities")
    }
    $acceptRows.Add([ordered]@{
      corpus_case_id = ("D010-C{0:D3}" -f $acceptOrdinal)
      profile_key = $profileKey
      expected_result = "accept"
      use_cache = ($cacheMode -eq "cache-aware")
      expected_exit_code = 0
      compile_args = $compileArgs.ToArray()
    })
    $acceptOrdinal++
  }

  return $acceptRows.ToArray()
}

function New-Objc3cNativeFrontendConformanceCorpusRejectionRows {
  $pathConstants = Get-Objc3cNativeFrontendConformanceCorpusPathConstants
  $fixtureSource = $pathConstants.FixtureSource
  $outputDirectoryArg = "--out-dir=$($pathConstants.OutputDirectory)"
  $capabilitiesSummaryArg = "--llvm-capabilities-summary=$($pathConstants.CapabilitiesSummary)"

  return @(
    [ordered]@{
      corpus_case_id = "D010-R001"
      matrix_case_id = "D009-R001"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      compile_args = @(
        $fixtureSource
        $outputDirectoryArg
        "--objc3-route-backend-from-capabilities"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R002"
      matrix_case_id = "D009-R002"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "unsupported value '<backend>' for --objc3-ir-object-backend"
      compile_args = @(
        $fixtureSource
        $outputDirectoryArg
        "--objc3-ir-object-backend=unsupported-backend"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R003"
      matrix_case_id = "D009-R003"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-ir-object-backend can be provided at most once"
      compile_args = @(
        $fixtureSource
        $outputDirectoryArg
        "--objc3-ir-object-backend=clang"
        "--objc3-ir-object-backend=llvm-direct"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R004"
      matrix_case_id = "D009-R004"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--llvm-capabilities-summary must not contain '..' relative segments"
      compile_args = @(
        $fixtureSource
        $outputDirectoryArg
        "--llvm-capabilities-summary=../outside/capabilities.json"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R005"
      matrix_case_id = "D009-R005"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities can be provided at most once"
      compile_args = @(
        $fixtureSource
        $outputDirectoryArg
        $capabilitiesSummaryArg
        "--objc3-route-backend-from-capabilities"
        "--objc3-route-backend-from-capabilities"
      )
    }
  )
}

function New-Objc3cNativeFrontendConformanceCorpusPayload {
  param(
    [Parameter(Mandatory = $true)]
    $MatrixPayload
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds
  $acceptRows = @(New-Objc3cNativeFrontendConformanceCorpusAcceptanceRows -MatrixPayload $MatrixPayload)
  $rejectRows = @(New-Objc3cNativeFrontendConformanceCorpusRejectionRows)

  return [ordered]@{
    contract_id = $contracts.ConformanceCorpus
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.ConformanceMatrix
      $contracts.EdgeCompatCompletion
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    acceptance_corpus_count = $acceptRows.Count
    rejection_corpus_count = $rejectRows.Count
    corpus_case_count = ($acceptRows.Count + $rejectRows.Count)
    acceptance_corpus = $acceptRows
    rejection_corpus = $rejectRows
  }
}

function New-Objc3cNativeFrontendIntegrationCloseoutPayload {
  param(
    [int]$AcceptanceCount,
    [int]$RejectionCount
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds

  return [ordered]@{
    contract_id = $contracts.IntegrationCloseout
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.ConformanceCorpus
      $contracts.ConformanceMatrix
      $contracts.RecoveryDeterminismHardening
    )
    closeout_gate = [ordered]@{
      build_integration_gate_signoff = $true
      invocation_profile_gate_signoff = $true
      corpus_coverage_gate_signoff = $true
      deterministic_fail_closed_exit_code = 2
      acceptance_corpus_count = $AcceptanceCount
      rejection_corpus_count = $RejectionCount
    }
  }
}
