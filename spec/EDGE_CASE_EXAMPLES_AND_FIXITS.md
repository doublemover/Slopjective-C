# Edge-Case Examples and Fix-its

This document provides the GH #47 edge-case corpus for punctuation-heavy
features and links each example to an executable conformance artifact.

## Scope

- Feature groups covered with at least 5 edge cases each:
  - optional member access `?.`,
  - postfix propagation `?`,
  - nil-coalescing `??`,
  - capture lists,
  - borrowed pointer punctuation,
  - optional spelling outcomes,
  - mangling invariants outcomes,
  - reification outcomes.
- Cross-feature coverage includes `try await` plus `?.` and postfix `?`.
- Every example maps to one JSON test spec under
  `tests/conformance/examples/`.

## Executable Artifact Contract

Conformance artifacts are machine-readable JSON specs that include:

- source snippet (`source`),
- parse expectation (`expect.parse`),
- diagnostics (`code`, `severity`, `span`, `message`),
- fix-its (`expect.diagnostics[*].fixits`) for mechanical rewrites.

Primary corpus index: `tests/conformance/examples/manifest.json`.

## Coverage Summary

| Group | Example count | Artifact directory |
| --- | ---: | --- |
| `?.` | 5 | `tests/conformance/examples/optional_chaining/` |
| Postfix `?` | 5 | `tests/conformance/examples/postfix_propagation/` |
| `??` | 5 | `tests/conformance/examples/nil_coalescing/` |
| Capture lists | 5 | `tests/conformance/examples/capture_lists/` |
| Borrowed pointer punctuation | 5 | `tests/conformance/examples/borrowed_pointer_punctuation/` |
| Cross-feature (`try await` + `?.` + postfix `?`) | 5 | `tests/conformance/examples/cross_feature/` |
| Optional spelling outcomes | 7 | `tests/conformance/examples/optional_spelling/` |
| Mangling invariants outcomes | 8 | `tests/conformance/examples/mangling_invariants/` |
| Reification outcomes | 13 | `tests/conformance/examples/reification_outcomes/` |

Total examples: 58.

## Optional Member Access `?.`

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-OPTCHAIN-01` | Scalar property selected through `?.` | Semantic error + fix-it | [EC-OPTCHAIN-01](../tests/conformance/examples/optional_chaining/EC-OPTCHAIN-01.json) |
| `EC-OPTCHAIN-02` | Non-contiguous `? .` tokenization | Parser error + fix-it | [EC-OPTCHAIN-02](../tests/conformance/examples/optional_chaining/EC-OPTCHAIN-02.json) |
| `EC-OPTCHAIN-03` | Optional message send returning scalar | Semantic error + fix-it | [EC-OPTCHAIN-03](../tests/conformance/examples/optional_chaining/EC-OPTCHAIN-03.json) |
| `EC-OPTCHAIN-04` | Left-to-right chaining parse shape | Accept | [EC-OPTCHAIN-04](../tests/conformance/examples/optional_chaining/EC-OPTCHAIN-04.json) |
| `EC-OPTCHAIN-05` | `?.` combined with postfix `?` then `+` | Diagnostic + parenthesize fix-it | [EC-OPTCHAIN-05](../tests/conformance/examples/optional_chaining/EC-OPTCHAIN-05.json) |

## Postfix Propagation `?`

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-POSTFIXQ-01` | Follow-token violation (`? +`) | Parser/diagnostic error + fix-it | [EC-POSTFIXQ-01](../tests/conformance/examples/postfix_propagation/EC-POSTFIXQ-01.json) |
| `EC-POSTFIXQ-02` | Cast ambiguity `(T)x?` | Parser error + disambiguation fix-it | [EC-POSTFIXQ-02](../tests/conformance/examples/postfix_propagation/EC-POSTFIXQ-02.json) |
| `EC-POSTFIXQ-03` | Non-optional return context | Semantic error + return-type fix-it | [EC-POSTFIXQ-03](../tests/conformance/examples/postfix_propagation/EC-POSTFIXQ-03.json) |
| `EC-POSTFIXQ-04` | Comma follow token in argument list | Accept | [EC-POSTFIXQ-04](../tests/conformance/examples/postfix_propagation/EC-POSTFIXQ-04.json) |
| `EC-POSTFIXQ-05` | Nil-to-throw carrier mismatch | Semantic error + explicit throw-path fix-it | [EC-POSTFIXQ-05](../tests/conformance/examples/postfix_propagation/EC-POSTFIXQ-05.json) |

