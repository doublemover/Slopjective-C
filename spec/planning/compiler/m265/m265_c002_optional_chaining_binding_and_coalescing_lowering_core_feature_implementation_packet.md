# M265-C002 Packet

Issue: `M265-C002`

Objective:
- implement real optional chaining lowering for `?.` on top of the already-landed optional binding/send/coalescing lowering packet

Code anchors:
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

Spec anchors:
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`

Acceptance detail:
- contiguous `?.` tokenizes as one punctuator rather than failing in the lexer
- parser lowers `expr?.member` into the existing optional-send/message-send machinery with explicit sugar tracking
- source-closure truth stops claiming optional-member access is unsupported
- the live lowering packet truthfully states that optional-member access rides the optional-send nil-short-circuit path
- the positive fixture proves chained optional-member access compiles, links, runs, and returns `9`
- the negative fixture proves non-ObjC-reference receivers still fail closed with `O3S206`
- deterministic evidence lands under `tmp/reports/m265/M265-C002/`

Non-goals:
- no executable typed key-path runtime yet
- no scalar optional-member access support
- no broader Part 3 runtime closeout beyond the already-supported optional lowering slice
