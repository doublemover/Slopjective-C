# M314-B004 Expectations

Contract ID: `objc3c-cleanup-alias-deprecation-compatibility-window/m314-b004-v1`

## Purpose

Define one explicit compatibility window for the legacy alias mass so command-surface cleanup can proceed without pretending those aliases are still public.

## Required truths

- Legacy package aliases remain temporarily supported but non-public.
- The compatibility window is machine-readable in `package.json` and names concrete alias families.
- No new legacy alias families may be added during the compatibility window.
- Public docs must not present compatibility aliases as preferred workflows.

## Covered compatibility families

- milestone-local validation aliases
- historical release-replay aliases
- historical planning aliases
- historical developer probe aliases

## Ownership

- `M314-C003` owns public-doc synchronization away from compatibility aliases.
- `M314-B005` owns the prototype-path retirement side of compatibility cleanup.
- `M314-E001` and `M314-E002` own final gate and closeout enforcement.

## Evidence

- `spec/planning/compiler/m314/m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_registry.json`
- `tmp/reports/m314/M314-B004/alias_deprecation_compatibility_window_summary.json`

## Next issue

- `M314-B005`
