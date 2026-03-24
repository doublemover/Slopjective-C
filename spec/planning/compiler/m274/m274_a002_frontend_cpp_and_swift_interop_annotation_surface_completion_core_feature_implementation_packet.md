# M274-A002 Packet: Frontend Cpp And Swift Interop Annotation Surface Completion - Core Feature Implementation

Packet: `M274-A002`
Milestone: `M274`
Lane: `A`
Dependencies: `M274-A001`
Next issue: `M274-B001`

## Objective

Complete the truthful Part 11 source surface for Swift-facing names, Swift-private markers, C++-facing names, header-name metadata, and interop metadata annotation tracking while preserving the source-only handoff boundary for later semantic expansion.

## Required Outputs

- semantic surface `frontend.pipeline.semantic_surface.objc_part11_cpp_and_swift_interop_annotation_source_completion`
- issue-local checker `scripts/check_m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation.py`
- issue-local readiness runner `scripts/run_m274_a002_lane_a_readiness.py`
- issue-local pytest `tests/tooling/test_check_m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation.py`

## Canonical Payload Expectations

The emitted manifest must preserve a deterministic source-only packet with the following field families:

- annotation site counts:
  - `swift_name_annotation_sites`
  - `swift_private_annotation_sites`
  - `cpp_name_annotation_sites`
  - `header_name_annotation_sites`
  - `interop_metadata_annotation_sites`
  - `named_annotation_payload_sites`
- source-support state:
  - `swift_annotation_source_supported`
  - `cpp_annotation_source_supported`
  - `interop_metadata_source_supported`
  - `deterministic_handoff`
  - `ready_for_semantic_expansion`

## Non-Goals

- Swift or C++ ABI lowering
- runtime bridge emission
- cross-language execution semantics beyond source-surface completion

