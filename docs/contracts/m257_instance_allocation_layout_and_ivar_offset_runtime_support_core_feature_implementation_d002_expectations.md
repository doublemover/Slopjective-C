# M257 Instance Allocation Layout And Ivar Offset Runtime Support Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-instance-allocation-layout-support/m257-d002-v1`

Scope: `M257-D002` upgrades the historical `M257-D001` runtime freeze into true per-instance allocation and slot-backed synthesized property execution.

Required outcomes:

1. Runtime allocation uses realized class layout instead of returning one canonical instance identity per class.
2. Repeated `alloc` / `new` for the same class materialize distinct runtime instance identities.
3. Synthesized property accessors read and write per-instance slot storage using emitted ivar offsets and layout records.
4. Runtime method-cache resolution stays stable: synthesized getter/setter owners and normalized receiver identities remain deterministic.
5. The runtime still consumes emitted property and ivar metadata without source rediscovery.
6. Code/spec/package anchors remain explicit and deterministic.
7. Validation evidence lands at `tmp/reports/m257/M257-D002/instance_allocation_layout_runtime_summary.json`.

Canonical proof assets:

- `tests/tooling/fixtures/native/m257_d002_instance_allocation_runtime_positive.objc3`
- `tests/tooling/runtime/m257_d002_instance_allocation_runtime_probe.cpp`
- `scripts/check_m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation.py`
- `scripts/run_m257_d002_lane_d_readiness.py`
- `tests/tooling/test_check_m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation.py`

Model strings:

- `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
- `alloc-new-materialize-distinct-runtime-instance-identities-backed-by-realized-class-layout`
- `synthesized-accessor-execution-reads-and-writes-per-instance-slot-storage-using-emitted-ivar-offset-layout-records`
- `no-layout-rederivation-no-shared-global-property-storage-no-reflective-property-registration-yet`
