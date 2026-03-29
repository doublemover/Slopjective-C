# objc3c Core Stdlib Surface

This runbook defines the live `M306` boundary for foundational utility, text,
data, collection, option, and result work in the checked-in standard library.

It extends `docs/runbooks/objc3c_stdlib_foundation.md` with the exact module
ownership and API-family split that downstream issues must preserve.

## Working boundary

Live `M306` work must stay on these paths:

- `stdlib/core_architecture.json`
- `stdlib/modules/objc3.core/`
- `stdlib/modules/objc3.errors/`
- `stdlib/modules/objc3.keypath/`
- `docs/runbooks/objc3c_stdlib_core.md`
- `tmp/artifacts/stdlib/`
- `tmp/reports/stdlib/`

Exact live implementation paths for downstream work:

- `stdlib/modules/objc3.core/module.objc3`
- `stdlib/modules/objc3.errors/module.objc3`
- `stdlib/modules/objc3.keypath/module.objc3`
- `stdlib/semantic_policy.json`
- `stdlib/modules/objc3.core/module.json`
- `stdlib/modules/objc3.errors/module.json`
- `stdlib/modules/objc3.keypath/module.json`
- `scripts/check_stdlib_surface.py`
- `scripts/materialize_objc3c_stdlib_workspace.py`
- `scripts/run_objc3c_stdlib_workspace_smoke.py`
- `scripts/objc3c_public_workflow_runner.py`

## Core family split

`objc3.core` owns:

- language/profile revision constants and capability query hooks
- option presence and tag helpers
- text/data view contracts for strings and bytes
- collection-shape helpers for arrays and maps
- trivial count/index predicates that can be reused without importing
  error-bridging semantics

`objc3.errors` owns:

- error identity tags and error-domain/category helpers
- result tags and result inspection helpers
- optional-to-result and optional-to-throw bridge helpers
- compatibility entrypoints that preserve payload identity across
  module/import boundaries

`objc3.keypath` remains in scope only where typed key-path helpers need
text/data compatibility adapters or metadata naming stability.

`objc3.concurrency` and `objc3.system` are not widened by `M306` beyond
dependencies already claimed by the standard-library contract.

## Expected shipped API families

The checked-in architecture contract requires these families to stay visible:

- `objc3.core`
  - `profile-revision`
  - `capability-query`
  - `option-shape`
  - `string-view`
  - `byte-span`
  - `array-slice`
  - `map-entry`
- `objc3.errors`
  - `error-identity`
  - `result-shape`
  - `optional-bridge`
  - `text-data-compatibility`
- `objc3.keypath`
  - `typed-keypath-application`
  - `typed-keypath-text-compatibility`

Downstream implementation issues may add concrete helpers inside these families,
but they should not invent a second family split or move ownership between
modules without updating the checked-in architecture contract.

## Exact checked-in source surface

`objc3.core` exports:

- `Objc3CoreLanguageRevision`
- `Objc3CoreProfileRevision`
- `objc3_core_language_revision`
- `objc3_core_profile_revision`
- `objc3_core_has_capability`
- `objc3_core_option_has_value`
- `objc3_core_string_view_length`
- `objc3_core_bytes_span_length`
- `objc3_core_array_count`
- `objc3_core_map_entry_present`

`objc3.errors` exports:

- `Objc3ErrorsResultOkTag`
- `Objc3ErrorsResultErrTag`
- `objc3_errors_result_ok_tag`
- `objc3_errors_result_err_tag`
- `objc3_errors_result_is_ok`
- `objc3_errors_ok_or_code`
- `objc3_errors_or_throw_code`
- `objc3_errors_text_data_compatibility_score`

`objc3.keypath` exports:

- `objc3_keypath_apply_index`
- `objc3_keypath_component_count`
- `objc3_keypath_text_compatibility_score`

## Semantic guarantees

- all `M306` helpers remain deterministic and side-effect free
- option and presence helpers use `0` for absent and nonzero for present
- `objc3_errors_result_ok_tag` stays `1` and
  `objc3_errors_result_err_tag` stays `2` within major version `1`
- text/data helpers preserve the caller-provided counts instead of claiming
  allocation, ownership, or transcoding semantics
- module semver metadata stays `1.0.0` for the initial core stdlib surface
- additive helper growth is allowed, but moving helper families between modules
  is a breaking change

## Explicit non-goals

This milestone does not justify:

- a second stdlib tree outside `stdlib/`
- generic container ABI claims that are not backed by the live frontend/runtime
- milestone-only proof helpers or shadow package manifests
- moving error/result helpers into `objc3.core`
- widening `objc3.system` just to host text/data helpers
