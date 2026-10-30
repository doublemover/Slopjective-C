# Dynamic storage in native async frames

Concurrency 2.6.0 admits variable-length local arrays and builtin `alloca` in
Linux x86-64 explicit native frames. The
[acceptance receipt](../support/linux_dynamic_frame_storage_acceptance.json)
binds the source checkpoint, rebuilt compiler, instrumented matrices, installed
SDK and exact-archive Debian/Ubuntu evidence. The preceding
[2.5.0 frame-value delivery](linux_cxx_frame_values.md) retains its original scope. Service ABI 4, frame ABI 1,
extension ABI 21 and interface schema 2.1 are unchanged.

A native frame can resume on another executor thread. Dynamic storage therefore
belongs to heap allocations owned by that frame. It cannot refer to the stack
of the factory invocation or a prior resume callback.

VLAs retain lexical lifetime. The compiler registers normal and exceptional
cleanup after allocation succeeds, preserving source defer ordering and native
array destruction. Loop iterations and backward jumps release the previous
allocation. Pointers to variably modified types retain their ordinary type and
bound evaluation without creating a second allocation.

Builtin `alloca`, its uninitialized variants, and explicit-alignment variants
retain invocation lifetime. An aligned header links each allocation into the
frame's private allocation chain. The chain remains live through the function's
structured child join, result publication and owned capture destruction. Frame
cleanup clears links before freeing them. Explicit alignment is preserved for
both array storage and builtin allocations.

Array dimension multiplication, element byte sizing, header addition and
alignment rounding use checked unsigned arithmetic. Overflow or an allocator
failure follows the existing owned `OUT_OF_MEMORY` service-error path and runs
all previously registered cleanup. Cancellation retains its established
precedence. Ordinary synchronous functions keep their existing stack lowering.
Builtin zero/pattern auto-initialization and uninitialized exclusions are
preserved; no new runtime allocator or public ABI record is introduced.

The maintained spawn matrix exercises C17, C++20/23, ARC/MRC, O0/O2 and source/PCH
transport. Witnesses cover arrays and array views across awaits, 128-byte
alignment, all allocation spellings, function-lifetime storage after loops,
lexical defer reads, cancellation, backward jumps, foreign exception cleanup,
structured final joins, unstarted disposal, four arithmetic-overflow edges and
two deterministic allocator failures. A separate C allocator fixture delegates
to `posix_memalign`, keeping allocator ownership visible to sanitizers.

The source/PCH matrix passes 24 configurations / 132 commands in each normal,
ASan/UBSan and TSan mode (72 / 396 total). Clients and generated allocation and
cleanup operations are rebuilt; unchanged service archives and instrumented GNU
runtime variants are reused. The normal source run uses the accepted 2.5.0 GNU
runtime. The new SDK rebuilds GNU with the new compiler and passes nine clients.

The rebuilt foundation passes 368 Clang/index/query regressions (365 ObjC3),
three unsupported cases and 166 native AST/matcher/clangd contracts. All 32
facade suites, 37 installed consumer checks, 13 selected integration nodes
(76 runtime checks) and 33 installed SDK gates pass. The installed five-family
matrix covers 168 configurations / 966 commands. Three focused compiler/PCH
regressions and 22 tooling checks pass; the final 12 authority/onboarding checks
overlap that tooling scope. Both fresh Debian 13 and Ubuntu 26.04 pass the exact
archive as ordinary users, including relocation, packages and debugger clients.
Both owned containers are removed. One active SDK and archive remain after
superseded-artifact cleanup.

C++ reference parameters, implicit receivers, nontrivial class semantic-package
projection and full profile conformance remain separate milestones.
