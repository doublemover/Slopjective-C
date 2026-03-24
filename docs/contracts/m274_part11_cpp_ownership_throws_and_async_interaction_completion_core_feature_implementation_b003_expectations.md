# M274 Part 11 C++ Ownership, Throws, And Async Interaction Completion Expectations (B003)

Contract ID: `objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1`

Issue: `#7365`

## Required outcomes

- the frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part11_cpp_ownership_throws_and_async_interactions`
- Part 11 C++-facing callables fail closed when they combine `objc_cxx_name` / `objc_header_name` with ownership-managed callable surfaces, `throws`, or `async` / `objc_executor(...)`
- diagnostics `O3S334`, `O3S335`, and `O3S336` remain deterministic
- ABI lowering and runnable ObjC++ bridge behavior are not claimed here
- evidence lands at `tmp/reports/m274/M274-B003/part11_cpp_ownership_throws_and_async_interactions_summary.json`
