# M314-B004 Expectations

Contract ID: `objc3c-cleanup-alias-deprecation-compatibility-window/m314-b004-v1`

## Purpose

Close the compatibility window entirely by removing the legacy alias mass from `package.json` and freezing only machine-readable retirement metadata.

## Required truths

- Legacy package aliases no longer remain supported through `package.json`.
- The retired alias families are machine-readable through repo metadata instead of package-script wrappers.
- No new legacy alias families may be added during the compatibility window.
- Public docs must not present removed alias families as workflows.

## Removed alias families

- milestone-local validation aliases
- historical release-replay aliases
- historical planning aliases
- historical developer probe aliases

## Ownership

- `M314-C003` owns public-doc synchronization away from removed aliases.
- `M314-B005` owns the prototype-path retirement side of the same cleanup tranche.
- `M314-E001` and `M314-E002` own final gate and closeout enforcement.

## Evidence

- `spec/planning/compiler/m314/m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_registry.json`
- `tmp/reports/m314/M314-B004/alias_deprecation_compatibility_window_summary.json`

## Next issue

- `M314-B005`
