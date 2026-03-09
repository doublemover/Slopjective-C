# M255 Protocol and Category-Aware Method Resolution Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-runtime-protocol-category-method-resolution/m255-d004-v1`

## Purpose

Extend the live runtime lookup path so class/metaclass lookup can continue into
category implementation records, and protocol declaration graphs can provide
fail-closed negative lookup evidence when no callable body exists.

## Preserved Dependencies

- `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
- `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`

## Canonical Models

- Category resolution model:
  `class-bodies-win-first-category-implementation-records-supply-next-live-method-tier`
- Protocol declaration model:
  `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution`
- Fallback model:
  `conflicting-category-or-protocol-resolution-fails-closed-to-compatibility-dispatch`

## Required Runtime Snapshot Surface

- `objc3_runtime_copy_method_cache_state_for_testing`
- `objc3_runtime_copy_method_cache_entry_for_testing`
- snapshot fields:
  - `last_category_probe_count`
  - `last_protocol_probe_count`
  - cached `category_probe_count`
  - cached `protocol_probe_count`

## Required Fixture and Probe

- Fixture:
  `tests/tooling/fixtures/native/m255_d004_protocol_category_dispatch.objc3`
- Probe:
  `tests/tooling/runtime/m255_d004_protocol_category_probe.cpp`

## Required Dynamic Outcomes

- instance dispatch of `tracedValue` resolves the live category implementation
  body and returns `33`
- class dispatch of `tracerClassValue` resolves the live category class-method
  body and returns `44`
- protocol-declared selector `protocolDeclaredOnly` remains unresolved and falls
  back to the preserved compatibility result
- instance/category live resolution records one category probe and zero protocol
  probes
- protocol-declared negative resolution records one category probe and two
  protocol probes
- repeat dispatches reuse cached positive and negative results without changing
  semantic outcome

## Required IR/Object Evidence

- emitted IR comment prefix:
  `runtime_protocol_category_method_resolution = contract=objc3c-runtime-protocol-category-method-resolution/m255-d004-v1`
- category descriptor method-list refs carry emitted list-local counts rather
  than aggregate graph totals
- protocol descriptor method-list refs carry emitted list-local counts rather
  than aggregate graph totals
- object output preserves populated protocol/category descriptor sections
