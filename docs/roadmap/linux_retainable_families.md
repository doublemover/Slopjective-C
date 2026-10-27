# Canonical retainable C families

System 2.9.0 implements the canonical header attributes from Part 8 §8.4:
`objc_retainable_family(Name)` on a pointer typedef/using alias and
`objc_family_retain`, `objc_family_release`, `objc_family_autorelease` on its
unary operations. Contracts follow typedef sugar without changing pointer ABI
or making C aliases nominal types. An alias cannot change its inherited family.

Operations require a concrete synchronous signature without checked throws, one
parameter from the named family, and a matching family result for retain or
autorelease; release returns void. Conflicting operations and redeclarations
that change family or lose the parameter/result family diagnose. ObjC-integrated
families additionally use `NSObject`: Clang's native ARC ownership applies and
direct manual family operations are rejected under ARC. Ordinary C families
permit their explicit operations in both ARC and MRC.

Canonical releases expire known borrowed owner claims and aliases. Existing
consumed-parameter metadata remains independent. A retain/autorelease operation
after a known release cannot resurrect a dead claim in a borrowing body. The
static analyzer receives explicit increment/decrement/autorelease summaries and
identity-result behavior, independent of function naming conventions. This does
not add general ownership counts to the mandatory borrowed CFG analysis.

Native structural equivalence and ODR hashing distinguish family types and
operations across AST contexts; incompatible declarations cannot be unified.
Generated PCH/AST import and polished declarations preserve family markers,
operation names and `NSObject`. Imported family identifiers belong to the
consumer AST context; operation contracts are installed before recursive
redeclaration import so compatible declarations keep one canonical function.
Native interface ownership identity includes the
family name, integration status and operation. Source-free readers reparse those
canonical declarations under the existing required native-ownership semantic;
changed contracts cannot reuse an earlier locked interface identity.

Maintained witnesses cover C17/C++20/23 under ARC/MRC, PCH, invalid signatures,
conflicting declarations, borrowed release effects, native ARC/pointer ABI,
analyzer retain/release balance, AST import, polished reparse and source-free
interfaces. The runtime package fixture also publishes a canonical family and
release operation with its existing exact-consumption witness. An integrated
family alias keeps an object alive after its original ARC owner is cleared and
checks exact destruction after the final alias ends. General manual
counts, indirect operation metadata, complete transfer/optimizer analysis and
full Strict System conformance remain open. Optional family sugar is outside
this canonical-header span. SDK 2.33 remains the accepted installed archive.
