# Checked executor queue locks

The first contract slice for
[#8285](https://github.com/doublemover/Slopjective-C/issues/8285) checks the executor
queue and stopping flag. Both fields require that executor's queue mutex.
`run_callback` excludes the queue lock, preserving callback reentry. Pump,
task, reference, actor and group locks remain separate, unannotated domains.
In particular, a child's structured index remains protected by its parent's
mutex; this pilot does not invent a different ownership rule.

`ExecutorQueueMutex` wraps the existing `std::mutex`. Its scoped guard owns the
same `std::unique_lock<std::mutex>` used by condition-variable waits. Waiting
releases/reacquires that underlying mutex, and the predicate explicitly requires
the queue capability. The wrapper adds no scheduler state; compile-time checks
preserve mutex size/alignment and guard size. Executor handles remain opaque.

Every canonical runtime translation unit enables Clang `-Wthread-safety` and
`-Werror=thread-safety`. Integrated and standalone builds share the same ordinary
warning owner; standalone configuration now exports compilation commands and
uses RelWithDebInfo by default. Assertions remain active in registered tests.
No broad warning-as-error or directory-wide diagnostic suppression was added.

Run the paired compiler controls with:

```sh
python3 -B -m scripts.objc3c_toolchain.qualify_executor_locks \
  --compiler "$OBJC3C_BOOTSTRAP_LLVM_ROOT/bin/clang++" \
  --output out/executor-lock-contract
```

The runner binds the compiler hash/version and owned source/header bytes. The
corrected fixture and actual executor compile cleanly. Four deliberately wrong
paths require a thread-safety error at their source site: an unlocked write, a
missing helper precondition, a callback invoked under the excluded lock, and an
unannotated wait predicate. A fifth negative control removes a queue guard from
a temporary copy of the real implementation. Crashes, timeout exits, unrelated
parse failures and missing/wrong diagnostic locations cannot satisfy detection.
The compiler version must match the pinned implementation build compiler.

Standalone CTest registers these controls. The native assurance sanitizer lane
also executes them through the matched product Clang and validates their complete
receipt before accepting runtime sanitizer evidence. This is mutex-contract
analysis of runtime implementation C++; it does not claim Objective-C language
analyzer coverage or whole-program race/deadlock/lifetime correctness.

The pilot passes all **nine standalone runtime CTests**, including the seven
compile-control cases, and verifies warnings on all **17 service compile
commands**, retaining the two explicit Objective-C++ language overrides. The
existing service/collection tests exercise queue draining, callbacks, task/group
lifetime and ownership. Full fork-matched clang-tidy implementation analysis,
reviewed checker/suppression policy and dependency-complete TU accounting remain
open under #8285. Compiler-profile validation and TSan are separate evidence.
