# Native async in strict profiles

Compile `main.m` as Objective-C C17 or Objective-C++20/23 with each of
`-fobjc3-profile=strict`, `strict-concurrency` and `strict-system`, under ARC
and MRC. For PCH transport, compile `runtime.h` in the matching header language
and pass `-DOBJC3_STRICT_ASYNC_PROFILES_PCH` to the consumer.

The maintained feature runner selects the complete 72-configuration
source/PCH, ownership, optimization, language and strict-profile matrix with
`--feature strict-async`. This extended selection is optional; existing default
feature selections retain their scope. The fixture is installed with the SDK.

The witness checks lazy native frames, typed child results, structured groups,
C Optional result iteration, C++ source async tasks and owned group callbacks,
exact defer counts, 128-byte parameter alignment, cancellation before entry and
unstarted frame destruction. Native task definitions use the Objective-C 3
aligned-allocation default without an extra source compilation flag.
Manual executor pumping makes completion and cleanup assertions deterministic.
This establishes focused implementation behavior; public async/actor feature
predicates and full profile qualification retain their separate acceptance gates.
