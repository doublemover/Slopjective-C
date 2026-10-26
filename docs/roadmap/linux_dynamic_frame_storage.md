# Dynamic storage in native async frames

The concurrency 2.6.0 source milestone admits variable-length local arrays and
builtin `alloca` in Linux x86-64 explicit native frames. The accepted installed
SDK remains the [2.5.0 delivery](linux_cxx_frame_values.md) until the coordinated
SDK build and qualification for this span completes. Service ABI 4, frame ABI 1,
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

C++ reference parameters, implicit receivers, nontrivial class semantic-package
projection and full profile conformance remain separate milestones.
