# M271 Resource, Borrowed-Pointer, And Capture-List Source Closure Expectations (A001)

Contract ID: `objc3c-part8-resource-borrowed-capture-source-closure/m271-a001-v1`

## Required outcomes

- The frontend must admit `__attribute__((objc_resource(close=..., invalid=...))) let name = value;` as the current local resource source surface.
- The frontend must admit `borrowed` as a contextual callable-signature qualifier for pointer-bearing parameter and return spellings.
- The frontend must admit callable `__attribute__((objc_returns_borrowed(owner_index=N)))` on functions and Objective-C methods.
- The frontend must admit explicit block capture lists in the form `^[weak x, unowned y, move z] { ... }`.
- The emitted manifest must publish deterministic counts under `frontend.pipeline.semantic_surface.objc_part8_resource_borrowed_and_capture_list_source_closure`.
- This issue remains source-only. It must not claim resource cleanup lowering, borrowed escape enforcement, or runnable capture ownership/runtime behavior.
