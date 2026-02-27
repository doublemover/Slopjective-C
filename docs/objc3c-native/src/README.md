# objc3c-native Source Fragment Contract (M132-C001)

This directory defines the canonical source fragments for building `docs/objc3c-native.md`.

## Ownership Policy

- Output owner: Compiler Docs maintainers for Objective-C native frontend contracts.
- Source owner: Each fragment owner listed below owns normative content in that fragment.
- `docs/objc3c-native.md` is generated output; edit source fragments, not stitched output.

## Canonical Fragment Taxonomy

| Order | Fragment | Scope | Owner |
| --- | --- | --- | --- |
| 10 | `10-cli.md` | CLI surface, flags, invocation, defaults | compiler/tooling |
| 20 | `20-grammar.md` | Parser/lexer grammar and token contracts | compiler/parser |
| 30 | `30-semantics.md` | Semantic and lowering behavior contracts | compiler/semantics |
| 40 | `40-diagnostics.md` | Diagnostic code and failure-surface contracts | compiler/diagnostics |
| 50 | `50-artifacts.md` | Artifact paths, runtime contracts, determinism output surfaces | compiler/release |
| 60 | `60-tests.md` | Validation commands, replay proofs, and fixture contracts | compiler/qa |

## Deterministic Stitch Order

The stitch order is fixed and must not depend on filesystem iteration order.

1. `10-cli.md`
2. `20-grammar.md`
3. `30-semantics.md`
4. `40-diagnostics.md`
5. `50-artifacts.md`
6. `60-tests.md`

## Include Rules

- Only files listed in the deterministic stitch order are stitched into `docs/objc3c-native.md`.
- `README.md` is metadata only and is never stitched.
- Unknown Markdown files under this directory are contract drift and must fail `--check-contract`.
- Missing listed fragments are tolerated until migration is complete; once present, ordering is still fixed.

## Contract Validation

- Validate contract only: `python scripts/build_objc3c_native_docs.py --check-contract`
- Build when fragments are present: `python scripts/build_objc3c_native_docs.py`
