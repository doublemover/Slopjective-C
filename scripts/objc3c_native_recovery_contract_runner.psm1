$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers.psm1") -Force -DisableNameChecking

$script:RecoveryContractContext = $null

function Set-RecoveryContractContext {
  param(
    [string]$RepoRoot,
    [string]$OutDir,
    [string]$CompilerPath,
    [string]$CompileWrapperScript,
    [string]$PowerShellExecutable
  )

  $script:RecoveryContractContext = [pscustomobject]@{
    RepoRoot = $RepoRoot
    OutDir = $OutDir
    CompilerPath = $CompilerPath
    CompileWrapperScript = $CompileWrapperScript
    PowerShellExecutable = $PowerShellExecutable
  }
}

function Get-RecoveryContractContext {
  if ($null -eq $script:RecoveryContractContext) {
    throw "contract FAIL: recovery contract runner context was not initialized"
  }

  return $script:RecoveryContractContext
}

function Invoke-Objc3cNativeWithRecovery {
  param(
    [string[]]$Arguments,
    [switch]$UseCompileWrapper
  )

  $context = Get-RecoveryContractContext
  $exe = $context.CompilerPath
  $compileWrapperScript = $context.CompileWrapperScript
  $pwsh = $context.PowerShellExecutable

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "contract FAIL: native compiler executable missing at $exe"
  }

  if ($UseCompileWrapper) {
    if (!(Test-Path -LiteralPath $compileWrapperScript -PathType Leaf)) {
      throw "contract FAIL: compile wrapper missing at $compileWrapperScript"
    }
    $null = & $pwsh -NoProfile -ExecutionPolicy Bypass -File $compileWrapperScript @Arguments
    return [int]$LASTEXITCODE
  }

  $null = & $exe @Arguments
  return [int]$LASTEXITCODE
}

