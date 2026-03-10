# M257-A002 Ivar Layout And Property Attribute Source-Model Completion Core Feature Implementation Packet

Packet: `M257-A002`

Issue: `#7146`

## Objective

Turn the frozen `M257-A001` property/ivar source closure into one deterministic
source-model completion step for property attributes, accessor ownership,
synthesized bindings, and ivar layout.

## Dependencies

- `M257-A001`

## Contract

- contract id
  `objc3c-executable-property-ivar-source-model-completion/m257-a002-v1`
- layout model
  `property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization`
- attribute model
  `property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles`
- evidence model
  `property-layout-fixture-manifest-and-ir-replay-key`
- failure model
  `fail-closed-on-property-attribute-accessor-ownership-or-layout-drift-before-storage-realization`

## Required anchors

- `docs/contracts/m257_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation_a002_expectations.md`
- `scripts/check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py`
- `tests/tooling/test_check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py`
- `scripts/run_m257_a002_lane_a_readiness.py`
- `check:objc3c:m257-a002-ivar-layout-and-property-attribute-source-model-completion`
- `check:objc3c:m257-a002-lane-a-readiness`

## Canonical completed source surface

- `Objc3PropertyDecl.property_attribute_profile`
- `Objc3PropertyDecl.effective_getter_selector`
- `Objc3PropertyDecl.effective_setter_available`
- `Objc3PropertyDecl.effective_setter_selector`
- `Objc3PropertyDecl.accessor_ownership_profile`
- `Objc3PropertyDecl.executable_synthesized_binding_kind`
- `Objc3PropertyDecl.executable_synthesized_binding_symbol`
- `Objc3PropertyDecl.executable_ivar_layout_symbol`
- `Objc3PropertyDecl.executable_ivar_layout_slot_index`
- `Objc3PropertyDecl.executable_ivar_layout_size_bytes`
- `Objc3PropertyDecl.executable_ivar_layout_alignment_bytes`
- `frontend.lowering.executable_property_ivar_source_model_replay_key`

## Live proof fixture

- `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`

## Handoff

`M257-B001` is the explicit next handoff after this implementation closes.
