# M271-A001 Packet: Resource, Borrowed-Pointer, And Capture-List Source Closure - Contract And Architecture Freeze

## Goal

Freeze the truthful Part 8 frontend/source-model contract for:
- local `objc_resource(...)` annotations,
- contextual `borrowed` pointer qualifiers,
- callable `objc_returns_borrowed(owner_index=N)` attributes,
- explicit block capture lists.

## Scope

This issue is parser/frontend only.
It does not claim:
- resource cleanup lowering,
- borrowed escape legality,
- runtime capture ownership behavior.

## Required packet

The emitted frontend manifest must publish:
- `frontend.pipeline.semantic_surface.objc_part8_resource_borrowed_and_capture_list_source_closure`

## Positive fixture proof

The positive fixture must prove deterministic publication for:
- `resource_attribute_sites = 1`
- `resource_close_clause_sites = 1`
- `resource_invalid_clause_sites = 1`
- `borrowed_pointer_sites = 2`
- `returns_borrowed_attribute_sites = 1`
- `explicit_capture_list_sites = 1`
- `explicit_capture_item_sites = 2`
- `explicit_capture_weak_sites = 1`
- `explicit_capture_move_sites = 1`
- `explicit_capture_unowned_sites = 0`
- `explicit_capture_plain_sites = 0`
