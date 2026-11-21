# Static borrowed-result control flow

Source 2.152 / System 2.78 extends permanent-storage proofs through complete,
statically bound helper definitions. The bounded proof checks every return and
uses Clang's CFG to reject normal fallthrough. Branches, switches, loops, goto,
throwing paths and catch handlers can return immutable program storage without
requiring a one-expression body.

Const local pointer aliases are followed through their initializers. C++23
implicit move wrappers preserve the same slot identity. Equal-bound C++ array
conditionals remain array lvalues until decay; the proof checks the selected
permanent array bases directly, including union member arrays. It never treats
the address of a pointer slot as its loaded pointee.

The 4096-work budget covers expression/type/body traversal and CFG work, with
bounded recursive depth. Nested callable bodies have their own return rules;
C++ capture initializers and GNU statement-expression returns still belong to
the surrounding function. Unknown/indirect/replaceable callees, mutable or TLS
bases, mutable pointer slots, recursion, exhausted work and any unproved return
remain rejected. A missing branch/default/loop/handler return cannot acquire a
permanent-storage proof.

`objc3.borrowed-result-proofs.v2` is a hard reader cutover. Published owner
contracts and consumer escape checks remain intact. Source/PCH fixtures cover
C17/C++20/C++23 with ARC/MRC; publication, decoder and native provider/consumer
controls exercise the same semantics after provider removal.

The [receipt](../support/linux_static_borrowed_control_flow_progress.json) records
58 borrowed compiler cases in the main batch and 54 final focused source/PCH
commands after the array-decay repair, six publication profiles, 64 AST controls,
nine adjacent frontend groups, 720 exact decoder replays and four native
ASan/UBSan configurations (24 operations), all passing.

This source checkpoint does not replace the SDK archive. Wider mutable/global
and callable-effect proofs remain in the [roadmap](linux_forward_development.md).
