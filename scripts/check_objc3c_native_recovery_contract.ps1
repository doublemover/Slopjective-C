$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$outDir = Join-Path $repoRoot "artifacts/compilation/objc3c-native/contract_check"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"

& powershell -NoProfile -ExecutionPolicy Bypass -File $buildScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$recoveryFixtureRoot = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery"
$positiveFixtureDir = Join-Path $recoveryFixtureRoot "positive"
$negativeFixtureDir = Join-Path $recoveryFixtureRoot "negative"

function Assert-Objc3ManifestPipelineSurface {
  param(
    [string]$ManifestText,
    [string]$CaseName
  )

  try {
    if ($PSVersionTable.PSVersion.Major -ge 6) {
      $manifest = $ManifestText | ConvertFrom-Json -Depth 8
    } else {
      $manifest = $ManifestText | ConvertFrom-Json
    }
  } catch {
    throw "contract FAIL: invalid manifest JSON for $CaseName"
  }

  if ($null -eq $manifest.frontend -or $null -eq $manifest.frontend.pipeline) {
    throw "contract FAIL: missing frontend.pipeline surface for $CaseName"
  }

  $pipeline = $manifest.frontend.pipeline
  if ($null -eq $pipeline.stages) {
    throw "contract FAIL: missing frontend.pipeline.stages for $CaseName"
  }
  if ($null -eq $pipeline.semantic_surface) {
    throw "contract FAIL: missing frontend.pipeline.semantic_surface for $CaseName"
  }

  foreach ($stageName in @("lexer", "parser", "semantic")) {
    $stage = $pipeline.stages.$stageName
    if ($null -eq $stage) {
      throw "contract FAIL: missing frontend.pipeline.stages.$stageName for $CaseName"
    }
    if ($null -eq $stage.diagnostics) {
      throw "contract FAIL: missing frontend.pipeline.stages.$stageName.diagnostics for $CaseName"
    }
    if ([int]$stage.diagnostics -ne 0) {
      throw "contract FAIL: expected zero stage diagnostics for successful compile ($CaseName stage=$stageName value=$($stage.diagnostics))"
    }
  }

  if ([bool]$pipeline.semantic_skipped) {
    throw "contract FAIL: frontend.pipeline.semantic_skipped must be false for successful .objc3 compile ($CaseName)"
  }

  $declaredGlobals = @($manifest.globals).Count
  $declaredFunctions = @($manifest.functions).Count
  $surface = $pipeline.semantic_surface
  if ([int]$surface.declared_globals -ne $declaredGlobals) {
    throw "contract FAIL: semantic surface declared_globals mismatch for $CaseName"
  }
  if ([int]$surface.declared_functions -ne $declaredFunctions) {
    throw "contract FAIL: semantic surface declared_functions mismatch for $CaseName"
  }
  if ([int]$surface.resolved_global_symbols -ne $declaredGlobals) {
    throw "contract FAIL: semantic surface resolved_global_symbols mismatch for $CaseName"
  }
  if ([int]$surface.resolved_function_symbols -ne $declaredFunctions) {
    throw "contract FAIL: semantic surface resolved_function_symbols mismatch for $CaseName"
  }

  if ($null -eq $surface.function_signature_surface) {
    throw "contract FAIL: missing frontend.pipeline.semantic_surface.function_signature_surface for $CaseName"
  }
  $signatureSurface = $surface.function_signature_surface
  foreach ($field in @("scalar_return_i32", "scalar_return_bool", "scalar_return_void", "scalar_param_i32", "scalar_param_bool")) {
    if ($null -eq $signatureSurface.$field) {
      throw "contract FAIL: missing frontend.pipeline.semantic_surface.function_signature_surface.$field for $CaseName"
    }
  }

  $functions = @($manifest.functions)
  $computedReturnI32 = 0
  $computedReturnBool = 0
  $computedReturnVoid = 0
  $computedParamI32 = 0
  $computedParamBool = 0
  foreach ($fn in $functions) {
    if ($null -eq $fn.param_types) {
      throw "contract FAIL: missing function.param_types in manifest for $CaseName"
    }
    $paramTypes = @($fn.param_types)
    if ($paramTypes.Count -ne [int]$fn.params) {
      throw "contract FAIL: function.param_types length mismatch for $CaseName function=$($fn.name)"
    }
    foreach ($paramType in $paramTypes) {
      if ($paramType -eq "i32") {
        $computedParamI32++
      } elseif ($paramType -eq "bool") {
        $computedParamBool++
      } else {
        throw "contract FAIL: unsupported function.param_types entry '$paramType' for $CaseName function=$($fn.name)"
      }
    }

    if ($fn.return -eq "i32") {
      $computedReturnI32++
    } elseif ($fn.return -eq "bool") {
      $computedReturnBool++
    } elseif ($fn.return -eq "void") {
      $computedReturnVoid++
    } else {
      throw "contract FAIL: unsupported function.return '$($fn.return)' for $CaseName function=$($fn.name)"
    }
  }

  if ([int]$signatureSurface.scalar_return_i32 -ne $computedReturnI32) {
    throw "contract FAIL: function_signature_surface.scalar_return_i32 mismatch for $CaseName"
  }
  if ([int]$signatureSurface.scalar_return_bool -ne $computedReturnBool) {
    throw "contract FAIL: function_signature_surface.scalar_return_bool mismatch for $CaseName"
  }
  if ([int]$signatureSurface.scalar_return_void -ne $computedReturnVoid) {
    throw "contract FAIL: function_signature_surface.scalar_return_void mismatch for $CaseName"
  }
  if ([int]$signatureSurface.scalar_param_i32 -ne $computedParamI32) {
    throw "contract FAIL: function_signature_surface.scalar_param_i32 mismatch for $CaseName"
  }
  if ([int]$signatureSurface.scalar_param_bool -ne $computedParamBool) {
    throw "contract FAIL: function_signature_surface.scalar_param_bool mismatch for $CaseName"
  }
}

