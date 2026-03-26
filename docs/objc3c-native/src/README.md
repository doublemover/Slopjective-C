# objc3c-native Source Fragment Contract

## Ownership Policy

These fragment files are the canonical source for `docs/objc3c-native.md`.
They describe the live native frontend, not historical milestone closeouts.

## Canonical Fragment Taxonomy

- CLI and driver behavior
- Grammar and parser surface
- Semantic and lowering surface
- Runtime execution architecture
- Diagnostics
- Artifacts and build outputs
- Live validation surface
- Library embedding API

## Deterministic Stitch Order

1. `10-cli.md`
2. `20-grammar.md`
3. `30-semantics.md`
4. `35-runtime-architecture.md`
5. `40-diagnostics.md`
6. `50-artifacts.md`
7. `60-tests.md`
8. `library-api.md`

## Include Rules

- Keep these fragments focused on the current live surface.
- Put historical planning and closeout material under `tmp/archive/`.
- Avoid milestone-coded sections and issue-era command chains here.

## Contract Validation

- Rebuild: `python scripts/build_objc3c_native_docs.py`
- Drift check: `python scripts/build_objc3c_native_docs.py --check`
