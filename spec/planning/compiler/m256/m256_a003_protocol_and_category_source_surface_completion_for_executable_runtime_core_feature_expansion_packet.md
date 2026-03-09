# M256-A003 Protocol And Category Source Surface Completion For Executable Runtime Core Feature Expansion Packet

Packet: `M256-A003`
Milestone: `M256`
Lane: `A`
Issue: `#7131`
Contract ID: `objc3c-executable-protocol-category-source-closure/m256-a003-v1`
Depends on: `M256-A002`
Next issue: `M256-B001`

## Summary

Complete the executable source surface for runtime-attached protocols and
categories, including the edges needed by attachment and conformance checks,
without breaking the frozen `M256-A001`/`M256-A002` boundary.

## Required Models

- `protocol-declaration-owned-inherited-protocol-identities`
- `category-declaration-owned-class-interface-implementation-attachment-identities`
- `category-declaration-owned-adopted-protocol-conformance-identities`

## Source-Graph Requirements

- Protocol graph nodes publish:
  - `declaration_complete`
  - `inherited_protocol_identity_complete`
- Category graph nodes publish:
  - `declaration_complete`
  - `attachment_identity_complete`
  - `conformance_identity_complete`
- Source graph publishes:
  - `protocol_category_source_closure_contract_id`
  - `protocol_inheritance_identity_model`
  - `category_attachment_identity_model`
  - `protocol_category_conformance_identity_model`
  - `protocol_category_declaration_closure_complete`
  - `protocol_inheritance_identity_closure_complete`
  - `category_attachment_identity_closure_complete`
  - `protocol_category_conformance_identity_closure_complete`

## IR/Manifest Proof Surface

- `module.manifest.json` publishes the new source-graph fields and node flags.
- `module.ll` publishes:
  - `; executable_protocol_category_source_closure = ...`
- The summary line carries protocol/category node counts and the edge counts for:
  - protocol inheritance
  - category attachment
  - adopted-protocol conformance

## Evidence Fixture

Primary fixture:
- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

Expected proof edges include:
- `protocol-to-inherited-protocol`
- `category-to-class`
- `category-to-interface`
- `category-to-implementation`
- `category-to-protocol`

## Validation

- `python scripts/check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py -q`
- `npm run check:objc3c:m256-a003-lane-a-readiness`
