# M255 Dispatch Lowering ABI Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1`

## Required frozen surface

- Semantic surface path: `frontend.pipeline.semantic_surface.objc_runtime_dispatch_lowering_abi_contract`
- Canonical runtime dispatch symbol: `objc3_runtime_dispatch_i32`
- Compatibility bridge symbol: `objc3_msgsend_i32`
- Selector lookup symbol: `objc3_runtime_lookup_selector`
- Selector handle type: `objc3_runtime_selector_handle`
- Receiver ABI type: `i32`
- Selector ABI type: `ptr`
- Fixed argument ABI type: `i32`
- Fixed argument slot count: `4`
- Result ABI type: `i32`

## Boundary rules

1. The default lowering target remains the compatibility bridge until `M255-C002` changes code generation explicitly.
2. The canonical live runtime ABI is still frozen now so `M255-C002` must preserve the same selector lookup and handle boundary.
3. The selector operand model remains `selector-cstring-pointer-remains-lowered-operand-until-m255-c002`.
4. The selector handle model remains `runtime-lookup-produces-selector-handle-before-live-dispatch`.
5. The argument padding model remains `zero-pad-to-fixed-runtime-arg-slot-count`.
6. The deferred cases model remains `super-nil-direct-runtime-entrypoint-cutover-deferred-until-m255-c003`.
7. Native IR must publish a replay-stable boundary comment rooted at `; runtime_dispatch_lowering_abi_boundary =`.
8. Validation evidence must land under `tmp/reports/m255/M255-C001/`.
