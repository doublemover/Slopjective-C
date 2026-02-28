# generated content maintenance playbook

This playbook defines ownership, update workflow, and review rules for generated docs/site outputs.

## ownership map

### objc3c-native docs ownership

| Source | Generated output | Owner group | Required check |
| --- | --- | --- | --- |
| `docs/objc3c-native/src/10-cli.md` | `docs/objc3c-native.md` | compiler/tooling | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/20-grammar.md` | `docs/objc3c-native.md` | compiler/parser | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/30-semantics.md` | `docs/objc3c-native.md` | compiler/semantics | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/40-diagnostics.md` | `docs/objc3c-native.md` | compiler/diagnostics | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/50-artifacts.md` | `docs/objc3c-native.md` | compiler/release | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/60-tests.md` | `docs/objc3c-native.md` | compiler/qa | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/README.md` | `docs/objc3c-native.md` | compiler/docs | `python scripts/build_objc3c_native_docs.py --check` |
| `docs/objc3c-native/src/OWNERSHIP.md` | process metadata | compiler/docs | `python scripts/build_objc3c_native_docs.py --check` |

### site index ownership

| Source | Generated output | Owner group | Required check |
| --- | --- | --- | --- |
| `site/src/index.contract.json` | `site/index.md` | compiler/docs | `python scripts/build_site_index.py --check` |
| `site/src/README.md` | policy metadata | compiler/docs | `python scripts/build_site_index.py --check` |
| `site/src/OWNERSHIP.md` | process metadata | compiler/docs | `python scripts/build_site_index.py --check` |
| `spec/TABLE_OF_CONTENTS.md` | `site/index.md` order surface | spec/maintainers | `python scripts/build_site_index.py --check` |
| `spec/*.md` listed in TOC | `site/index.md` content surface | spec/maintainers | `python scripts/build_site_index.py --check` |

## update workflow

1. Edit source files only (`docs/objc3c-native/src/*`, `site/src/*`, and canonical `spec/*` inputs).
1. Regenerate outputs:
   - `python scripts/build_objc3c_native_docs.py`
   - `python scripts/build_site_index.py`
1. Validate drift checks:
   - `python scripts/build_objc3c_native_docs.py --check`
   - `python scripts/build_site_index.py --check`
1. Commit source changes with regenerated outputs in the same change set.
1. In CI, require generated-only checks to pass before merge.

## regenerate failure guidance

- If `docs/objc3c-native.md` drift fails, run `python scripts/build_objc3c_native_docs.py`.
- If `site/index.md` drift fails, run `python scripts/build_site_index.py`.
- If contract validation fails, restore canonical source layout and rerun checks.

## review policy

- Direct manual edits to generated outputs are not accepted.
- Reviewer assignment must include the owner group for each touched source area.
- Cross-area changes (docs + site + spec) require at least one `compiler/docs` reviewer.
- PR text must include exact regenerate and check commands run.
