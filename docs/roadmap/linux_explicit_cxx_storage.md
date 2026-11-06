# Explicit C++ lifetimes and dependent native-frame awaits

Concurrency source 2.105 admits C++ aggregate and union values whose user-provided
constructors/destructor define their ownership, including MRC object fields and
explicit active-arm union lifetimes. Native frame hooks perform ordinary C++
construction, copying and destruction. Deleted/inaccessible operations,
noncopyable results and throwing destruction still fail hook admission.
Sendable checking remains independent and borrowed leaves remain forbidden.
Implicit managed unions and Part 8 resource-union cleanup keep their existing
restrictions. Static C++ member entries now use the direct isolation call path.

Nontrivial call arguments materialize a source snapshot before moving into the
thunk parameter. This preserves native constructors and cleanup in source and
PCH replay. Storage-hook lookup rejects recovered expressions and undefined
specializations without casting them into calls or emitting recovered code.

Native-frame templates retain dependent await operands until substitution,
then rebuild isolation, async/task adaptation and result contracts. They use the
serialized native suspension expression and never acquire a C++ promise.
Dependent callback calls and plain nonsuspending substitutions follow the same
rule. Await inside defer remains an error. Published dependent native awaits
require `objc3.native-dependent-await.v1`; every reader and codec/contract check
rejects missing, invented or unsupported requirements.

Four new compiler regressions pass 32 source/PCH/admission commands across
C++20/23 and ARC/MRC; two existing C++ frame regressions also pass. The maintained
runtime adds 296 exact-lifetime scenarios per configuration for union/aggregate
parameters/results, synchronous/frame/task/static entries, three caller routes,
cancellation and unused frames. The C17/MRC/O0/source isolation configuration
passes all 25 commands, compiling independent ARC/MRC providers. Four package
source modes and affected frontend/test syntax checks pass. Replay, capability
and source-free consumer controls are authored. Optimized, instrumented,
frontend/package and final SDK consolidation remain pending; SDK 2.62 is unchanged.
