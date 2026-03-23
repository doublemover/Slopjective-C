# M271 Capture-List And Retainable-Family Legality Completion Expectations (B004)

Issue: `#7326`

## Contract

- Contract ID: `objc3c-part8-capture-list-retainable-family-legality/m271-b004-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part8_capture_list_and_retainable_family_legality_completion`
- Dependency contract: `objc3c-part8-borrowed-pointer-escape-analysis/m271-b003-v1`

## Required truth

- `M271-B004` stays a live semantic/accounting issue.
- The compiler must publish one deterministic Part 8 packet for capture-list and retainable-family legality completion.
- The packet must consume the already-landed `M271-B003` sema packet.
- Lowering and runtime behavior must remain explicitly deferred.

## Positive proof

- The positive fixture must compile through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
- The emitted manifest must publish deterministic zero-violation counts for:
  - duplicate explicit captures
  - non-object weak/unowned explicit captures
  - unused explicit captures
  - conflicting retainable-family annotations
  - invalid family operation callable shapes
  - invalid compatibility-alias callable shapes

## Negative proof

- Negative fixtures must fail closed with:
  - `O3S301` for duplicate explicit captures
  - `O3S302` for weak/unowned explicit captures on non-object bindings
  - `O3S304` for conflicting retainable-family annotations
  - `O3S306` for compatibility aliases without an Objective-C reference return type

## Anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `package.json`
