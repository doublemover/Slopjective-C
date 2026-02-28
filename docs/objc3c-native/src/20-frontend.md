# Frontend Parser + AST Contract Addendum

## M153 frontend method lookup-override-conflict parser surface

Frontend parser/AST now emits deterministic method lookup, override lookup, and conflict packets for Objective-C
protocol/interface/implementation declarations.

M153 parser/AST surface details:

- method lookup helper anchors:
  - `BuildObjcMethodLookupSymbol(...)`
  - `BuildObjcMethodOverrideLookupSymbol(...)`
  - `BuildObjcMethodConflictLookupSymbol(...)`
  - `BuildObjcMethodLookupSymbolsLexicographic(...)`
  - `BuildObjcMethodOverrideLookupSymbolsLexicographic(...)`
  - `BuildObjcMethodConflictLookupSymbolsLexicographic(...)`
- parser assignment anchors:
  - `AssignObjcMethodLookupOverrideConflictSymbols(...)`
  - `FinalizeObjcMethodLookupOverrideConflictPackets(...)`
  - `method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);`
  - `method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);`
  - `method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);`
- container packet anchors:
  - `decl->semantic_link_symbol = "protocol:" + decl->name;`
  - `decl->method_lookup_symbols_lexicographic`
  - `decl->override_lookup_symbols_lexicographic`
  - `decl->conflict_lookup_symbols_lexicographic`

Deterministic grammar intent:

- lookup/override/conflict packets are attached per method and replay-stable for sema handoff.
- container-level packet vectors are normalized as sorted unique symbols.

Recommended M153 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py -q`
