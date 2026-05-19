Set-StrictMode -Version Latest

function Assert-Objc3ManifestPipelineSurface {
  param(
    [string]$ManifestText,
    [string]$CaseName
  )

  try {
    if ($PSVersionTable.PSVersion.Major -ge 6) {
      $manifest = $ManifestText | ConvertFrom-Json -Depth 64
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
  $hasExtendedSignatureTypes = $false
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
      } elseif ([string]::IsNullOrWhiteSpace([string]$paramType)) {
        throw "contract FAIL: empty function.param_types entry for $CaseName function=$($fn.name)"
      } else {
        $hasExtendedSignatureTypes = $true
      }
    }

    if ($fn.return -eq "i32") {
      $computedReturnI32++
    } elseif ($fn.return -eq "bool") {
      $computedReturnBool++
    } elseif ($fn.return -eq "void") {
      $computedReturnVoid++
    } elseif ([string]::IsNullOrWhiteSpace([string]$fn.return)) {
      throw "contract FAIL: empty function.return for $CaseName function=$($fn.name)"
    } else {
      $hasExtendedSignatureTypes = $true
    }
  }

  if (-not $hasExtendedSignatureTypes) {
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
}

Export-ModuleMember -Function @(
  "Assert-Objc3ManifestPipelineSurface"
)
