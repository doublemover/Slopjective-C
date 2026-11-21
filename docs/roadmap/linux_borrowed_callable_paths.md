# Callable paths in captured records and arrays

System 2.12.0 propagates known invocation effects through callable fields and
array elements captured by synchronous concrete Blocks and C++ lambdas. Nested
record fields, aggregate init-captures, copied closures and forwarding through
another closure retain the selected path. Invoking one callback does not consume
the owners of unused siblings. A dynamic array index conservatively combines
possible elements.

Cached CFG summaries identify callable leaves with capture-relative field/index
selectors. The caller reconstructs those selectors in its own storage graph;
analysis-local path identifiers never cross the summary boundary. Lambda objects
are opaque callable leaves, and wildcard array slots do not taint every constant
element. Read-only copies preserve callable facts without invoking them.

ARC compiler-generated structural record copies are qualified alongside trivial
copies. Records with bases, virtual dispatch, unions or user-provided nontrivial
copy operations retain explicit contract boundaries. The checker does not infer
arbitrary copy bodies or general heap/global callback mutation. Mutable/reference,
generic and async capture-state limitations remain as documented in their spans.

Native witnesses cover C++20/23, ARC/MRC and PCH, nested fields, direct and aggregate
init-captures, copies, constant/dynamic arrays, unused siblings and nested Block/
lambda forwarding. Frontend witnesses repeat lifetime and precision checks and
publish a concrete record-captured consuming closure through a source-free
interface. Broader validation is batched with System 2.10–2.11; SDK 2.33 remains
the accepted archive. Full Strict System conformance is not claimed.

The [combined System 2.10–2.12 receipt](../support/linux_captured_callable_ownership_progress.json)
records the completed native, frontend and focused source-free package batch.
Broader runtime/sanitizer matrices and installed delivery retain their separate
checkpoints.

The [System 2.14 array extension](linux_borrowed_large_arrays.md) preserves
conservative effects beyond eager element tracking. Its large-array summaries
can include unrepresented siblings; precise selection applies within the eager
range.