## Nil-coalescing `??`

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-COALESCE-01` | Right associativity | Accept | [EC-COALESCE-01](../tests/conformance/examples/nil_coalescing/EC-COALESCE-01.json) |
| `EC-COALESCE-02` | Precedence with ternary | Accept | [EC-COALESCE-02](../tests/conformance/examples/nil_coalescing/EC-COALESCE-02.json) |
| `EC-COALESCE-03` | Non-contiguous `? ?` tokenization | Parser error + contiguous fix-it | [EC-COALESCE-03](../tests/conformance/examples/nil_coalescing/EC-COALESCE-03.json) |
| `EC-COALESCE-04` | `a??b:c` likely ternary intent | Diagnostic + ternary rewrite fix-it | [EC-COALESCE-04](../tests/conformance/examples/nil_coalescing/EC-COALESCE-04.json) |
| `EC-COALESCE-05` | RHS type mismatch | Semantic error + type-correct fallback fix-it | [EC-COALESCE-05](../tests/conformance/examples/nil_coalescing/EC-COALESCE-05.json) |

## Capture Lists

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-CAPTURE-01` | Missing comma between capture items | Parser error + insertion fix-it | [EC-CAPTURE-01](../tests/conformance/examples/capture_lists/EC-CAPTURE-01.json) |
| `EC-CAPTURE-02` | Trailing comma before `]` | Parser error + removal fix-it | [EC-CAPTURE-02](../tests/conformance/examples/capture_lists/EC-CAPTURE-02.json) |
| `EC-CAPTURE-03` | Duplicate capture of same binding | Diagnostic + remove duplicate fix-it | [EC-CAPTURE-03](../tests/conformance/examples/capture_lists/EC-CAPTURE-03.json) |
| `EC-CAPTURE-04` | Mixed `take(...)` and `weak` capture forms | Accept | [EC-CAPTURE-04](../tests/conformance/examples/capture_lists/EC-CAPTURE-04.json) |
| `EC-CAPTURE-05` | Escaping block captures borrowed pointer | Strict-system error + ownership-copy fix-it | [EC-CAPTURE-05](../tests/conformance/examples/capture_lists/EC-CAPTURE-05.json) |

## Borrowed Pointer Punctuation

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-BORROW-01` | Canonical `borrowed T *name` spelling | Accept | [EC-BORROW-01](../tests/conformance/examples/borrowed_pointer_punctuation/EC-BORROW-01.json) |
| `EC-BORROW-02` | Malformed qualifier/star placement | Parser error + canonical spelling fix-it | [EC-BORROW-02](../tests/conformance/examples/borrowed_pointer_punctuation/EC-BORROW-02.json) |
| `EC-BORROW-03` | Store borrowed pointer into static storage | Strict-system error + copy fix-it | [EC-BORROW-03](../tests/conformance/examples/borrowed_pointer_punctuation/EC-BORROW-03.json) |
| `EC-BORROW-04` | Missing `owner_index` on borrowed return | Diagnostic + attribute insertion fix-it | [EC-BORROW-04](../tests/conformance/examples/borrowed_pointer_punctuation/EC-BORROW-04.json) |
| `EC-BORROW-05` | Pass borrowed pointer into escaping closure context | Strict-system error + ownership-copy fix-it | [EC-BORROW-05](../tests/conformance/examples/borrowed_pointer_punctuation/EC-BORROW-05.json) |

## Cross-feature: `try await` + `?.` + Postfix `?`

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-CROSS-01` | Canonical `try await` + `?.` + postfix `?` | Accept | [EC-CROSS-01](../tests/conformance/examples/cross_feature/EC-CROSS-01.json) |
| `EC-CROSS-02` | Non-canonical `await try` ordering | Warning + reorder fix-it | [EC-CROSS-02](../tests/conformance/examples/cross_feature/EC-CROSS-02.json) |
| `EC-CROSS-03` | Follow-token violation after cross-feature chain | Diagnostic + parenthesize fix-it | [EC-CROSS-03](../tests/conformance/examples/cross_feature/EC-CROSS-03.json) |
| `EC-CROSS-04` | `try? await` plus chained optional propagation | Accept | [EC-CROSS-04](../tests/conformance/examples/cross_feature/EC-CROSS-04.json) |
| `EC-CROSS-05` | Cross-feature expression with `??` fallback | Accept | [EC-CROSS-05](../tests/conformance/examples/cross_feature/EC-CROSS-05.json) |

