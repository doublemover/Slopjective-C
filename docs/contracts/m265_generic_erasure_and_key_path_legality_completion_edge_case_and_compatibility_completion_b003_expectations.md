# M265-B003 Expectations

Semantic packet contract ID: `objc3c-part3-type-semantic-model/m265-b001-v1`

Scope: Close the remaining generic-erasure and typed key-path legality gaps in the Part 3 semantic model before later lowering/runtime milestones widen the executable surface.

Required proof:
- source-only positive probe for a class-root key path such as `@keypath(Person, name)`
- source-only positive probe showing generic-erasure semantic counts remain live on the existing pragmatic-generic corpus
- negative probe for reserved generic Objective-C method syntax `- <T> ...`
- negative probe for a class-root key path component that is not a readable property on the root type
- negative probe for a non-ObjC-compatible identifier root
- negative probe for a multi-component member chain, which remains fail-closed until later executable typed key-path lowering work

Required surface updates:
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
- `package.json`

Required semantic packet facts:
- `typed_keypath_class_root_sites` is published and nonzero for the class-root happy-path fixture
- `typed_keypath_root_legality_violation_sites` and `typed_keypath_member_path_contract_violation_sites` are published and stay zero on the happy path
- reserved generic Objective-C method syntax fails closed with a reserved-for-future diagnostic
- class-root key paths fail closed when the component does not name a readable property on the root type
- typed key-path roots fail closed when they are not `self`, a known class type, or an ObjC-reference-compatible identifier
- multi-component typed key-path member chains remain fail-closed until later executable key-path lowering work

Validation artifacts:
- `tmp/reports/m265/M265-B003/generic_erasure_keypath_legality_completion_summary.json`