function Invoke-ContractCase {
  param(
    [string]$Source,
    [string]$CaseName,
    [switch]$RequireLl,
    [switch]$RequireCompileProvenance,
    [switch]$UseCompileWrapper,
    [string[]]$ExtraArgs = @(),
    [string[]]$RequiredLlTokens = @("define i32 @objc3c_entry"),
    [string[]]$ForbiddenLlTokens = @(),
    [string[]]$RequiredManifestTokens = @(),
    [switch]$RequireObjc3ManifestSurface
  )

  $context = Get-RecoveryContractContext
  $repoRoot = $context.RepoRoot
  $outDir = $context.OutDir

  $resolvedSource = if ([System.IO.Path]::IsPathRooted($Source)) { $Source } else { Join-Path $repoRoot $Source }
  if (-not (Test-Path -LiteralPath $resolvedSource -PathType Leaf)) {
    throw "contract FAIL: missing source fixture for $CaseName at $resolvedSource"
  }

  $run1 = Join-Path $outDir ($CaseName + "_run1")
  $run2 = Join-Path $outDir ($CaseName + "_run2")
  New-Item -ItemType Directory -Force -Path $run1 | Out-Null
  New-Item -ItemType Directory -Force -Path $run2 | Out-Null

  $compileArgsRun1 = @($resolvedSource, "--out-dir", $run1, "--emit-prefix", "module") + $ExtraArgs
  $compileArgsRun2 = @($resolvedSource, "--out-dir", $run2, "--emit-prefix", "module") + $ExtraArgs

  $exitRun1 = Invoke-Objc3cNativeWithRecovery -Arguments $compileArgsRun1 -UseCompileWrapper:$UseCompileWrapper
  if ($exitRun1 -ne 0) { throw "contract FAIL: compile failed for $CaseName run1" }
  $exitRun2 = Invoke-Objc3cNativeWithRecovery -Arguments $compileArgsRun2 -UseCompileWrapper:$UseCompileWrapper
  if ($exitRun2 -ne 0) { throw "contract FAIL: compile failed for $CaseName run2" }

  $manifest1 = Get-Content -LiteralPath (Join-Path $run1 "module.manifest.json") -Raw
  $manifest2 = Get-Content -LiteralPath (Join-Path $run2 "module.manifest.json") -Raw
  $diag1 = Get-Content -LiteralPath (Join-Path $run1 "module.diagnostics.txt") -Raw
  $diag2 = Get-Content -LiteralPath (Join-Path $run2 "module.diagnostics.txt") -Raw

  if ($manifest1 -ne $manifest2) { throw "contract FAIL: manifest drift across replay for $CaseName" }
  if ($diag1 -ne $diag2) { throw "contract FAIL: diagnostics drift across replay for $CaseName" }
  if ($RequireCompileProvenance) {
    $provenance1 = Assert-CompileOutputProvenance -CaseName "$CaseName run1" -RunDir $run1
    $provenance2 = Assert-CompileOutputProvenance -CaseName "$CaseName run2" -RunDir $run2
    if ($provenance1 -ne $provenance2) { throw "contract FAIL: compile provenance drift across replay for $CaseName" }
  }
  foreach ($token in $RequiredManifestTokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($manifest1.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "contract FAIL: missing manifest token '$token' in run1 for $CaseName"
    }
    if ($manifest2.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "contract FAIL: missing manifest token '$token' in run2 for $CaseName"
    }
  }
  if ($RequireObjc3ManifestSurface) {
    Assert-Objc3ManifestPipelineSurface -ManifestText $manifest1 -CaseName "$CaseName run1"
    Assert-Objc3ManifestPipelineSurface -ManifestText $manifest2 -CaseName "$CaseName run2"
  }

  $objPath = Join-Path $run1 "module.obj"
  if (!(Test-Path -LiteralPath $objPath -PathType Leaf)) { throw "contract FAIL: missing object artifact for $CaseName" }
  $objSize = (Get-Item -LiteralPath $objPath).Length
  if ($objSize -le 0) { throw "contract FAIL: empty object artifact for $CaseName" }

  if ($RequireLl) {
    $ll1 = Get-Content -LiteralPath (Join-Path $run1 "module.ll") -Raw
    $ll2 = Get-Content -LiteralPath (Join-Path $run2 "module.ll") -Raw
    $ll1Code = (($ll1 -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
    $ll2Code = (($ll2 -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
    $ll1Entrypoints = Get-EntrypointLlSurface -LlText $ll1Code
    $ll2Entrypoints = Get-EntrypointLlSurface -LlText $ll2Code
    if ($ll1 -ne $ll2) { throw "contract FAIL: LLVM IR drift across replay for $CaseName" }
    foreach ($token in $RequiredLlTokens) {
      if ([string]::IsNullOrWhiteSpace($token)) {
        continue
      }
      if ($ll1Code.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
        throw "contract FAIL: missing LLVM IR token '$token' in run1 for $CaseName"
      }
      if ($ll2Code.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
        throw "contract FAIL: missing LLVM IR token '$token' in run2 for $CaseName"
      }
    }
    foreach ($token in $ForbiddenLlTokens) {
      if ([string]::IsNullOrWhiteSpace($token)) {
        continue
      }
      if ($ll1Entrypoints.IndexOf($token, [System.StringComparison]::Ordinal) -ge 0) {
        throw "contract FAIL: forbidden LLVM IR token '$token' present in run1 for $CaseName"
      }
      if ($ll2Entrypoints.IndexOf($token, [System.StringComparison]::Ordinal) -ge 0) {
        throw "contract FAIL: forbidden LLVM IR token '$token' present in run2 for $CaseName"
      }
    }
    Write-Output "$CaseName`_deterministic_ir=true"
  }

  Write-Output "$CaseName`_deterministic_manifest=true"
  if ($RequireCompileProvenance) {
    Write-Output "$CaseName`_deterministic_compile_provenance=true"
  }
  Write-Output "$CaseName`_deterministic_diagnostics=true"
  Write-Output "$CaseName`_object_size=$objSize"
}

function Invoke-Objc3cNativeRecoveryContract {
  param(
    [string]$RepoRoot,
    [string]$OutDir,
    [string]$CompilerPath,
    [string]$CompileWrapperScript,
    [string]$PowerShellExecutable,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  Set-RecoveryContractContext `
    -RepoRoot $RepoRoot `
    -OutDir $OutDir `
    -CompilerPath $CompilerPath `
    -CompileWrapperScript $CompileWrapperScript `
    -PowerShellExecutable $PowerShellExecutable

  $recoveryFixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery"
  $helloObjc3Source = Join-Path $RepoRoot "tests/tooling/fixtures/native/hello.objc3"
  $positiveFixtureDir = Join-Path $recoveryFixtureRoot "positive"
  $negativeFixtureDir = Join-Path $recoveryFixtureRoot "negative"

  Invoke-ContractCase -Source "tests/tooling/fixtures/native/hello.m" -CaseName "objc_baseline"
  Invoke-ContractCase -Source "tests/tooling/fixtures/native/hello.objc3" -CaseName "objc3_frontend" -RequireLl -RequireObjc3ManifestSurface
  Invoke-ContractCase -Source "tests/tooling/fixtures/native/hello.objc3" -CaseName "objc3_frontend_wrapper_launch_contract" -RequireLl -RequireCompileProvenance -RequireObjc3ManifestSurface -UseCompileWrapper
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/function_return_annotation_bool.objc3" `
    -CaseName "objc3_typed_signature_bool_return" `
    -RequireLl `
    -RequiredLlTokens @("define i1 @is_zero(i32 %arg0)", "call i1 @is_zero(i32 0)", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""name"":""is_zero""", """return"":""bool""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/typed_i32_bool.objc3" `
    -CaseName "objc3_typed_signature_mixed_abi" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @select_i32(i32 %arg0, i32 %arg1, i1 %arg2)", "zext i1 %arg2 to i32", "call i32 @select_i32(i32", "define i32 @objc3c_entry") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/dispatch/message_send_six_args.objc3" `
    -CaseName "objc3_dispatch_surface_custom_symbol_argslots" `
    -RequireLl `
    -ExtraArgs @("--objc3-runtime-dispatch-symbol", "objc3_runtime_dispatch_lane_c_surface", "--objc3-max-message-args", "6") `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_lane_c_surface(i32, ptr, i32, i32, i32, i32, i32, i32)", "call i32 @objc3_runtime_dispatch_lane_c_surface(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_symbol"":""objc3_runtime_dispatch_lane_c_surface""", """runtime_dispatch_arg_slots"":6", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_short_circuit.objc3" `
    -CaseName "objc3_dispatch_nil_receiver_short_circuit" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_unary_short_circuit.objc3" `
    -CaseName "objc3_dispatch_nil_receiver_unary_short_circuit" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_semantic_compatibility.objc3" `
    -CaseName "objc3_dispatch_nil_receiver_semantic_compatibility" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "icmp eq i32", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_mixed_expression_flow.objc3" `
    -CaseName "objc3_dispatch_nil_receiver_mixed_expression_flow" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "icmp eq i32", "cond_true_", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_direct_nil_receiver_elision.objc3" `
    -CaseName "objc3_dispatch_direct_nil_receiver_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_direct_nil_receiver_keyword_elision.objc3" `
    -CaseName "objc3_dispatch_direct_nil_receiver_keyword_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_unary_elision.objc3" `
    -CaseName "objc3_dispatch_nil_bound_identifier_unary_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_keyword_elision.objc3" `
    -CaseName "objc3_dispatch_nil_bound_identifier_keyword_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_alias_identifier_elision.objc3" `
    -CaseName "objc3_dispatch_nil_alias_identifier_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_conditional_receiver_elision.objc3" `
    -CaseName "objc3_dispatch_nil_conditional_receiver_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "cond_true_", "cond_false_", "cond_merge_", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_elision.objc3" `
    -CaseName "objc3_dispatch_nil_immutable_global_identifier_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_mutable_global_identifier_non_elided.objc3" `
    -CaseName "objc3_dispatch_nil_mutable_global_identifier_non_elided" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_global_identifier_post_call_non_elided.objc3" `
    -CaseName "objc3_dispatch_nil_global_identifier_post_call_non_elided" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_post_pure_call_elision.objc3" `
    -CaseName "objc3_dispatch_nil_immutable_global_identifier_post_pure_call_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_post_pure_prototype_call_elision.objc3" `
    -CaseName "objc3_dispatch_nil_immutable_global_identifier_post_pure_prototype_call_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_mixed_flow.objc3" `
    -CaseName "objc3_dispatch_nil_bound_identifier_mixed_flow" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_pre_reassignment_elision.objc3" `
    -CaseName "objc3_dispatch_nil_bound_identifier_pre_reassignment_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_post_assignment_nil_elision.objc3" `
    -CaseName "objc3_dispatch_nil_bound_identifier_post_assignment_nil_elision" `
    -RequireLl `
    -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
    -ForbiddenLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_nonzero_receiver_fast_path.objc3" `
    -CaseName "objc3_dispatch_numeric_nonzero_receiver_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_bound_identifier_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_constant_expression_receiver_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_constant_expression_receiver_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_global_identifier_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_const_expr_identifier_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_global_const_expr_identifier_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_call_non_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_global_identifier_post_call_non_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_pure_call_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_global_identifier_post_pure_call_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_pure_prototype_call_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_global_identifier_post_pure_prototype_call_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_extern_call_non_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_global_identifier_post_extern_call_non_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_mutable_global_identifier_non_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_mutable_global_identifier_non_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_immutable_global_identifier_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_immutable_global_identifier_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_unary_receiver_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_unary_receiver_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_short_circuit_receiver_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_short_circuit_receiver_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_const_expr_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_bound_identifier_const_expr_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_post_assignment_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_bound_identifier_post_assignment_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_invalidation_non_fast_path.objc3" `
    -CaseName "objc3_dispatch_nonzero_bound_identifier_invalidation_non_fast_path" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface
  Invoke-ContractCase `
    -Source "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_zero_receiver_non_elided.objc3" `
    -CaseName "objc3_dispatch_numeric_zero_receiver_non_elided" `
    -RequireLl `
    -RequiredLlTokens @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry") `
    -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
    -RequireObjc3ManifestSurface

  $invalidDispatchOutDir = Join-Path $OutDir "objc3_invalid_dispatch_symbol"
  New-Item -ItemType Directory -Force -Path $invalidDispatchOutDir | Out-Null
  $resolvedInvalidDispatchSource = [System.IO.Path]::GetFullPath($helloObjc3Source)
  if (-not (Test-Path -LiteralPath $resolvedInvalidDispatchSource -PathType Leaf)) {
    throw "contract FAIL: missing invalid-dispatch source fixture at $resolvedInvalidDispatchSource"
  }
  $invalidDispatchExit = Invoke-Objc3cNativeWithRecovery -Arguments @(
    $resolvedInvalidDispatchSource,
    "--out-dir",
    $invalidDispatchOutDir,
    "--emit-prefix",
    "module",
    "--objc3-runtime-dispatch-symbol",
    "9invalid_symbol"
  )
  if ($invalidDispatchExit -ne 2) {
    throw "contract FAIL: invalid runtime dispatch symbol should fail with exit 2 (got $invalidDispatchExit)"
  }
  Write-Output "objc3_invalid_dispatch_symbol_rejected=true"
  Write-Output "objc3_invalid_dispatch_symbol_exit_code=$invalidDispatchExit"

  $positiveFixtures = Get-RecoveryFixtures -Directory $positiveFixtureDir -FixtureKind "positive native recovery" -Extensions @(".objc3")
  Assert-RecoveryFixtureClass -Fixtures $positiveFixtures -FixtureKind "positive native recovery"
  $negativeFixtures = Get-RecoveryFixtures -Directory $negativeFixtureDir -FixtureKind "negative native recovery" -Extensions @(".objc3")
  Assert-RecoveryFixtureClass -Fixtures $negativeFixtures -FixtureKind "negative native recovery"
  $recoveryFixtureEntries = @(
    $positiveFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "positive"
        file = $_
        relative_path = [System.IO.Path]::GetRelativePath($RepoRoot, $_.FullName).Replace("\", "/")
      }
    }
  ) + @(
    $negativeFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "negative"
        file = $_
        relative_path = [System.IO.Path]::GetRelativePath($RepoRoot, $_.FullName).Replace("\", "/")
      }
    }
  )
  $selectedRecoveryEntries = Select-RecoveryFixtureEntries `
    -Entries $recoveryFixtureEntries `
    -FixtureListPath $FixtureList `
    -FixtureGlobPattern $FixtureGlob `
    -ShardIndexValue $ShardIndex `
    -ShardCountValue $ShardCount `
    -LimitValue $Limit `
    -RepoRoot $RepoRoot
  $selectedPositiveFixtures = @($selectedRecoveryEntries | Where-Object { $_.kind -eq "positive" } | ForEach-Object { $_.file })
  $selectedNegativeFixtures = @($selectedRecoveryEntries | Where-Object { $_.kind -eq "negative" } | ForEach-Object { $_.file })
  Write-Output ("selection: positive={0} negative={1}" -f $selectedPositiveFixtures.Count, $selectedNegativeFixtures.Count)
  foreach ($fixture in $selectedPositiveFixtures) {
    $source = $fixture.FullName
    $caseName = Get-FixtureCaseName -Prefix "recovery_positive" -FixturePath $source
    $caseOutDir = Join-Path $OutDir $caseName
    New-Item -ItemType Directory -Force -Path $caseOutDir | Out-Null

    $positiveExit = Invoke-Objc3cNativeWithRecovery -Arguments @($source, "--out-dir", $caseOutDir, "--emit-prefix", "module")
    if ($positiveExit -ne 0) {
      throw "contract FAIL: positive fixture compile failed for $source with exit $positiveExit"
    }

    $objPath = Join-Path $caseOutDir "module.obj"
    if (!(Test-Path -LiteralPath $objPath -PathType Leaf)) { throw "contract FAIL: missing object artifact for positive fixture $source" }
    $objSize = (Get-Item -LiteralPath $objPath).Length
    if ($objSize -le 0) { throw "contract FAIL: empty object artifact for positive fixture $source" }

    $manifestPath = Join-Path $caseOutDir "module.manifest.json"
    if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) { throw "contract FAIL: missing manifest artifact for positive fixture $source" }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw
    if ([string]::IsNullOrWhiteSpace($manifest)) {
      throw "contract FAIL: empty manifest artifact for positive fixture $source"
    }
    Assert-Objc3ManifestPipelineSurface -ManifestText $manifest -CaseName $source
    Write-Output "$caseName`_compiled=true"
    Write-Output "$caseName`_object_size=$objSize"
  }

  $negativeFixtureDiagnosticTokenContracts = @{
    "negative_pure_definition_impure_global_write.objc3" = @(
      "error:7:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: global-write",
      "cause-site:8:3",
      "detail:global-write@8:3"
    )
    "negative_pure_definition_impure_transitive_call.objc3" = @(
      "error:12:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: impure-callee:bump",
      "cause-site:13:10",
      "detail:global-write@8:3"
    )
    "negative_pure_definition_impure_unannotated_extern_call.objc3" = @(
      "error:7:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: unannotated-extern-call:ext_impure",
      "cause-site:8:10",
      "detail:unannotated-extern-call:ext_impure@8:10"
    )
    "negative_pure_definition_impure_message_send.objc3" = @(
      "error:5:9",
      "O3S215",
      "pure contract violation",
      "declared 'pure' has side effects",
      "cause: message-send",
      "cause-site:6:10",
      "detail:message-send@6:10"
    )
  }
  foreach ($fixture in $selectedNegativeFixtures) {
    $source = $fixture.FullName
    $caseName = Get-FixtureCaseName -Prefix "recovery_negative" -FixturePath $source
    $run1 = Join-Path $OutDir ($caseName + "_run1")
    $run2 = Join-Path $OutDir ($caseName + "_run2")
    New-Item -ItemType Directory -Force -Path $run1 | Out-Null
    New-Item -ItemType Directory -Force -Path $run2 | Out-Null

    $exit1 = Invoke-Objc3cNativeWithRecovery -Arguments @($source, "--out-dir", $run1, "--emit-prefix", "module")
    if ($exit1 -eq 0) {
      throw "contract FAIL: negative fixture unexpectedly compiled for $source"
    }
    $diagPath1 = Join-Path $run1 "module.diagnostics.txt"
    if (!(Test-Path -LiteralPath $diagPath1 -PathType Leaf)) { throw "contract FAIL: missing diagnostics artifact for negative fixture $source run1" }
    $diag1 = Get-Content -LiteralPath $diagPath1 -Raw
    if ([string]::IsNullOrWhiteSpace($diag1)) {
      throw "contract FAIL: empty diagnostics artifact for negative fixture $source run1"
    }

    $exit2 = Invoke-Objc3cNativeWithRecovery -Arguments @($source, "--out-dir", $run2, "--emit-prefix", "module")
    if ($exit2 -eq 0) {
      throw "contract FAIL: negative fixture unexpectedly compiled for $source on replay"
    }
    $diagPath2 = Join-Path $run2 "module.diagnostics.txt"
    if (!(Test-Path -LiteralPath $diagPath2 -PathType Leaf)) { throw "contract FAIL: missing diagnostics artifact for negative fixture $source run2" }
    $diag2 = Get-Content -LiteralPath $diagPath2 -Raw
    if ([string]::IsNullOrWhiteSpace($diag2)) {
      throw "contract FAIL: empty diagnostics artifact for negative fixture $source run2"
    }

    if ($exit1 -ne $exit2) {
      throw "contract FAIL: negative fixture exit-code drift across replay for $source ($exit1 vs $exit2)"
    }
    if ($diag1 -ne $diag2) {
      throw "contract FAIL: negative fixture diagnostics drift across replay for $source"
    }

    $fixtureLeaf = [System.IO.Path]::GetFileName($source)
    if ($negativeFixtureDiagnosticTokenContracts.ContainsKey($fixtureLeaf)) {
      $requiredTokens = @($negativeFixtureDiagnosticTokenContracts[$fixtureLeaf])
      foreach ($token in $requiredTokens) {
        if ($diag1.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
          throw "contract FAIL: missing negative fixture diagnostic token '$token' for $source run1"
        }
        if ($diag2.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
          throw "contract FAIL: missing negative fixture diagnostic token '$token' for $source run2"
        }
      }
      Write-Output "$caseName`_diagnostic_tokens_verified=true"
    }

    Write-Output "$caseName`_fails=true"
    Write-Output "$caseName`_exit_code=$exit1"
    Write-Output "$caseName`_deterministic_diagnostics=true"
  }

  Write-Output "status: PASS"
  Write-Output ("out_dir: " + [System.IO.Path]::GetRelativePath($RepoRoot, $OutDir).Replace("\", "/"))
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativeRecoveryContract"
)
