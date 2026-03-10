# M257 Property Metadata Registration And Reflective Access Helpers Core Feature Expansion Expectations (D003)

Contract ID: `objc3c-runtime-property-metadata-reflection/m257-d003-v1`

Scope: `M257-D003` adds a private runtime reflection helper surface over the realized property/accessor/layout graph built by `M257-D002`.

Required outcomes:

1. Runtime exposes aggregate property reflection state for testing and diagnostics.
2. Runtime exposes per-property reflective entry snapshots by class/property name.
3. Reflective entries publish effective accessors, owner identities, synthesized binding symbols, and layout facts without source rediscovery.
4. Misses stay fail-closed and publish deterministic last-query evidence.
5. Code/spec/package anchors remain explicit and deterministic.
6. Validation evidence lands at `tmp/reports/m257/M257-D003/property_metadata_reflection_summary.json`.

Canonical proof assets:

- `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
- `tests/tooling/runtime/m257_d003_property_metadata_reflection_probe.cpp`
- `scripts/check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py`
- `scripts/run_m257_d003_lane_d_readiness.py`
- `tests/tooling/test_check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py`

Model strings:

- `runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery`
- `private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts`
- `no-public-reflection-abi-no-reflective-source-recovery-no-property-query-success-without-realized-runtime-layout`
