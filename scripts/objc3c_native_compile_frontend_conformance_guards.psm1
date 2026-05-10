$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendConformanceMatrix {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  $conformancePath = Resolve-FrontendConformanceMatrixPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $conformancePath -PathType Leaf)) {
    Write-Error "frontend conformance matrix artifact missing at $conformancePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $conformancePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend conformance matrix artifact is not valid JSON at $conformancePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend conformance matrix contract id mismatch in $conformancePath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend conformance matrix missing dependency contract '$requiredContractId' in $conformancePath"
      exit 2
    }
  }

  if ([int]$payload.acceptance_profile_count -le 0) {
    Write-Error "frontend conformance matrix acceptance_profile_count must be positive in $conformancePath"
    exit 2
  }
  if ([int]$payload.rejection_profile_count -le 0) {
    Write-Error "frontend conformance matrix rejection_profile_count must be positive in $conformancePath"
    exit 2
  }

  $acceptanceRows = @($payload.acceptance_matrix)
  $acceptanceProfileSet = @{}
  foreach ($row in $acceptanceRows) {
    $caseId = [string]$row.case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Write-Error "frontend conformance matrix acceptance rows must define case_id and profile_key in $conformancePath"
      exit 2
    }
    if ($expectedResult -ne "accept") {
      Write-Error "frontend conformance matrix acceptance row '$caseId' must declare expected_result='accept' in $conformancePath"
      exit 2
    }
    if ($acceptanceProfileSet.ContainsKey($profileKey)) {
      Write-Error "frontend conformance matrix duplicate acceptance profile '$profileKey' in $conformancePath"
      exit 2
    }
    $acceptanceProfileSet[$profileKey] = $caseId
  }
  if ([int]$payload.acceptance_profile_count -ne $acceptanceRows.Count) {
    Write-Error "frontend conformance matrix acceptance_profile_count mismatch in $conformancePath"
    exit 2
  }

  $expectedProfileSet = @{}
  foreach ($cacheMode in @("no-cache", "cache-aware")) {
    foreach ($backendMode in @("default", "clang", "llvm-direct")) {
      foreach ($summaryMode in @("none", "present")) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $expectedProfileSet[$profileKey] = $true
      }
      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $expectedProfileSet[$profileKey] = $true
    }
  }
  foreach ($expectedProfile in $expectedProfileSet.Keys) {
    if (-not $acceptanceProfileSet.ContainsKey($expectedProfile)) {
      Write-Error "frontend conformance matrix missing acceptance profile '$expectedProfile' in $conformancePath"
      exit 2
    }
  }

  $requiredRejectDiagnostics = @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
  $rejectRows = @($payload.rejection_matrix)
  if ([int]$payload.rejection_profile_count -ne $rejectRows.Count) {
    Write-Error "frontend conformance matrix rejection_profile_count mismatch in $conformancePath"
    exit 2
  }
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectRows) {
    $caseId = [string]$row.case_id
    $expectedResult = [string]$row.expected_result
    $requiredDiagnostic = [string]$row.required_diagnostic
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($requiredDiagnostic)) {
      Write-Error "frontend conformance matrix rejection rows must define case_id and required_diagnostic in $conformancePath"
      exit 2
    }
    if ($expectedResult -ne "reject") {
      Write-Error "frontend conformance matrix rejection row '$caseId' must declare expected_result='reject' in $conformancePath"
      exit 2
    }
    $rejectDiagnosticSet[$requiredDiagnostic] = $true
  }
  foreach ($requiredDiagnostic in $requiredRejectDiagnostics) {
    if (-not $rejectDiagnosticSet.ContainsKey($requiredDiagnostic)) {
      Write-Error "frontend conformance matrix missing rejection diagnostic '$requiredDiagnostic' in $conformancePath"
      exit 2
    }
  }

  $compileArgs = @($EffectiveCompileArgs)
  $cacheMode = if ($null -ne $ParsedArgs -and [bool]$ParsedArgs.use_cache) { "cache-aware" } else { "no-cache" }
  $backendMode = "default"
  $routingMode = "manual"
  $summaryMode = "none"
  $routeEnabled = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (@("clang", "llvm-direct") -notcontains $backendKey) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $backendMode = $backendKey
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (@("clang", "llvm-direct") -notcontains $backendKey) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $backendMode = $backendKey
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $summaryMode = "present"
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryValue = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $summaryMode = "present"
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeEnabled = $true
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $routeEnabled = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
  }

  if ($routeEnabled) {
    $routingMode = "capability-route"
  }
  if ($routingMode -eq "capability-route" -and $summaryMode -ne "present") {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  $invocationProfileKey = "{0}|{1}|{2}|{3}" -f $cacheMode, $backendMode, $routingMode, $summaryMode
  if (-not $acceptanceProfileSet.ContainsKey($invocationProfileKey)) {
    Write-Error "frontend conformance matrix has no acceptance row for invocation profile '$invocationProfileKey' in $conformancePath"
    exit 2
  }

  return [pscustomobject]@{
    conformance_matrix_path = $conformancePath
    profile_key = $invocationProfileKey
    case_id = [string]$acceptanceProfileSet[$invocationProfileKey]
  }
}

