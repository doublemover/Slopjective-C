# M253-C005 Selector And String Pool Emission Core Feature Expansion Packet

Packet: `M253-C005`
Milestone: `M253`
Lane: `C`
Issue: `#7094`

## Dependency Chain

- `M253-C004`

## Goal

Replace the older selector-only global emission scheme with canonical selector and string pool sections that remain deterministic, object-emittable, and reusable by later runtime lookup work.

## Scope

- Emit `!objc3.objc_runtime_selector_string_pool_emission`
- Emit retained selector and string pool aggregate globals
- Route message-send selector pointers through the canonical selector pool
- Preserve the existing class/protocol/category/property/ivar payload contracts from earlier `M253` lane-C issues
- Publish evidence under `tmp/`

## Non-Goals

- Runtime registration/bootstrap
- Descriptor-family rewiring to pooled string pointers
- Selector interning table mutation at runtime

## Core Anchors

- Contract id: `objc3c-runtime-selector-string-pool-emission/m253-c005-v1`
- Selector payload model: `canonical-selector-cstring-pool-with-stable-ordinal-aggregate`
- String payload model: `canonical-runtime-string-cstring-pool-with-stable-ordinal-aggregate`
- Pool aggregate symbols:
  - `@__objc3_sec_selector_pool`
  - `@__objc3_sec_string_pool`
- Pool entry symbols:
  - `@__objc3_sel_pool_0000`
  - `@__objc3_str_pool_0000`

## Validation Fixtures

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- `tests/tooling/fixtures/native/execution/positive/message_send_runtime_shim.objc3`

## Validation Artifacts

- `tmp/reports/m253/M253-C005/selector_string_pool_emission_summary.json`
