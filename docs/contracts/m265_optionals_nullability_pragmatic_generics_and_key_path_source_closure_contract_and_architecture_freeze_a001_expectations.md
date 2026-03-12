# M265 Optionals, Nullability, Pragmatic Generics, And Key-Path Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part3-type-source-closure/m265-a001-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part3_type_source_closure`

Required truths:

- protocol `@required` / `@optional` partitions are published as source-only frontend state
- object-pointer nullability suffix carriers are published as source-only frontend state
- pragmatic generic suffix carriers are published as source-only frontend state
- optional-member access remains fail-closed
- nil-coalescing remains fail-closed
- typed key-path literals remain fail-closed

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m265-a001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m265_part3_type_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/a001/positive --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m265_optional_member_access_fail_closed.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/a001/optional-member-access --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m265_nil_coalescing_fail_closed.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/a001/nil-coalescing --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m265_typed_keypath_literal_fail_closed.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m265/a001/typed-keypath --emit-prefix module --no-emit-ir --no-emit-object`

Expected fail-closed diagnostics:

- optional-member access: `unexpected character '.' [O3L001]`
- nil-coalescing: `invalid expression [O3P103]`
- typed key-path literal: `unsupported '@' directive '@keypath' [O3L001]`
