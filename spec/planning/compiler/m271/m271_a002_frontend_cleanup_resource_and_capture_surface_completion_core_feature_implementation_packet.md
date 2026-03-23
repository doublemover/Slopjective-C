# M271-A002 Packet: Frontend Cleanup, Resource, And Capture Surface Completion - Core Feature Implementation

## Goal

Complete the remaining Part 8 frontend/source-model surface for:
- plain cleanup hooks on local `let` bindings,
- `@cleanup(...)` sugar,
- `@resource(..., invalid: ...)` sugar,
- full explicit block capture item-mode coverage.

## Scope

This issue is parser/frontend only.
It does not claim:
- cleanup lowering,
- resource runtime behavior,
- borrowed-pointer legality enforcement.

## Required packet

The emitted frontend manifest must publish:
- `frontend.pipeline.semantic_surface.objc_part8_cleanup_resource_and_capture_source_completion`

## Positive fixture proof

The positive fixture must prove deterministic publication for:
- `cleanup_attribute_sites = 2`
- `cleanup_sugar_sites = 1`
- `resource_attribute_sites = 2`
- `resource_sugar_sites = 1`
- `resource_close_clause_sites = 2`
- `resource_invalid_clause_sites = 2`
- `explicit_capture_list_sites = 1`
- `explicit_capture_item_sites = 4`
- `explicit_capture_weak_sites = 1`
- `explicit_capture_unowned_sites = 1`
- `explicit_capture_move_sites = 1`
- `explicit_capture_plain_sites = 1`
