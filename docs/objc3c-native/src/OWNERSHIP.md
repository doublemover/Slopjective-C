# objc3c-native Source Ownership

This file defines source ownership for generated `docs/objc3c-native.md`.

## Scope

- Source directory: `docs/objc3c-native/src/`
- Generated output: `docs/objc3c-native.md`
- Generator: `python scripts/build_objc3c_native_docs.py`

## Fragment Ownership Matrix

| Fragment | Responsibility | Primary owner | Backup owner |
| --- | --- | --- | --- |
| `10-cli.md` | CLI usage, flags, defaults, invocation examples | compiler/tooling | compiler/docs |
| `20-grammar.md` | Lexer/parser grammar and token surface | compiler/parser | compiler/docs |
| `30-semantics.md` | Semantic contracts and lowering behavior | compiler/semantics | compiler/docs |
| `40-diagnostics.md` | Diagnostic codes and failure semantics | compiler/diagnostics | compiler/semantics |
| `50-artifacts.md` | Artifact paths, runtime output contracts, exit surfaces | compiler/release | compiler/tooling |
| `60-tests.md` | Replay proof and validation command contracts | compiler/qa | compiler/release |
| `README.md` | Fragment taxonomy, stitch contract, author instructions | compiler/docs | compiler/tooling |

## Update Workflow

1. Edit only source fragments in `docs/objc3c-native/src/`.
1. Run `python scripts/build_objc3c_native_docs.py` to regenerate output.
1. Run `python scripts/build_objc3c_native_docs.py --check` and require pass.
1. Commit fragment edits and generated output together when output changed.

## Review Policy

- Manual edits to `docs/objc3c-native.md` are unsupported.
- Changes must be reviewed by the primary owner for each touched fragment.
- Cross-fragment changes require at least one reviewer from `compiler/docs`.
- PR description must include regeneration command and check output status.