## Optional Spelling Outcomes (`OPT-SPELL-*`) {#optional-spelling-outcomes-opt-spell-}

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-OPTSP-01` | Future mode uses canonical `Optional<int>` | Accept | [EC-OPTSP-01](../tests/conformance/examples/optional_spelling/EC-OPTSP-01.json) |
| `EC-OPTSP-02` | Future compatibility mode parses `optional<int>` in Core migration mode | Accept with `OPT-SPELL-NONCANON` warning and one-step fix-it to `Optional<int>`. | [EC-OPTSP-02](../tests/conformance/examples/optional_spelling/EC-OPTSP-02.json) |
| `EC-OPTSP-03` | Future canonical-only mode sees `optional<int>` | Reject with `OPT-SPELL-NONCANON-UNSUPPORTED` error and required fix-it. | [EC-OPTSP-03](../tests/conformance/examples/optional_spelling/EC-OPTSP-03.json) |
| `EC-OPTSP-04` | v1 mode sees `Optional<int>` or `optional<int>` | Reject with `OPT-SPELL-RESERVED-V1` (reserved-for-future wording; no mandatory fix-it). | [EC-OPTSP-04](../tests/conformance/examples/optional_spelling/EC-OPTSP-04.json) |
| `EC-OPTSP-05` | Nested noncanonical forms (`optional<optional<int>>`) in compatibility mode outside migration | Reject with per-occurrence `OPT-SPELL-NONCANON` diagnostics as errors and per-occurrence fix-its. | [EC-OPTSP-05](../tests/conformance/examples/optional_spelling/EC-OPTSP-05.json) |
| `EC-OPTSP-06` | Noncanonical spelling in non-rewritable macro expansion | Emit owning spelling diagnostic plus `OPT-SPELL-NOFIX-MACRO` note; no invalid rewrite edit. | [EC-OPTSP-06](../tests/conformance/examples/optional_spelling/EC-OPTSP-06.json) |
| `EC-OPTSP-07` | Interface/module emission after noncanonical source input | Accept with canonicalized round-trip output (`Optional<...>` only). | [EC-OPTSP-07](../tests/conformance/examples/optional_spelling/EC-OPTSP-07.json) |

## Mangling Invariants Outcomes

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-MANGLE-01` | Same declaration with same policy ID across repeated runs | Accept deterministically | [EC-MANGLE-01](../tests/conformance/examples/mangling_invariants/EC-MANGLE-01.json) |
| `EC-MANGLE-02` | Distinct normalized signatures for same base name remain distinct | Accept | [EC-MANGLE-02](../tests/conformance/examples/mangling_invariants/EC-MANGLE-02.json) |
| `EC-MANGLE-03` | Collision vector pair forces same symbol under buggy mangler | Reject (`MANGLE-INV-COLLISION`) | [EC-MANGLE-03](../tests/conformance/examples/mangling_invariants/EC-MANGLE-03.json) |
| `EC-MANGLE-04` | Equivalent constraint spellings normalize to one semantic signature | Accept | [EC-MANGLE-04](../tests/conformance/examples/mangling_invariants/EC-MANGLE-04.json) |
| `EC-MANGLE-05` | Cross-policy comparison is semantic, not raw symbol string | Accept | [EC-MANGLE-05](../tests/conformance/examples/mangling_invariants/EC-MANGLE-05.json) |
| `EC-MANGLE-06` | Harness requests symbol-equality across different policy IDs | Reject (`MANGLE-INV-POLICY-SCOPE`) | [EC-MANGLE-06](../tests/conformance/examples/mangling_invariants/EC-MANGLE-06.json) |
| `EC-MANGLE-07` | Import payload has unknown required `mangling_policy_id` | Hard reject (`MANGLE-INV-UNKNOWN-POLICY`) | [EC-MANGLE-07](../tests/conformance/examples/mangling_invariants/EC-MANGLE-07.json) |
| `EC-MANGLE-08` | Metadata/interface omits semantic signature payload | Hard reject (`MANGLE-INV-MISSING-PAYLOAD`) | [EC-MANGLE-08](../tests/conformance/examples/mangling_invariants/EC-MANGLE-08.json) |

