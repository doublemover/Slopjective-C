# M265 Frontend Support For Optional Sends, Binds, Coalescing, And Typed Key Paths Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part3-type-source-closure/m265-a002-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part3_type_source_closure`

Required truths:

- protocol `@required` / `@optional` partitions remain published as source-only frontend state
- object-pointer nullability suffix carriers remain published as source-only frontend state
- pragmatic generic suffix carriers remain published as source-only frontend state
- optional binding forms `if let`, `if var`, `guard let`, and `guard var` are admitted as parser-owned source forms
- optional sends written as `[receiver? selector]` are admitted as parser-owned source forms
- nil-coalescing `??` is admitted as a parser-owned source form
- typed key-path literals such as `@keypath(self, title)` are admitted as parser-owned source forms
- optional-member access now lowers through the same nil-short-circuit path as
  optional sends and therefore no longer remains outside the admitted frontend
  surface

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m265-a002-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m265_optional_binding_send_coalescing_keypath_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/a002/positive --emit-prefix module --no-emit-ir --no-emit-object`
Expected live surface facts:

- manifest contract id is `objc3c-part3-type-source-closure/m265-a002-v1`
- source-only claim ids include optional bindings, optional sends, nil-coalescing, and typed key-path literals
- unsupported claim ids are now empty for the Part 3 lane-A packet
- optional-send, nil-coalescing, and typed key-path site counts are nonzero for the positive fixture
- `optional_member_access_fail_closed` is `false`
- `nil_coalescing_fail_closed` and `typed_keypath_literal_fail_closed` are `false`
