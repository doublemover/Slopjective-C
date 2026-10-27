# Foreign consumption and Block owner claims

The System 2.5.0 source span gives `cf_consumed` parameters a distinct native
callable-type flag. Direct functions/messages, typed function/Block callbacks,
C++ constructors, operators and member pointers retain the foreign ownership
obligation under ARC and MRC. Canonical printing, PCH, AST import and source-free
interfaces preserve it. Named parameters retain the annotation under canonical
declaration polishing; Itanium callback/member-pointer mangling distinguishes
foreign-consumed overloads. Native type/declaration USRs retain consumed,
noescape and retained-result callback contracts. Implicit erasure, conflicting unannotated function declarations
and explicit casts changing the contract diagnose.

Foreign consumption is manual under both memory modes. The borrowed-flow pass
expires the known consumed claim and its aliases, preserves evaluated argument
identities and rejects overlapping borrowed arguments. A later replacement
owner starts a new known identity. Callee-entry consumption propagates to
exception handlers; throwing before a nonthrowing consumer is entered preserves
the original claim. The foreign flag remains separate from `ns_consumed`, so
native IR never selects ARC object-transfer retains or releases for a raw
foreign pointer.

MRC `ns_consumed` Block owners now participate in the same known-claim analysis.
Borrowed-return declarations may name a Block owner. Existing independent
object-owner lifetime pins retain their earlier protection. Borrowed bodies
requiring an implicit copy of an unknown Block lifetime binding remain
diagnosed until its acquisition/unwind operations have a complete CFG model. ARC Block
sources retain their independent strong owner and existing parameter ABI.
Indirect C++ member calls recover their bound callable prototype for consumed
and noescape parameters, covering object and foreign claims without treating
the implicit receiver as a written argument.

Native structural equivalence and ODR hashing include callable parameter
metadata. AST import cannot unify callbacks whose consumed, foreign-consumed or
noescape contracts differ. Public declaration and nested callable ownership
identities bind the same facts.

This span does not infer ownership effects from function names. Unannotated
foreign release APIs, retainable-family operations, `os_consumed`, independent
manual ownership counts, unknown/interprocedural aliases and consuming isolation
entry remain separate work. It does not claim complete CF interoperability or
Strict System conformance. Regenerate dependent objects, PCH, interfaces and
packages for the compiler cutover. Extension ABI 21, interface schema 2.2 and
runtime layouts remain unchanged; SDK 2.33 remains the accepted archive.

Maintained witnesses cover source/PCH in C17/C++20/C++23 under ARC/MRC,
conditional and copied aliases, direct/indirect/Block/member calls, consumed
Block owner boundaries, exception ordering, explicit/implicit callback erasure,
source-free imports, native AST equivalence/ODR identity and raw foreign-call IR.

The maintained C++ binary-package fixture also exports foreign consumed callbacks,
borrowed owner-return contracts and retained-result callback aliases. Its
source-free consumer checks exact foreign consumption/destruction counts and
retained object destruction after callback invocation. The qualifier requires
the corresponding ownership identities in the distributed interface.

The [System 2.6.0 source span](linux_borrowed_block_pins.md) supplies the CFG
acquisition/unwind model for lifetime pins loaded from ordinary Block slots and
for ARC consumed Block argument copies. Other implicit Block copy boundaries
remain as documented there.

The [combined ownership receipt](../support/linux_callable_ownership_batch_progress.json)
records 390 compiler regressions, 188 native unit tests, all 33 frontend suites,
15 tooling tests and 24 source-free package configurations / 96 commands across
normal, eager-import and ASan/UBSan execution. Three platform compiler cases
remain unsupported. Broader combined runtime/sanitizer and SDK delivery remain
separate checkpoints.
