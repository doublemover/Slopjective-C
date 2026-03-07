# M251-C002 LLVM Global and Section Scaffold for Runtime Metadata Payloads Packet

Packet: `M251-C002`
Milestone: `M251`
Lane: `C`
Issue: `#7063`

## Objective

Add the first real emitted runtime metadata section scaffold so LLVM IR and emitted objects contain deterministic placeholder globals in the canonical metadata sections before the full runtime object-model payload layouts land.

## Dependencies

- `M251-A002`
- `M251-B002`
- `M251-C001`

## Required implementation

1. Build a canonical `Objc3RuntimeMetadataSectionScaffoldSummary` from the frozen `M251-C001` section ABI plus the runtime export counts validated by `M251-B002`.
2. Project the scaffold summary into manifest JSON and `Objc3IRFrontendMetadata`.
3. Emit an image-info placeholder global at `@__objc3_image_info` in `objc3.runtime.image_info`.
4. Emit retained aggregate globals at:
   - `@__objc3_sec_class_descriptors`
   - `@__objc3_sec_protocol_descriptors`
   - `@__objc3_sec_category_descriptors`
   - `@__objc3_sec_property_descriptors`
   - `@__objc3_sec_ivar_descriptors`
5. Emit per-record descriptor placeholder globals using the canonical descriptor prefix `@__objc3_meta_...` inside the matching logical section.
6. Retain the emitted globals through `@llvm.used` so `llc` cannot discard them before later object inspection and runtime registration work lands.
7. Publish the scaffold contract in emitted LLVM IR as `!objc3.objc_runtime_metadata_section_scaffold`.

## Determinism and fail-closed rules

- `M251-C002` must remain gated on the `M251-C001` ABI freeze packet and the `M251-B002` runtime export enforcement packet.
- If the scaffold prerequisites are not ready, the summary must remain fail-closed and the emission path must not synthesize partial runtime metadata globals.
- Aggregate symbol names, descriptor prefixes, logical section names, and the `llvm.used` retention root are frozen inputs rather than ad hoc lowering decisions.

## Validation plan

The checker runs two deterministic probes:

1. Manifest-only runner probe on `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`.
2. Native compile probe on `tests/tooling/fixtures/native/hello.objc3` proving LLVM IR/object emission succeeds with the scaffold present.

## Evidence

- `tmp/reports/m251/M251-C002/runtime_metadata_section_scaffold_summary.json`
