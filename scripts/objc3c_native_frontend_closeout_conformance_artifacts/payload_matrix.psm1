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
