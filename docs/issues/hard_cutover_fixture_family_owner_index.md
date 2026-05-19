# Hard-Cutover Fixture Family Owner Index

This is the human-readable companion to
`tests/conformance/hard_cutover_fixture_family_owner_index.json`. It is local
checked-in branch evidence only; no validation, GitHub edits, push, build,
generator, formatter, lint, npm, cmake, or test command was run while preparing
it.

The index keeps fixture-family ownership separate from issue-number closeout:
canonical behavior, retired-surface rejection, generated provenance, tooling
metadata, and issue payloads each have a bounded role.

| Fixture Family                    | Source                                                               | Issues                             | Boundary                                                                                                                                                        |
| --------------------------------- | -------------------------------------------------------------------- | ---------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Canonical native manifest         | `tests/fixtures/canonical/manifest.json`                             | `#8144`, `#8150`                   | hand-authored canonical positives only                                                                                                                          |
| Phase owner contracts             | `tests/conformance/hard_cutover_behavior_phase_owner_contracts.json` | `#8144`, `#8150`                   | parser, sema, lowering, IR, runtime, and e2e support claims are backed by native fixture families only                                                          |
| Retired surface matrix            | `tests/native/retired_surface_matrix.json`                           | `#8144`, `#8150`                   | retired modes, retired adapters, alternate acceptance paths, runtime-dispatch, and retired-source lanes are rejection, strict-error, or absent-support evidence |
| Parser behavior                   | `tests/native/parser`                                                | `#8132`, `#8134`, `#8146`          | parser positives are canonical; removed compatibility and retired route flags are parser rejections                                                             |
| Sema behavior                     | `tests/native/sema`                                                  | `#8135`, `#8145`, `#8146`, `#8147` | semantic positives are canonical; retired adapter and unsupported-feature claims are diagnostics                                                                |
| Lowering behavior                 | `tests/native/lowering`                                              | `#8136`, `#8137`, `#8147`          | runtime-dispatch retired route lowering is strict-error or removed-flag rejection evidence                                                                      |
| IR behavior                       | `tests/native/ir`                                                    | `#8137`, `#8147`                   | runtime helper/call evidence does not introduce retired route support                                                                                           |
| Runtime behavior                  | `tests/native/runtime`                                               | `#8133`, `#8143`, `#8144`          | object/runtime positives are canonical; dispatch retired route is strict-error behavior                                                                         |
| E2E behavior                      | `tests/native/e2e`                                                   | `#8144`, `#8150`                   | runnable positives are canonical; legacy literals and unknown dispatch are negative execution evidence                                                          |
| Generated boundary                | `tests/fixtures/generated/manifest.json`                             | `#8138`, `#8144`, `#8148`          | generated artifacts are provenance/schema evidence only                                                                                                         |
| Tooling native execution metadata | `tests/tooling/fixtures/native`                                      | `#8143`, `#8144`                   | metadata mirrors positive/negative native behavior and cannot widen support                                                                                     |
| Issue closeout evidence           | `docs/issues`                                                        | `#8132`-`#8150`                    | tracker payloads summarize committed evidence and point back to owner indexes                                                                                   |
| Latest branch owner refresh       | `docs/issues/hard_cutover_latest_local_commit_refresh.md`            | `#8132`-`#8150`                    | docs/issues-only refresh of branch owner commits after `abc203478`; validation, push, GitHub updates, and remote closure remain deferred                        |

Generated fixtures and issue payloads are deliberately not behavior authorities.
They may prove provenance, schema shape, or closeout linkage, but canonical
support still has to be represented by hand-authored behavior fixtures or by
strict rejection/error evidence.
