# M255 Selector Interning and Lookup Tables Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-selector-lookup-tables/m255-d002-v1`

`M255-D002` turns the deferred selector-table boundary from `M255-D001` into a
real runtime capability backed by emitted startup selector pools and retained
bootstrap replay state.

## Required anchors

- `objc3_runtime_copy_selector_lookup_table_state_for_testing`
- `objc3_runtime_copy_selector_lookup_entry_for_testing`
- `registered-selector-pools-materialize-process-global-stable-id-table`
- `per-image-selector-pools-deduplicated-and-merged-across-registration-order`
- `unknown-selector-lookups-remain-dynamic-until-m255-d003`
- `reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order`

## Behavioral requirements

1. Selector pools discovered through the staged registration table must
   materialize canonical stable IDs before live lookup or replay depends on
   them.
2. Duplicate selector spellings inside one selector pool must fail closed.
3. Duplicate selector spellings across distinct registered images must merge
   onto one stable ID while retaining provider-count and registration-order
   provenance.
4. Reset must clear the live selector table.
5. Replay must rebuild the metadata-backed selector table in original
   registration order.
6. Unknown selector lookups remain available as dynamic fallback until
   `M255-D003` lands method-cache and slow-path lookup.

## Proof surface

- The checker compiles a real emitted fixture object from
  `tests/tooling/fixtures/native/m254_c002_runtime_bootstrap_metadata_library.objc3`.
- The probe links that object with `artifacts/lib/objc3_runtime.lib` and proves:
  - emitted startup registration materializes selector-table entries,
  - a second staged registration merges duplicate selectors onto the same stable
    ID,
  - dynamic fallback remains separate from metadata-backed selectors,
  - reset/replay rebuilds the metadata-backed table deterministically.