function Invoke-ContractCase {
  param(
    [string]$Source,
    [string]$CaseName,
    [switch]$RequireLl,
    [string[]]$ExtraArgs = @(),
    [string[]]$RequiredLlTokens = @("define i32 @objc3c_entry"),
    [string[]]$ForbiddenLlTokens = @(),
    [string[]]$RequiredManifestTokens = @(),
    [switch]$RequireObjc3ManifestSurface
  )

  $run1 = Join-Path $outDir ($CaseName + "_run1")
  $run2 = Join-Path $outDir ($CaseName + "_run2")
  New-Item -ItemType Directory -Force -Path $run1 | Out-Null
  New-Item -ItemType Directory -Force -Path $run2 | Out-Null

  $compileArgsRun1 = @($Source, "--out-dir", $run1, "--emit-prefix", "module") + $ExtraArgs
  $compileArgsRun2 = @($Source, "--out-dir", $run2, "--emit-prefix", "module") + $ExtraArgs

  & $exe @compileArgsRun1
  if ($LASTEXITCODE -ne 0) { throw "contract FAIL: compile failed for $CaseName run1" }
  & $exe @compileArgsRun2
  if ($LASTEXITCODE -ne 0) { throw "contract FAIL: compile failed for $CaseName run2" }

  $manifest1 = Get-Content (Join-Path $run1 "module.manifest.json") -Raw
  $manifest2 = Get-Content (Join-Path $run2 "module.manifest.json") -Raw
  $diag1 = Get-Content (Join-Path $run1 "module.diagnostics.txt") -Raw
  $diag2 = Get-Content (Join-Path $run2 "module.diagnostics.txt") -Raw

  if ($manifest1 -ne $manifest2) { throw "contract FAIL: manifest drift across replay for $CaseName" }
  if ($diag1 -ne $diag2) { throw "contract FAIL: diagnostics drift across replay for $CaseName" }
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
  if (!(Test-Path $objPath)) { throw "contract FAIL: missing object artifact for $CaseName" }
  $objSize = (Get-Item $objPath).Length
  if ($objSize -le 0) { throw "contract FAIL: empty object artifact for $CaseName" }

  if ($RequireLl) {
    $ll1 = Get-Content (Join-Path $run1 "module.ll") -Raw
    $ll2 = Get-Content (Join-Path $run2 "module.ll") -Raw
    if ($ll1 -ne $ll2) { throw "contract FAIL: LLVM IR drift across replay for $CaseName" }
    foreach ($token in $RequiredLlTokens) {
      if ([string]::IsNullOrWhiteSpace($token)) {
        continue
      }
      if ($ll1.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
        throw "contract FAIL: missing LLVM IR token '$token' in run1 for $CaseName"
      }
      if ($ll2.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
        throw "contract FAIL: missing LLVM IR token '$token' in run2 for $CaseName"
      }
    }
    foreach ($token in $ForbiddenLlTokens) {
      if ([string]::IsNullOrWhiteSpace($token)) {
        continue
      }
      if ($ll1.IndexOf($token, [System.StringComparison]::Ordinal) -ge 0) {
        throw "contract FAIL: forbidden LLVM IR token '$token' present in run1 for $CaseName"
      }
      if ($ll2.IndexOf($token, [System.StringComparison]::Ordinal) -ge 0) {
        throw "contract FAIL: forbidden LLVM IR token '$token' present in run2 for $CaseName"
      }
    }
    Write-Output "$CaseName`_deterministic_ir=true"
  }

  Write-Output "$CaseName`_deterministic_manifest=true"
  Write-Output "$CaseName`_deterministic_diagnostics=true"
  Write-Output "$CaseName`_object_size=$objSize"
}

