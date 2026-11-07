# Catch binding ownership and native handler cleanup

System source 2.34 gives Objective-C and C++ catch value bindings their own
handler-scoped owner identities. Their initialization comes from the runtime,
without a source DeclStmt. Unknown borrowed exception pointers and record fields
retain their nonescape obligation. An opaque payload cannot manufacture a
referent or borrowed return permission.

The CFG registers Objective-C catch storage outside the handler body, so body
defers complete before its lifetime ends. ARC object binding cleanup uses the
primitive release recipe. Pointer/object catch bindings establish an unwind
region even without another defer/resource action; otherwise rethrow previously
bypassed lifetime cleanup. Normal exit, return, goto and rethrow now use the same
registration-aware path. The borrowed solver initializes catch value owners and
retains unknown payload obligations instead of treating missing initialization as
an unborrowed value. Reference payloads retain conservative referent admission.

Publication exposed a foundational traversal omission: RecursiveASTVisitor did
not visit Objective-C catch parameter declarations. The source fix traverses that
declaration before the body, making it available to identity, dependency,
reflection and static/dynamic AST consumers. New controls require both native
visitor forms to visit the binding exactly once. Canonical source-free replay,
relocation and expired consumers are authored; the full shared-header compiler
rebuild and their execution remain pending.

Before that traversal rebuild, all eighteen source/PCH compiler commands pass,
including direct/local/expired/rebound owners, rethrow, retained-source borrowing,
C++ object/pointer bindings and opaque borrowed pointer/record/reference payloads.
All sixty-eight selected borrowed/lifetime/cleanup/defer compiler cases and all
eighteen existing/new AST infrastructure controls pass. The path oracle checks
body work, deferred work and exactly one catch lifetime end on every exit in four
language/ownership modes. Twelve Strict System ASan/UBSan native source/PCH
configurations pass forty-two compile/PCH/link/execution commands. They check
return, rethrow, goto, fallthrough and exact destruction after per-case pools;
ARC's throw operand may retain a separate autorelease claim until pool drain.

The maintained catch_binding_ownership fixture installs with the SDK. Optional
`--feature catch-bindings` selects all 24 O0/O2 configurations under Strict System.
Three-source owner alias correlation, implicit exception-record construction and
broader native cleanup/storage remain separate obligations. Extension ABI 24 and
service ABI 4 retain their layouts. Matched SDK delivery follows implementation
and consolidated verification.
