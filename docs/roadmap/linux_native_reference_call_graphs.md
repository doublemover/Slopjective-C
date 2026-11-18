# Caller storage through closed reference helpers

Source 2.230 / System 2.156 extends the closed native helper graph to scalar
reference parameters whose actual storage cells are completely discovered.
Assignments through a formal reference update the caller's local, global,
record-field or fixed-array cells. Const reference loads read that same source;
composed helper calls preserve it. Receiver-independent native methods also
participate, including composed member helpers, qualified virtual calls and
final targets; open virtual dispatch remains unproved. Ordinary independent stores no longer become
unknown pointer/resource escapes simply because they pass through a helper.

Reference parameters map to the union of their admitted actual cells. Shared
callee bodies are indexed once, then all writes and escapes are resolved again
against the completed argument graph. This retains cells supplied by later
calls regardless of caller traversal order. Unknown operations in a helper
escape the actual cells, including those discovered after its first traversal.
Repeated calls intentionally retain a may-source union, without claiming
per-invocation strong updates or erasing earlier loans.

Pure scalar pointer-load helpers also preserve known pointee loans/resources
without claiming reference-result aliases. Copying a byte out of borrowed
storage stays an independent owned value; mutable pointer-parameter writers
retain their separate requirements.

The Source 2.229 graph/work/depth limits remain. Actual reference cells must be
closed; unresolved reference targets and discovery exhaustion remain conservative.
Volatile/atomic reference parameters do not establish these summaries. Reference
results, receiver state mutation, constructor effects, pending dependent
bodies, lazy async analysis and general foreign/interprocedural lifetime/count
precision retain their separate requirements.

The preceding frontend rejects ordinary pointer and integer reference-helper
stores. Maintained C17/C++20/C++23 ARC/MRC source/PCH controls distinguish those
from actual borrowed/resource transfers. Shared-callee tests defend both later
write targets and later escapes. The native source-free owner publishes C++
reference store/load/chain helpers and checks caller locals, globals, record
fields, fixed arrays and both shared-callee result selections. This focused
execution passes in all six language/ownership configurations. All 1008 new
and adjacent source/PCH commands and 100 tooling checks pass. Full matching
SDK, native ARM and profile acceptance remain pending.
