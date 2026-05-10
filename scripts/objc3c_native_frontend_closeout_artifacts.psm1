$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force
function Write-Objc3cNativeFrontendEdgeCompatibilityArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendCoreFeatureExpansionPath
  )

  if (!(Test-Path -LiteralPath $FrontendCoreFeatureExpansionPath -PathType Leaf)) {
    throw "frontend core feature expansion missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  try {
    $coreFeaturePayload = Get-Content -LiteralPath $FrontendCoreFeatureExpansionPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend core feature expansion is not valid JSON for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $expectedCoreFeatureContractId = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  if ([string]$coreFeaturePayload.contract_id -ne $expectedCoreFeatureContractId) {
    throw "frontend core feature contract id mismatch for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $allowedBackends = @()
  foreach ($backend in @($coreFeaturePayload.backend_routing.allowed_ir_object_backends)) {
    $backendText = [string]$backend
    if (![string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends += $backendText
    }
  }
  if ($allowedBackends.Count -eq 0) {
    throw "frontend core feature expansion allowed_ir_object_backends missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCoreFeatureContractId
      "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    )
    backend_compat = [ordered]@{
      canonical_allowed_backends = $allowedBackends
      alias_to_canonical = [ordered]@{
        "clang" = "clang"
        "clang++" = "clang"
        "clang-cl" = "clang"
        "llvm-direct" = "llvm-direct"
        "llvm_direct" = "llvm-direct"
        "llvmdirect" = "llvm-direct"
        "llvm" = "llvm-direct"
      }
      single_value_flags = @(
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
      )
    }
    invocation_edge_compat = [ordered]@{
      supports_equals_form_flags = @(
        "--out-dir"
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
      )
      supports_boolean_equals_flags = @(
        "--use-cache"
        "--objc3-route-backend-from-capabilities"
      )
      route_flag = "--objc3-route-backend-from-capabilities"
      capability_summary_flag = "--llvm-capabilities-summary"
      fail_closed_exit_code = 2
      disallow_relative_parent_segments = $true
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendEdgeRobustnessArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeCompatibilityPath
  )

  if (!(Test-Path -LiteralPath $FrontendEdgeCompatibilityPath -PathType Leaf)) {
    throw "frontend edge compatibility artifact missing for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  try {
    $edgeCompatPayload = Get-Content -LiteralPath $FrontendEdgeCompatibilityPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend edge compatibility artifact is not valid JSON for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $expectedEdgeCompatContractId = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  if ([string]$edgeCompatPayload.contract_id -ne $expectedEdgeCompatContractId) {
    throw "frontend edge compatibility contract id mismatch for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeCompatContractId
      "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    )
    wrapper_guardrails = [ordered]@{
      wrapper_single_value_flags = @(
        "--use-cache"
        "--out-dir"
      )
      compile_single_value_flags = @(
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
        "--objc3-route-backend-from-capabilities"
      )
      reject_empty_equals_value_flags = @(
        "--out-dir"
        "--emit-prefix"
        "--clang"
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
        "--objc3-route-backend-from-capabilities"
        "--use-cache"
      )
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendDiagnosticsHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeRobustnessPath
  )

  if (!(Test-Path -LiteralPath $FrontendEdgeRobustnessPath -PathType Leaf)) {
    throw "frontend edge robustness artifact missing for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  try {
    $edgeRobustnessPayload = Get-Content -LiteralPath $FrontendEdgeRobustnessPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend edge robustness artifact is not valid JSON for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $expectedEdgeRobustnessContractId = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  if ([string]$edgeRobustnessPayload.contract_id -ne $expectedEdgeRobustnessContractId) {
    throw "frontend edge robustness contract id mismatch for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeRobustnessContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    wrapper_diagnostics = [ordered]@{
      fail_closed_exit_code = 2
      required_error_messages = @(
        "--use-cache can be provided at most once"
        "invalid --use-cache value"
        "--out-dir can be provided at most once"
        "missing value for --out-dir"
        "empty value for --out-dir"
        "missing value for --emit-prefix"
        "empty value for --emit-prefix"
        "missing value for --clang"
        "empty value for --clang"
      )
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendRecoveryDeterminismHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendDiagnosticsHardeningPath
  )

  if (!(Test-Path -LiteralPath $FrontendDiagnosticsHardeningPath -PathType Leaf)) {
    throw "frontend diagnostics hardening artifact missing for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  try {
    $diagnosticsPayload = Get-Content -LiteralPath $FrontendDiagnosticsHardeningPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend diagnostics hardening artifact is not valid JSON for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $expectedDiagnosticsContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
  if ([string]$diagnosticsPayload.contract_id -ne $expectedDiagnosticsContractId) {
    throw "frontend diagnostics hardening contract id mismatch for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedDiagnosticsContractId
      "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    )
    cache_determinism = [ordered]@{
      fail_closed_exit_code = 2
      entry_contract_id = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
      cache_status_tokens = @(
        "cache_hit=true"
        "cache_hit=false"
      )
      required_entry_files = @(
        "files"
        "exit_code.txt"
        "ready.marker"
        "metadata.json"
      )
      recovery_signals = @(
        "cache_recovery=metadata_missing"
        "cache_recovery=metadata_invalid"
        "cache_recovery=metadata_contract_mismatch"
        "cache_recovery=metadata_cache_key_mismatch"
        "cache_recovery=metadata_exit_code_mismatch"
        "cache_recovery=metadata_digest_mismatch"
        "cache_recovery=restore_failed"
      )
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendConformanceMatrixArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendRecoveryDeterminismHardeningPath
  )

  if (!(Test-Path -LiteralPath $FrontendRecoveryDeterminismHardeningPath -PathType Leaf)) {
    throw "frontend recovery determinism hardening artifact missing for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  try {
    $recoveryPayload = Get-Content -LiteralPath $FrontendRecoveryDeterminismHardeningPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend recovery determinism hardening artifact is not valid JSON for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  $expectedRecoveryContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
  if ([string]$recoveryPayload.contract_id -ne $expectedRecoveryContractId) {
    throw "frontend recovery determinism hardening contract id mismatch for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  $cacheModes = @("no-cache", "cache-aware")
  $backendModes = @("default", "clang", "llvm-direct")
  $summaryModes = @("none", "present")
  $acceptRows = New-Object System.Collections.Generic.List[object]
  $caseOrdinal = 1
  foreach ($cacheMode in $cacheModes) {
    foreach ($backendMode in $backendModes) {
      foreach ($summaryMode in $summaryModes) {
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

  $rejectRows = @(
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

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedRecoveryContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
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
    acceptance_matrix = $acceptRows.ToArray()
    rejection_matrix = $rejectRows
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 10) -Encoding utf8
}

function Write-Objc3cNativeFrontendConformanceCorpusArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceMatrixPath
  )

  if (!(Test-Path -LiteralPath $FrontendConformanceMatrixPath -PathType Leaf)) {
    throw "frontend conformance matrix artifact missing for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  try {
    $matrixPayload = Get-Content -LiteralPath $FrontendConformanceMatrixPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend conformance matrix artifact is not valid JSON for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  $expectedMatrixContractId = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
  if ([string]$matrixPayload.contract_id -ne $expectedMatrixContractId) {
    throw "frontend conformance matrix contract id mismatch for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  $acceptRows = New-Object System.Collections.Generic.List[object]
  $acceptOrdinal = 1
  foreach ($row in @($matrixPayload.acceptance_matrix)) {
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
    $compileArgs.Add("tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3")
    $compileArgs.Add("--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out")
    if ($backendMode -ne "default") {
      $compileArgs.Add("--objc3-ir-object-backend=$backendMode")
    }
    if ($summaryMode -eq "present") {
      $compileArgs.Add("--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json")
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

  $rejectRows = @(
    [ordered]@{
      corpus_case_id = "D010-R001"
      matrix_case_id = "D009-R001"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      compile_args = @(
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
        "--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json"
        "--objc3-route-backend-from-capabilities"
        "--objc3-route-backend-from-capabilities"
      )
    }
  )

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedMatrixContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    acceptance_corpus_count = $acceptRows.Count
    rejection_corpus_count = $rejectRows.Count
    corpus_case_count = $acceptRows.Count + $rejectRows.Count
    acceptance_corpus = $acceptRows.ToArray()
    rejection_corpus = $rejectRows
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 12) -Encoding utf8
}

function Write-Objc3cNativeFrontendIntegrationCloseoutArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceCorpusPath
  )

  if (!(Test-Path -LiteralPath $FrontendConformanceCorpusPath -PathType Leaf)) {
    throw "frontend conformance corpus artifact missing for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  try {
    $corpusPayload = Get-Content -LiteralPath $FrontendConformanceCorpusPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend conformance corpus artifact is not valid JSON for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $expectedCorpusContractId = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
  if ([string]$corpusPayload.contract_id -ne $expectedCorpusContractId) {
    throw "frontend conformance corpus contract id mismatch for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $acceptanceCount = [int]$corpusPayload.acceptance_corpus_count
  $rejectionCount = [int]$corpusPayload.rejection_corpus_count
  if ($acceptanceCount -le 0 -or $rejectionCount -le 0) {
    throw "frontend conformance corpus must provide non-empty acceptance and rejection coverage for integration closeout"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCorpusContractId
      "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
      "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    )
    closeout_gate = [ordered]@{
      build_integration_gate_signoff = $true
      invocation_profile_gate_signoff = $true
      corpus_coverage_gate_signoff = $true
      deterministic_fail_closed_exit_code = 2
      acceptance_corpus_count = $acceptanceCount
      rejection_corpus_count = $rejectionCount
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 12) -Encoding utf8
}
Export-ModuleMember -Function @(
  "Write-Objc3cNativeFrontendEdgeCompatibilityArtifact",
  "Write-Objc3cNativeFrontendEdgeRobustnessArtifact",
  "Write-Objc3cNativeFrontendDiagnosticsHardeningArtifact",
  "Write-Objc3cNativeFrontendRecoveryDeterminismHardeningArtifact",
  "Write-Objc3cNativeFrontendConformanceMatrixArtifact",
  "Write-Objc3cNativeFrontendConformanceCorpusArtifact",
  "Write-Objc3cNativeFrontendIntegrationCloseoutArtifact"
)