function Assert-FrontendConformanceCorpus {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [string]$InvocationProfileKey
  )

  $corpusPath = Resolve-FrontendConformanceCorpusPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $corpusPath -PathType Leaf)) {
    Write-Error "frontend conformance corpus artifact missing at $corpusPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $corpusPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend conformance corpus artifact is not valid JSON at $corpusPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend conformance corpus contract id mismatch in $corpusPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend conformance corpus missing dependency contract '$requiredContractId' in $corpusPath"
      exit 2
    }
  }

  $acceptanceRows = @($payload.acceptance_corpus)
  $rejectionRows = @($payload.rejection_corpus)
  if ([int]$payload.acceptance_corpus_count -ne $acceptanceRows.Count) {
    Write-Error "frontend conformance corpus acceptance_corpus_count mismatch in $corpusPath"
    exit 2
  }
  if ([int]$payload.rejection_corpus_count -ne $rejectionRows.Count) {
    Write-Error "frontend conformance corpus rejection_corpus_count mismatch in $corpusPath"
    exit 2
  }
  if ([int]$payload.corpus_case_count -ne ($acceptanceRows.Count + $rejectionRows.Count)) {
    Write-Error "frontend conformance corpus corpus_case_count mismatch in $corpusPath"
    exit 2
  }
  if ($acceptanceRows.Count -le 0 -or $rejectionRows.Count -le 0) {
    Write-Error "frontend conformance corpus requires non-empty acceptance and rejection corpus in $corpusPath"
    exit 2
  }

  $acceptanceByProfile = @{}
  foreach ($row in $acceptanceRows) {
    $caseId = [string]$row.corpus_case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Write-Error "frontend conformance corpus acceptance rows must define corpus_case_id and profile_key in $corpusPath"
      exit 2
    }
    if ($expectedResult -ne "accept") {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must declare expected_result='accept' in $corpusPath"
      exit 2
    }
    if ($expectedExitCode -ne 0) {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must declare expected_exit_code=0 in $corpusPath"
      exit 2
    }
    if ($compileArgs.Count -le 0) {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must provide compile_args in $corpusPath"
      exit 2
    }
    if (-not $acceptanceByProfile.ContainsKey($profileKey)) {
      $acceptanceByProfile[$profileKey] = New-Object System.Collections.Generic.List[string]
    }
    $acceptanceByProfile[$profileKey].Add($caseId)
  }

  $requiredRejectDiagnostics = @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectionRows) {
    $caseId = [string]$row.corpus_case_id
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $expectedDiagnostic = [string]$row.expected_diagnostic
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($expectedDiagnostic)) {
      Write-Error "frontend conformance corpus rejection rows must define corpus_case_id and expected_diagnostic in $corpusPath"
      exit 2
    }
    if ($expectedResult -ne "reject") {
      Write-Error "frontend conformance corpus rejection row '$caseId' must declare expected_result='reject' in $corpusPath"
      exit 2
    }
    if ($expectedExitCode -ne 2) {
      Write-Error "frontend conformance corpus rejection row '$caseId' must declare expected_exit_code=2 in $corpusPath"
      exit 2
    }
    if ($compileArgs.Count -le 0) {
      Write-Error "frontend conformance corpus rejection row '$caseId' must provide compile_args in $corpusPath"
      exit 2
    }
    $rejectDiagnosticSet[$expectedDiagnostic] = $true
  }
  foreach ($requiredDiagnostic in $requiredRejectDiagnostics) {
    if (-not $rejectDiagnosticSet.ContainsKey($requiredDiagnostic)) {
      Write-Error "frontend conformance corpus missing rejection diagnostic '$requiredDiagnostic' in $corpusPath"
      exit 2
    }
  }

  if ([string]::IsNullOrWhiteSpace($InvocationProfileKey)) {
    Write-Error "frontend conformance corpus invocation profile key is required"
    exit 2
  }
  if (-not $acceptanceByProfile.ContainsKey($InvocationProfileKey)) {
    Write-Error "frontend conformance corpus has no acceptance case for invocation profile '$InvocationProfileKey' in $corpusPath"
    exit 2
  }

  return [pscustomobject]@{
    conformance_corpus_path = $corpusPath
    profile_key = $InvocationProfileKey
    acceptance_case_count = [int]$acceptanceByProfile[$InvocationProfileKey].Count
  }
}

function Assert-FrontendIntegrationCloseout {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $closeoutPath = Resolve-FrontendIntegrationCloseoutPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $closeoutPath -PathType Leaf)) {
    Write-Error "frontend integration closeout artifact missing at $closeoutPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $closeoutPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend integration closeout artifact is not valid JSON at $closeoutPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend integration closeout contract id mismatch in $closeoutPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1",
    "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1",
    "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend integration closeout missing dependency contract '$requiredContractId' in $closeoutPath"
      exit 2
    }
  }

  $closeoutGate = $payload.closeout_gate
  if ($null -eq $closeoutGate) {
    Write-Error "frontend integration closeout closeout_gate metadata missing in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.build_integration_gate_signoff) {
    Write-Error "frontend integration closeout build_integration_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.invocation_profile_gate_signoff) {
    Write-Error "frontend integration closeout invocation_profile_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.corpus_coverage_gate_signoff) {
    Write-Error "frontend integration closeout corpus_coverage_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if ([int]$closeoutGate.deterministic_fail_closed_exit_code -ne 2) {
    Write-Error "frontend integration closeout deterministic_fail_closed_exit_code must be 2 in $closeoutPath"
    exit 2
  }
  if ([int]$closeoutGate.acceptance_corpus_count -le 0 -or [int]$closeoutGate.rejection_corpus_count -le 0) {
    Write-Error "frontend integration closeout acceptance/rejection corpus counts must be positive in $closeoutPath"
    exit 2
  }

  return [pscustomobject]@{
    integration_closeout_path = $closeoutPath
  }
}

Export-ModuleMember -Function @("Assert-FrontendConformanceMatrix", "Assert-FrontendConformanceCorpus", "Assert-FrontendIntegrationCloseout")
