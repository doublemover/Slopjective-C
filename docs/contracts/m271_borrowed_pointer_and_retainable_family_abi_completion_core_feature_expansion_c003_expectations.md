# M271 Borrowed-Pointer And Retainable-Family ABI Completion Expectations (C003)

Contract ID: `objc3c-part8-borrowed-retainable-family-abi-completion/m271-c003-v1`
Issue: `#7329`

## Required outcomes

- keep `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract` as the single Part 8 lowering boundary
- publish one dedicated artifact/ABI packet at:
  - `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_and_retainable_family_abi_completion`
- emitted IR must publish:
  - `; part8_borrowed_retainable_abi_completion = ...`
  - `!objc3.objc_part8_borrowed_pointer_and_retainable_family_abi_completion = !{!99}`
- the ABI packet must preserve the borrowed-return attribute inventory and the retainable-family operation/compatibility-alias inventories

## Truth constraints

- this issue widens the artifact/replay surface above `M271-C001` and `M271-C002`; it does not mint a second Part 8 lowering boundary
- this issue does not claim borrowed lifetime runtime interop
- this issue does not claim escaping move-capture ownership transfer support
- this issue does not claim runnable retainable-family runtime behavior; lane-D remains responsible for runtime/helper integration
