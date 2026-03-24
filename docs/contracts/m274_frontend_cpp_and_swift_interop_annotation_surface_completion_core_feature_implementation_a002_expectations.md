# M274 Frontend Cpp And Swift Interop Annotation Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1`

## Required outcomes

- the frontend admits parser-owned Swift-facing callable annotations via `objc_swift_name(named("..."))`
- the frontend admits parser-owned Swift-private callable annotations via `objc_swift_private`
- the frontend admits parser-owned C++-facing callable annotations via `objc_cxx_name(named("..."))`
- the frontend admits parser-owned header-name callable annotations via `objc_header_name(named("..."))`
- the frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part11_cpp_and_swift_interop_annotation_source_completion`
- the emitted packet stays source-only and deterministic while semantic expansion remains deferred

