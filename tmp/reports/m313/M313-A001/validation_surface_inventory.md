# M313-A001 Validation Surface Inventory

- generated_at: `2026-03-31T00:58:24.651670+00:00`
- package_scripts_total: `144`
- package_test_scripts: `75`
- package_check_scripts: `27`
- check_py_files: `82`
- test_check_py_files: `0`
- validation_ps1_files: `17`
- shared_acceptance_harness_suite_count: `3`
- retained_static_guard_count: `19`
- executable_validation_count: `63`

## Retained static guard classes
- `retain:docs-surface`: `1`
- `retain:product-surface`: `2`
- `retain:repo-shape`: `1`
- `retain:schema-contract`: `6`
- `retain:source-surface-contract`: `8`
- `retain:task-hygiene`: `1`

## Surface-kind counts
- `executable_acceptance_validator`: `10`
- `executable_integration_validator`: `20`
- `executable_runnable_validator`: `24`
- `executable_validator`: `9`
- `retained_static_guard`: `19`

## Acceptance-first truth owners
- scripts/shared_compiler_runtime_acceptance_harness.py
- scripts/objc3c_public_workflow_runner.py validate-* and test-* actions
- scripts/check_objc3c_*_integration.py and scripts/check_objc3c_runnable_*_end_to_end.py executable flows
- PowerShell runtime suites such as scripts/check_objc3c_native_execution_smoke.ps1, scripts/check_objc3c_execution_replay_proof.ps1, and scripts/check_objc3c_native_recovery_contract.ps1

## Retained static guards
- `scripts/check_distribution_credibility_schema_surface.py` -> `retain:schema-contract`: guards distribution-credibility schemas before dashboard/trust-report publication
- `scripts/check_distribution_credibility_source_surface.py` -> `retain:source-surface-contract`: guards distribution-credibility source roots and checked-in trust-signal contracts
- `scripts/check_documentation_surface.py` -> `retain:docs-surface`: guards reader-facing docs, site structure, command appendix accessibility, and documented surface boundaries
- `scripts/check_external_validation_source_surface.py` -> `retain:source-surface-contract`: guards external-validation intake and trust-policy source-of-truth structure
- `scripts/check_packaging_channels_schema_surface.py` -> `retain:schema-contract`: guards packaging-channel schemas before package-manifest and install-receipt publication
- `scripts/check_packaging_channels_source_surface.py` -> `retain:source-surface-contract`: guards packaging-channel source roots, supported-platform inputs, and workflow-surface contracts
- `scripts/check_performance_governance_schema_surface.py` -> `retain:schema-contract`: guards performance governance schemas before dashboard/report publication
- `scripts/check_performance_governance_source_surface.py` -> `retain:source-surface-contract`: guards performance governance source inputs, policy roots, and checked-in contract layout
- `scripts/check_public_conformance_reporting_source_surface.py` -> `retain:source-surface-contract`: guards public conformance reporting source-surface inputs and checked-in contracts
- `scripts/check_public_conformance_schema_surface.py` -> `retain:schema-contract`: guards public conformance publication schemas before report generation and publication
- `scripts/check_release_foundation_schema_surface.py` -> `retain:schema-contract`: guards release-foundation schemas before manifest/SBOM/attestation publication
- `scripts/check_release_foundation_source_surface.py` -> `retain:source-surface-contract`: guards release-foundation source inputs, policy roots, and payload contract layout
- `scripts/check_release_operations_schema_surface.py` -> `retain:schema-contract`: guards release-operations schemas before update-manifest and publication metadata generation
- `scripts/check_release_operations_source_surface.py` -> `retain:source-surface-contract`: guards release-operations source inputs, version/update policy roots, and workflow metadata contracts
- `scripts/check_repo_superclean_surface.py` -> `retain:repo-shape`: guards canonical repo roots, generated output boundaries, and build-owned source-of-truth publication
- `scripts/check_showcase_surface.py` -> `retain:product-surface`: guards showcase source-of-truth structure and compile-coupled example boundaries
- `scripts/check_stdlib_surface.py` -> `retain:product-surface`: guards stdlib roots, module inventory, package alias mapping, and import/lowering contract shape
- `scripts/check_stress_source_surface.py` -> `retain:source-surface-contract`: guards stress source roots and machine-owned artifact boundaries before execution begins
- `scripts/ci/check_task_hygiene.py` -> `retain:task-hygiene`: prevents removed script families, stale prototype roots, live bytecode, and budget drift from silently returning

## Unreferenced check surfaces
- `scripts/check_activation_triggers.py`
- `scripts/check_bootstrap_readiness.py`
- `scripts/check_conformance_corpus_surface.py`
- `scripts/check_getting_started_surface.py`
- `scripts/check_objc3c_end_to_end_determinism.py`
- `scripts/check_objc3c_library_cli_parity.py`
- `scripts/check_open_blocker_audit_contract.py`

## Non-goals
- This inventory does not collapse or rename validation commands yet; that belongs to later M313 issues.
- This inventory does not delete executable validators; it classifies retained static guards versus acceptance-first truth surfaces.
- This inventory does not rewrite CI scheduling; that belongs to M313-D001 and M313-D002.

Next issue: `M313-B001`