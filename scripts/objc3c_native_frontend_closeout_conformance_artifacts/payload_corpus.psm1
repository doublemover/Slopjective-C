$ErrorActionPreference = "Stop"

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