function Get-RecoveryFixtures {
  param(
    [string]$Directory,
    [string]$FixtureKind,
    [string[]]$Extensions = @(".objc3")
  )

  if (!(Test-Path $Directory -PathType Container)) {
    throw "contract FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(Get-ChildItem -Path $Directory -Recurse -File | Where-Object {
      $_.Extension -in $Extensions
    } | Sort-Object FullName)

  if ($fixtures.Count -eq 0) {
    throw "contract FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return $fixtures
}

function Get-FixtureCaseName {
  param(
    [string]$Prefix,
    [string]$FixturePath
  )

  $leaf = [System.IO.Path]::GetFileNameWithoutExtension($FixturePath)
  $leaf = $leaf -replace "[^A-Za-z0-9_-]", "_"
  if ($leaf.Length -gt 48) {
    $leaf = $leaf.Substring(0, 48)
  }

  $fullPath = [System.IO.Path]::GetFullPath($FixturePath)
  $bytes = [System.Text.Encoding]::UTF8.GetBytes($fullPath.Replace("\", "/"))
  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($bytes)
  } finally {
    $sha256.Dispose()
  }
  $hash = [System.BitConverter]::ToString($hashBytes).Replace("-", "").ToLowerInvariant().Substring(0, 12)

  return "$Prefix`_$leaf`_$hash"
}

function Assert-RecoveryFixtureClass {
  param(
    [object[]]$Fixtures,
    [string]$FixtureKind
  )

  $nonObjc3Fixtures = @($Fixtures | Where-Object { $_.Extension -ine ".objc3" })
  if ($nonObjc3Fixtures.Count -gt 0) {
    $sample = ($nonObjc3Fixtures | Select-Object -First 3 | ForEach-Object { $_.FullName.Replace("\", "/") }) -join ", "
    throw "contract FAIL: $FixtureKind fixture class resolved non-.objc3 entries (sample: $sample)"
  }

  $dispatchFixtures = @($Fixtures | Where-Object { $_.FullName -match "[\\/](lowering_dispatch|message_dispatch|dispatch)[\\/]" })
  if ($dispatchFixtures.Count -gt 0) {
    $sample = ($dispatchFixtures | Select-Object -First 3 | ForEach-Object { $_.FullName.Replace("\", "/") }) -join ", "
    throw "contract FAIL: $FixtureKind fixture class resolved dispatch/lowering fixtures (sample: $sample)"
  }

  Write-Output ("fixture-class: kind={0} expected=recovery-objc3 count={1}" -f $FixtureKind, $Fixtures.Count)
}

Invoke-ContractCase -Source "tests/tooling/fixtures/native/hello.m" -CaseName "objc_baseline"
Invoke-ContractCase -Source "tests/tooling/fixtures/native/hello.objc3" -CaseName "objc3_frontend" -RequireLl -RequireObjc3ManifestSurface
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
  -ExtraArgs @("--objc3-runtime-dispatch-symbol", "objc3_msgsend_lane_c_surface", "--objc3-max-message-args", "6") `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_lane_c_surface(i32, ptr, i32, i32, i32, i32, i32, i32)", "call i32 @objc3_msgsend_lane_c_surface(", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_symbol"":""objc3_msgsend_lane_c_surface""", """runtime_dispatch_arg_slots"":6", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_short_circuit.objc3" `
  -CaseName "objc3_dispatch_nil_receiver_short_circuit" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_unary_short_circuit.objc3" `
  -CaseName "objc3_dispatch_nil_receiver_unary_short_circuit" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_semantic_compatibility.objc3" `
  -CaseName "objc3_dispatch_nil_receiver_semantic_compatibility" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "icmp eq i32", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_mixed_expression_flow.objc3" `
  -CaseName "objc3_dispatch_nil_receiver_mixed_expression_flow" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "icmp eq i32", "cond_true_", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_direct_nil_receiver_elision.objc3" `
  -CaseName "objc3_dispatch_direct_nil_receiver_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_direct_nil_receiver_keyword_elision.objc3" `
  -CaseName "objc3_dispatch_direct_nil_receiver_keyword_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_unary_elision.objc3" `
  -CaseName "objc3_dispatch_nil_bound_identifier_unary_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_keyword_elision.objc3" `
  -CaseName "objc3_dispatch_nil_bound_identifier_keyword_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_alias_identifier_elision.objc3" `
  -CaseName "objc3_dispatch_nil_alias_identifier_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_conditional_receiver_elision.objc3" `
  -CaseName "objc3_dispatch_nil_conditional_receiver_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "cond_true_", "cond_false_", "cond_merge_", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_elision.objc3" `
  -CaseName "objc3_dispatch_nil_immutable_global_identifier_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_mutable_global_identifier_non_elided.objc3" `
  -CaseName "objc3_dispatch_nil_mutable_global_identifier_non_elided" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_global_identifier_post_call_non_elided.objc3" `
  -CaseName "objc3_dispatch_nil_global_identifier_post_call_non_elided" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_post_pure_call_elision.objc3" `
  -CaseName "objc3_dispatch_nil_immutable_global_identifier_post_pure_call_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_post_pure_prototype_call_elision.objc3" `
  -CaseName "objc3_dispatch_nil_immutable_global_identifier_post_pure_prototype_call_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_mixed_flow.objc3" `
  -CaseName "objc3_dispatch_nil_bound_identifier_mixed_flow" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_pre_reassignment_elision.objc3" `
  -CaseName "objc3_dispatch_nil_bound_identifier_pre_reassignment_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_post_assignment_nil_elision.objc3" `
  -CaseName "objc3_dispatch_nil_bound_identifier_post_assignment_nil_elision" `
  -RequireLl `
  -RequiredLlTokens @("define i32 @main()", "define i32 @objc3c_entry", "ret i32") `
  -ForbiddenLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_dispatch_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_nonzero_receiver_fast_path.objc3" `
  -CaseName "objc3_dispatch_numeric_nonzero_receiver_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_bound_identifier_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_constant_expression_receiver_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_constant_expression_receiver_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_global_identifier_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_const_expr_identifier_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_global_const_expr_identifier_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_call_non_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_global_identifier_post_call_non_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", " = phi i32 [0, %msg_nil_", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_pure_call_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_global_identifier_post_pure_call_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_pure_prototype_call_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_global_identifier_post_pure_prototype_call_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_extern_call_non_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_global_identifier_post_extern_call_non_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", " = phi i32 [0, %msg_nil_", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_mutable_global_identifier_non_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_mutable_global_identifier_non_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", " = phi i32 [0, %msg_nil_", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_immutable_global_identifier_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_immutable_global_identifier_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_unary_receiver_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_unary_receiver_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_short_circuit_receiver_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_short_circuit_receiver_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_const_expr_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_bound_identifier_const_expr_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_post_assignment_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_bound_identifier_post_assignment_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -ForbiddenLlTokens @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_invalidation_non_fast_path.objc3" `
  -CaseName "objc3_dispatch_nonzero_bound_identifier_invalidation_non_fast_path" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", " = phi i32 [0, %msg_nil_", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface
Invoke-ContractCase `
  -Source "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_zero_receiver_non_elided.objc3" `
  -CaseName "objc3_dispatch_numeric_zero_receiver_non_elided" `
  -RequireLl `
  -RequiredLlTokens @("declare i32 @objc3_msgsend_i32(", "icmp eq i32", "msg_nil_", "msg_dispatch_", "call i32 @objc3_msgsend_i32(", "define i32 @objc3c_entry") `
  -RequiredManifestTokens @("""runtime_dispatch_arg_slots"":4", """selector_global_ordering"":""lexicographic""") `
  -RequireObjc3ManifestSurface

$invalidDispatchOutDir = Join-Path $outDir "objc3_invalid_dispatch_symbol"
New-Item -ItemType Directory -Force -Path $invalidDispatchOutDir | Out-Null
& $exe "tests/tooling/fixtures/native/hello.objc3" --out-dir $invalidDispatchOutDir --emit-prefix module --objc3-runtime-dispatch-symbol "9invalid_symbol"
$invalidDispatchExit = $LASTEXITCODE
if ($invalidDispatchExit -ne 2) {
  throw "contract FAIL: invalid runtime dispatch symbol should fail with exit 2 (got $invalidDispatchExit)"
}
Write-Output "objc3_invalid_dispatch_symbol_rejected=true"
Write-Output "objc3_invalid_dispatch_symbol_exit_code=$invalidDispatchExit"

$positiveFixtures = Get-RecoveryFixtures -Directory $positiveFixtureDir -FixtureKind "positive native recovery" -Extensions @(".objc3")
Assert-RecoveryFixtureClass -Fixtures $positiveFixtures -FixtureKind "positive native recovery"
foreach ($fixture in $positiveFixtures) {
  $source = $fixture.FullName
  $caseName = Get-FixtureCaseName -Prefix "recovery_positive" -FixturePath $source
  $caseOutDir = Join-Path $outDir $caseName
  New-Item -ItemType Directory -Force -Path $caseOutDir | Out-Null

  & $exe $source --out-dir $caseOutDir --emit-prefix module
  if ($LASTEXITCODE -ne 0) {
    throw "contract FAIL: positive fixture compile failed for $source with exit $LASTEXITCODE"
  }

  $objPath = Join-Path $caseOutDir "module.obj"
  if (!(Test-Path $objPath)) { throw "contract FAIL: missing object artifact for positive fixture $source" }
  $objSize = (Get-Item $objPath).Length
  if ($objSize -le 0) { throw "contract FAIL: empty object artifact for positive fixture $source" }

  $manifestPath = Join-Path $caseOutDir "module.manifest.json"
  if (!(Test-Path $manifestPath)) { throw "contract FAIL: missing manifest artifact for positive fixture $source" }
  $manifest = Get-Content $manifestPath -Raw
  if ([string]::IsNullOrWhiteSpace($manifest)) {
    throw "contract FAIL: empty manifest artifact for positive fixture $source"
  }
  Assert-Objc3ManifestPipelineSurface -ManifestText $manifest -CaseName $source

  Write-Output "$caseName`_compiled=true"
  Write-Output "$caseName`_object_size=$objSize"
}

$negativeFixtures = Get-RecoveryFixtures -Directory $negativeFixtureDir -FixtureKind "negative native recovery" -Extensions @(".objc3")
Assert-RecoveryFixtureClass -Fixtures $negativeFixtures -FixtureKind "negative native recovery"
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
foreach ($fixture in $negativeFixtures) {
  $source = $fixture.FullName
  $caseName = Get-FixtureCaseName -Prefix "recovery_negative" -FixturePath $source
  $run1 = Join-Path $outDir ($caseName + "_run1")
  $run2 = Join-Path $outDir ($caseName + "_run2")
  New-Item -ItemType Directory -Force -Path $run1 | Out-Null
  New-Item -ItemType Directory -Force -Path $run2 | Out-Null

  & $exe $source --out-dir $run1 --emit-prefix module
  $exit1 = $LASTEXITCODE
  if ($exit1 -eq 0) {
    throw "contract FAIL: negative fixture unexpectedly compiled for $source"
  }
  $diagPath1 = Join-Path $run1 "module.diagnostics.txt"
  if (!(Test-Path $diagPath1)) { throw "contract FAIL: missing diagnostics artifact for negative fixture $source run1" }
  $diag1 = Get-Content $diagPath1 -Raw
  if ([string]::IsNullOrWhiteSpace($diag1)) {
    throw "contract FAIL: empty diagnostics artifact for negative fixture $source run1"
  }

  & $exe $source --out-dir $run2 --emit-prefix module
  $exit2 = $LASTEXITCODE
  if ($exit2 -eq 0) {
    throw "contract FAIL: negative fixture unexpectedly compiled for $source on replay"
  }
  $diagPath2 = Join-Path $run2 "module.diagnostics.txt"
  if (!(Test-Path $diagPath2)) { throw "contract FAIL: missing diagnostics artifact for negative fixture $source run2" }
  $diag2 = Get-Content $diagPath2 -Raw
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