## Reification Outcomes

| ID | Edge case | Expected result | Conformance artifact |
| --- | --- | --- | --- |
| `EC-REIFY-01` | `@reify_generics` on generic declaration with both gates enabled | Accept (`explicit_reified`) | [EC-REIFY-01](../tests/conformance/examples/reification_outcomes/EC-REIFY-01.json) |
| `EC-REIFY-02` | Unmarked generic declaration with both gates enabled | Accept (`erased_default`) | [EC-REIFY-02](../tests/conformance/examples/reification_outcomes/EC-REIFY-02.json) |
| `EC-REIFY-03` | Marker used when reification gate is off (`generic=1,reify=0`) | Reject (`REIFY-02`) | [EC-REIFY-03](../tests/conformance/examples/reification_outcomes/EC-REIFY-03.json) |
| `EC-REIFY-04` | Marker used when generic declarations are deferred (`generic=0`) | Reject (`REIFY-01`) | [EC-REIFY-04](../tests/conformance/examples/reification_outcomes/EC-REIFY-04.json) |
| `EC-REIFY-05` | Marker applied to non-generic declaration | Reject (`REIFY-03`) | [EC-REIFY-05](../tests/conformance/examples/reification_outcomes/EC-REIFY-05.json) |
| `EC-REIFY-06` | Duplicate marker on one declaration | Reject (`REIFY-04`) | [EC-REIFY-06](../tests/conformance/examples/reification_outcomes/EC-REIFY-06.json) |
| `EC-REIFY-07` | Redeclaration mismatch between marked and unmarked forms | Reject (`REIFY-05`) | [EC-REIFY-07](../tests/conformance/examples/reification_outcomes/EC-REIFY-07.json) |
| `EC-REIFY-08` | Override mismatch in reification status | Reject (`REIFY-05`) | [EC-REIFY-08](../tests/conformance/examples/reification_outcomes/EC-REIFY-08.json) |
| `EC-REIFY-09` | Import payload missing required `reification_status` | Hard reject (`REIFY-06`) | [EC-REIFY-09](../tests/conformance/examples/reification_outcomes/EC-REIFY-09.json) |
| `EC-REIFY-10` | Import payload declares unknown required reification capability | Hard reject (`REIFY-07`) | [EC-REIFY-10](../tests/conformance/examples/reification_outcomes/EC-REIFY-10.json) |
| `EC-REIFY-11` | Module mixes marked and unmarked declarations | Accept | [EC-REIFY-11](../tests/conformance/examples/reification_outcomes/EC-REIFY-11.json) |
| `EC-REIFY-12` | Override status remains consistent across hierarchy | Accept | [EC-REIFY-12](../tests/conformance/examples/reification_outcomes/EC-REIFY-12.json) |
| `EC-REIFY-13` | Imported declaration conflicts with local reification status | Reject (`REIFY-05`) | [EC-REIFY-13](../tests/conformance/examples/reification_outcomes/EC-REIFY-13.json) |
