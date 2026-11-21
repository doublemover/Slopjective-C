# Read-only pointer and callback bindings in permanent-result proofs

Source 2.163 / System 2.89 proves ordinary by-value pointer/callback parameters
and initialized local pointer/callback aliases without requiring written
top-level `const`. A bounded whole-body use scan admits a slot only when every
use reads its value. The return proof then substitutes the actual caller argument
or local initializer and reuses the complete helper-return/target proof.

Assignments, increments, address/reference escapes, reference captures, volatile
slots and mutable global/TLS storage cannot establish a read-only binding.
`__block` locals and resource owners retain conservative admission because their
implicit operations can change storage. Copied lambda/Block captures keep their
independent snapshot semantics; nested callable bodies do not count as writes to
the original slot. C++23 implicit-move no-op wrappers on pointer returns are
transparent only around the required native value load.

The use scan charges the same 4096-unit AST/CFG budget as return and target
analysis. Depth 32, 64 unique targets, complete normal returns, exact physical
callable ABI, replaceable-function gates and caller owner obligations remain in
force. This does not infer arbitrary mutation effects or escaping references.
`objc3.borrowed-result-proofs.v6` requires new readers and rebuilt interfaces;
runtime/extension ABI layouts are unchanged.

Eighteen C17/C++20/23 ARC/MRC source/PCH commands cover direct/chained/higher-order
forwarding, local pointer/callback aliases, copied captures and negative mutation,
address/reference escape, reference capture, `__block`, unknown-origin and budget
controls. AST, publication and source-free native tests retain the same helper
behavior after provider removal. Full rebuilt compiler/SDK acceptance remains
separate from the focused production-source checks.
