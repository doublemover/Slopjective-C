# M262-D001 Runtime ARC Helper API Surface Contract And Architecture Freeze Packet

Issue: `#7203`
Packet: `M262-D001`
Milestone: `M262`
Lane: `D`

## Intent

Freeze the truthful private runtime ARC helper API that lane C already lowers
against after `M262-C004`, without claiming a widened public runtime ABI.

## Required Surface

- public runtime header still omits ARC helper entrypoints
- private bootstrap-internal runtime header remains the canonical home for:
  - `objc3_runtime_retain_i32`
  - `objc3_runtime_release_i32`
  - `objc3_runtime_autorelease_i32`
  - `objc3_runtime_read_current_property_i32`
  - `objc3_runtime_write_current_property_i32`
  - `objc3_runtime_exchange_current_property_i32`
  - `objc3_runtime_load_weak_current_property_i32`
  - `objc3_runtime_store_weak_current_property_i32`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
- emitted IR must carry the canonical summary line and named metadata for this
  frozen runtime/helper boundary

## Required Artifacts

- `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
- `tmp/reports/m262/M262-D001/runtime_arc_helper_api_surface_contract_summary.json`

## Non-goals

- no public ARC runtime header widening
- no dedicated user-facing ARC helper ABI
- no claim that helper behavior is stable outside lowered/compiler-owned paths

## Dependency Continuity

- `M262-C004` is the direct prerequisite lowering boundary
- `M262-D002` is the explicit next issue after this freeze lands
