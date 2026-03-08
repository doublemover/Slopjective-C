# M253 Selector And String Pool Emission Core Feature Expansion Expectations (C005)

Contract ID: `objc3c-runtime-selector-string-pool-emission/m253-c005-v1`

## Objective

Expand lane-C runtime-adjacent string emission from the old selector-only global scheme into a canonical selector pool plus canonical runtime string pool, while preserving the frozen class/protocol/category/property/ivar descriptor payload contracts from `M253-C002`, `M253-C003`, and `M253-C004`.

## Required Anchors

- Emitted LLVM metadata: `!objc3.objc_runtime_selector_string_pool_emission`
- Boundary line prefix: `; runtime_metadata_selector_string_pool_emission = contract=objc3c-runtime-selector-string-pool-emission/m253-c005-v1`
- Selector pool payload model: `canonical-selector-cstring-pool-with-stable-ordinal-aggregate`
- String pool payload model: `canonical-runtime-string-cstring-pool-with-stable-ordinal-aggregate`
- Canonical aggregate symbols:
  - `@__objc3_sec_selector_pool`
  - `@__objc3_sec_string_pool`
- Canonical pooled payload symbols:
  - `@__objc3_sel_pool_0000`
  - `@__objc3_str_pool_0000`
- Legacy selector-only global naming must not remain on the happy path:
  - `@.objc3.sel.`

## Behavior Requirements

- The native IR path shall emit a retained selector pool section for canonical selector cstrings.
- The native IR path shall emit a retained runtime string pool section for canonical non-selector runtime strings.
- Message-send lowering shall source selector pointers from the canonical selector pool instead of the older selector-only global naming scheme.
- The pool sections shall be retained through `@llvm.used` using stable aggregate symbols.
- Existing runtime metadata descriptor bundles may keep their current inline cstring payloads in this issue; `C005` does not rewrite descriptor families to consume pooled pointers.
- The emitted pools shall remain deterministic and lexicographic.
- The `llvm-direct` object path shall preserve both pool sections verbatim.

## Probe Fixtures

- Metadata-rich pool proof fixture:
  `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- Message-send selector pool proof fixture:
  `tests/tooling/fixtures/native/execution/positive/message_send_runtime_shim.objc3`

## Expected Dynamic Evidence

- Metadata fixture boundary summary contains:
  - `selector_pool_count=4`
  - `string_pool_count=21`
- Message-send fixture boundary summary contains:
  - `selector_pool_count=1`
  - `string_pool_count=0`
- Metadata fixture object sections contain:
  - `objc3.runtime.selector_pool`
  - `objc3.runtime.string_pool`
- Message-send fixture object sections contain:
  - `objc3.runtime.selector_pool`
  - `objc3.runtime.string_pool`
- Message-send fixture IR contains pooled selector consumption through `@__objc3_sel_pool_0000`.

## Evidence Path

- `tmp/reports/m253/M253-C005/selector_string_pool_emission_summary.json`
