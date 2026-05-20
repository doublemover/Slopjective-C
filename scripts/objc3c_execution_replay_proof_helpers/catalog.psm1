Set-StrictMode -Version Latest

function Get-ExecutionReplayProofRuntimeSections {
  return @(
    "objc3.runtime.class_descriptors",
    "objc3.runtime.protocol_descriptors",
    "objc3.runtime.category_descriptors",
    "objc3.runtime.property_descriptors",
    "objc3.runtime.ivar_descriptors",
    "objc3.runtime.selector_pool",
    "objc3.runtime.string_pool",
    "objc3.runtime.discovery_root",
    "objc3.runtime.linker_anchor",
    "objc3.runtime.image_root",
    "objc3.runtime.registration_descriptor"
  )
}

function Get-ExecutionReplayProofCases {
  $requiredRuntimeSections = @(Get-ExecutionReplayProofRuntimeSections)

  return @(
    [ordered]@{
      case_id = "canonical-runnable"
      fixture = "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3"
      required_ll_tokens = @()
      required_runtime_sections = @($requiredRuntimeSections)
    },
    [ordered]@{
      case_id = "dispatch-fast-path"
      fixture = "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3"
      required_ll_tokens = @("@objc3_runtime_dispatch_i32")
      required_runtime_sections = @()
    },
    [ordered]@{
      case_id = "synthesized-accessor"
      fixture = "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
      required_ll_tokens = @(
        "define i32 @objc3_method_Widget_instance_count()",
        "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
        "define i1 @objc3_method_Widget_instance_enabled()",
        "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)"
      )
      required_runtime_sections = @()
    },
    [ordered]@{
      case_id = "metadata-sections"
      fixture = "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3"
      required_ll_tokens = @()
      required_runtime_sections = @($requiredRuntimeSections)
    },
    [ordered]@{
      case_id = "stdlib-core-runtime-helpers"
      fixture = "tests/tooling/fixtures/native/execution/positive/stdlib_core_runtime_helpers.objc3"
      required_ll_tokens = @(
        "@objc3_runtime_stdlib_core_language_revision_i32",
        "@objc3_runtime_stdlib_core_profile_revision_i32",
        "@objc3_runtime_stdlib_core_has_capability_i32",
        "@objc3_runtime_stdlib_core_option_unwrap_or_i32",
        "@objc3_runtime_stdlib_core_count_i32",
        "@objc3_runtime_stdlib_core_prefix_count_i32",
        "@objc3_runtime_stdlib_core_map_entry_value_or_i32"
      )
      required_runtime_sections = @()
    }
  )
}

function Select-ProofCases {
  param(
    [Parameter(Mandatory = $true)][object[]]$Cases,
    [string]$CaseId = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  if ($Limit -lt 0) {
    throw "execution replay proof FAIL: limit must be non-negative"
  }
  if ($ShardCount -lt 0) {
    throw "execution replay proof FAIL: shard-count must be non-negative"
  }
  if (($ShardIndex -ge 0) -and ($ShardCount -le 0)) {
    throw "execution replay proof FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCount -gt 0) -and (($ShardIndex -lt 0) -or ($ShardIndex -ge $ShardCount))) {
    throw "execution replay proof FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
  }

  $selected = @($Cases)
  if (-not [string]::IsNullOrWhiteSpace($CaseId)) {
    $selected = @($selected | Where-Object { [string]$_.case_id -eq $CaseId })
    if ($selected.Count -eq 0) {
      throw "execution replay proof FAIL: no replay proof case matched case-id '$CaseId'"
    }
  }

  if ($ShardCount -gt 0) {
    $sharded = New-Object System.Collections.Generic.List[object]
    for ($index = 0; $index -lt $selected.Count; $index++) {
      if (($index % $ShardCount) -eq $ShardIndex) {
        $sharded.Add($selected[$index]) | Out-Null
      }
    }
    $selected = @($sharded)
  }

  if (($Limit -gt 0) -and ($selected.Count -gt $Limit)) {
    $selected = @($selected | Select-Object -First $Limit)
  }

  if ($selected.Count -eq 0) {
    throw "execution replay proof FAIL: no replay proof cases matched the requested selection"
  }

  return $selected
}
