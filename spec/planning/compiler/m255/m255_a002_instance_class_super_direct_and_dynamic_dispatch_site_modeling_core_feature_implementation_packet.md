# M255-A002 Instance, Class, Super, Direct, and Dynamic Dispatch-Site Modeling Core Feature Implementation Packet

Packet: `M255-A002`
Milestone: `M255`
Lane: `A`
Issue: `M255-A002`
Dependencies: `M255-A001`
Implementation Contract ID: `objc3c-dispatch-site-modeling/m255-a002-v1`
Live lowering handoff contract: `objc3c-dispatch-surface-classification/m255-a001-v1`

## Scope

Promote the frozen `M255-A001` dispatch taxonomy into one real frontend/sema/
lowering implementation that classifies live message-send receiver surfaces and
keeps the current compatibility dispatch family reachable through native LLVM
emission.

## Required outputs

- `tests/tooling/fixtures/native/m255_dispatch_surface_modeling.objc3`
- `scripts/check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py`
- `tests/tooling/test_check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py`
- `scripts/run_m255_a002_lane_a_readiness.py`
- `tmp/reports/m255/M255-A002/dispatch_site_modeling_summary.json`

## Canonical semantic surface

- `frontend.pipeline.semantic_surface.objc_dispatch_surface_classification_surface`

## Canonical lowering handoff

- `lowering_dispatch_surface_classification`

## Canonical proof counts

- instance `2`
- class `2`
- super `1`
- direct `0`
- dynamic `1`

## Canonical proof commands

- `check:objc3c:m255-a002-lane-a-readiness`

## Required IR anchors

- textual profile line `frontend_objc_dispatch_surface_classification_profile`
- named metadata `!objc3.objc_dispatch_surface_classification`
