# Aggregate values at awaited isolation boundaries

The concurrency 2.30.0 source milestone extends private executor/actor entry
frames to concrete nonunion record arguments and results. Native scalar/void
entry remains available. Record fields, constant arrays and C++ bases are
checked recursively; the existing Sendable classifier independently validates
the complete transferred source type.

ARC record fields retain their native strong/weak copy and destruction
operations. MRC records containing Objective-C or Block references remain
excluded until their independent aggregate ownership is modeled. Resource
leaves, references, volatile/atomic storage and unsupported nested value forms
are rejected by the entry shape check. Consuming parameters and receivers remain
a separate implementation boundary.

C++ thunks instantiate the same typed capture/result hooks as explicit native
frames. This preserves construction, access/deletion checks, copyable result
requirements and nonthrowing destruction. A frame already owns each by-value
parameter; the thunk forwards record parameters as xvalues into the target call,
so move-only parameters do not acquire an accidental copy requirement. Targets
retain their nonthrowing native C++ exception requirement. Native construction
failures use the existing frame unwind/cancellation channel.

The maintained witnesses include large records with arrays, actor methods and
main-executor functions, caller destination restoration, exactly-once argument
evaluation and cancellation before entry. Separate ARC record and move-only C++
providers exercise independent ownership, complete destruction and construction
failure at successive move boundaries. They execute through C17 and C++20/23
clients, ARC/MRC provider combinations and source/PCH declarations. Source-free
interface consumers reconstruct record executor/actor signatures before forming
their own entry thunks.

This milestone does not enable ordinary C++ task-ABI isolated targets,
class/super message entry, automatic actor association or full profile
conformance. The accepted SDK remains
[2.29.0](../support/linux_source_batch_2_29_acceptance.json) while the next source
implementation batch is developed and validated.

The [focused progress receipt](../support/linux_isolation_aggregate_values_progress.json)
records the compiler/tool build, two native codegen/PCH regressions, the frontend
contract suite and 48 configurations / 312 commands in each normal and ASan/UBSan
source/PCH matrix. Broader batch validation and SDK delivery remain separate.
