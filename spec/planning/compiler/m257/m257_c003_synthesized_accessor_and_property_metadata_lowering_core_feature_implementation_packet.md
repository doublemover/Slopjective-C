# M257-C003 Synthesized Accessor And Property Metadata Lowering Core Feature Implementation Packet

Packet: `M257-C003`
Issue: `#7152`
Milestone: `M257`
Lane: `C`
Wave: `W49`

## Objective

Lower synthesized property accessors into real emitted method bodies and bind those accessors back into emitted property metadata so the current realized-object path exposes runnable property behavior instead of summaries only.

## Dependencies

- `M257-A001`
- `M257-A002`
- `M257-B001`
- `M257-B002`
- `M257-B003`
- `M257-C001`
- `M257-C002`

## Required implementation points

1. Missing implementation-owned effective getters/setters become executable method-list entries.
2. Lowering emits deterministic storage globals keyed by synthesized binding symbols.
3. Property descriptor payloads carry:
   - effective getter selector
   - effective setter selector
   - synthesized binding symbol
   - ivar layout symbol
   - accessor implementation pointers
   - slot / size / alignment facts
4. The happy path is proven with:
   - `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
   - `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
5. Validation evidence lands at `tmp/reports/m257/M257-C003/synthesized_accessor_property_lowering_summary.json`.
