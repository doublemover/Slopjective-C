# Linux paired-sidecar fuzzing

The initial Linux discovery owner feeds exact `(source bytes, semantic sidecar JSON bytes)` pairs into the production semantic-interface decoder. Four first-party implementation translation units carry libFuzzer edge feedback, ASan, UBSan and source coverage. The matched native foundation archive and Clang/LLVM libraries satisfy production dependencies; their presence does not count as whole-frontend feedback coverage.

The target rejects malformed framing without repairing it, preserves embedded NUL and invalid UTF-8 bytes, and gives each half an exact-sized allocation. An accepted input must preserve its semantic fingerprint through decode/encode/decode, produce byte-stable canonical JSON and remain invariant when unordered declarations and required semantics are permuted. The replay executable records the ordered input byte count and SHA-256, outcome, canonical output SHA-256 and before/after descriptor and thread counts.

Qualification generates ten reviewed boundary and semantic seeds, replays every byte exactly, checks repeated A/B/A and maximum-then-empty sequences, and then runs equal bounded raw/dictionary and structured-pair discovery spans. Wall time, input size, per-input timeout, allocator size, RSS and retained discovery bytes are bounded. A live retained-byte monitor kills a campaign that crosses its cap. Promotion requires actual libFuzzer counters in every selected implementation object, runtime feedback in `DecodeSemanticInterfaceSidecar`, and exported branch coverage from `interface_codec.cpp`.

Two test-owned binaries prove useful failure detection. One aborts after a valid decode and digest check; the other performs a one-byte read beyond an exact source allocation. Each failure must have its expected symbolized class, reduce for a fixed 12-second budget to an exact artifact no larger than the original, reproduce twice from the unchanged reduced bytes and still reproduce from the original input afterward. This is bounded crash reduction, not a claim that the resulting bytes are globally minimal. An independently compiled production decoder without feedback must fail the instrumentation gate.

Crash minimization admits exactly three reviewed process dispositions: the
injected fault exit, a runner-owned wall-clock stop, or libFuzzer's natural
zero exit after it can no longer shrink the reproducer. Natural completion is
accepted only by minimization commands. It does not replace the required
diagnostic-class check, nonempty size-bounded artifact, two exact fresh replays
or original-input replay. Retained-byte termination, a clean unbounded timeout,
signals and every other exit remain failures. A pinned LLVM 23.1.5 control
reproduces the natural completion path after reducing a faulting input.

The integrated owner now also executes the production v2/v3 syntax/Sema
embedding targets. Six first-party implementation translation units and the
matched `compiler-fuzz-asan-ubsan` Clang/LLVM graph carry engine feedback and
ASan/UBSan instrumentation. Admission requires actual feedback from production
compile-session, patched parser and patched Sema functions, separately from
paired decoder reach. First-party source coverage remains separately labeled.
Exact frontend replay and repeated reset sequences check persistent cleanup;
a controlled frontend result-cleanup defect must be detected, reduced and
replayed. Missing decoder feedback remains an explicit rejecting control.

The existing native assurance lane selects the frontend and two paired discovery
strategies with bounded time, memory, retained bytes and durable diagnostics.
Live integrated hosted acceptance remains under #8287/#8283. This does not
qualify code generation, AFL++/CMPLOG, every production-size boundary or an
exhaustive frontend grammar.
