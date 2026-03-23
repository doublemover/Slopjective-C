# M271 Frontend Cleanup, Resource, And Capture Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part8-cleanup-resource-capture-source-closure/m271-a002-v1`

## Required behavior

- The compiler must admit local `__attribute__((cleanup(CleanupFn))) let name = value;` declarations as a real parser/frontend capability.
- The compiler must admit local `@cleanup(CleanupFn) let name = value;` declarations as a real parser/frontend capability.
- The compiler must admit local `@resource(CloseFn, invalid: Expr) let name = value;` declarations as a real parser/frontend capability.
- The compiler must continue to admit explicit block capture lists and preserve all four source forms: plain, `weak`, `unowned`, and `move`.
- The emitted frontend manifest must publish `frontend.pipeline.semantic_surface.objc_part8_cleanup_resource_and_capture_source_completion`.

## Positive fixture proof

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

## Non-goals

- This issue does not claim cleanup lowering.
- This issue does not claim runtime resource ownership behavior.
- This issue does not claim borrowed-pointer semantic enforcement.
