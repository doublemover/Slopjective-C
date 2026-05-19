# Hard-Cutover Acceptance Area Owners

This is the human-readable companion to
`tests/conformance/hard_cutover_acceptance_area_owners.json`. It is checked-in
branch evidence only; no validation, GitHub edits, push, build, or test run was
performed while preparing it.

| Area                                | Issues                    | Owner Evidence                                                                                                                                                                               | Behavior / Retired-Surface Boundary                                                                                   |
| ----------------------------------- | ------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------- |
| Parser, lexer, AST                  | `#8132`, `#8134`, `#8146` | parser contract fingerprints, recovery boundaries, expression/statement nodes, AST scope/signature surfaces, token legacy-literal classifier                                                 | canonical parser positives only; old-mode and retired route flags are parser rejections                               |
| Semantic diagnostics                | `#8135`, `#8145`, `#8147` | diagnostic catalog contract/table, sema contract headers, semantic feature-claim owners, frontend diagnostic-stage contract                                                                  | retired adapter gates and unsupported feature claims are semantic diagnostics                                         |
| Lowering and IR                     | `#8136`, `#8137`, `#8147` | lower control-flow/block contracts, IR control-flow ops, runtime helper calls, runtime metadata emission, surface serialization                                                              | runtime retired route lowering and unresolved runtime calls are strict-error evidence                                 |
| Runtime dispatch and registration   | `#8133`, `#8143`          | dispatch API, method cache, runtime registration, driver runtime registration command surfaces, class graph, selector/keypath tables                                                         | runtime retired route and unknown receiver behavior are strict errors                                                 |
| Frontend public C API runner        | `#8140`, `#8141`          | runner options, commands, sessions, output contracts, public-result mapping, runtime public result headers                                                                                   | strict C API contract evidence, not a compatibility wrapper                                                           |
| Pipeline, artifacts, config, JSON   | `#8138`, `#8148`          | pipeline stage contracts, artifact schema/publication contracts, feature-claim artifacts, contract-id registry, JSON validation/equivalence/pointer/type helpers, config feature-state query | schemas classify unsupported states without retired route support                                                     |
| Workflow and hygiene                | `#8142`, `#8149`          | workflow action catalog, registry views, action integrity, request dispatch, command/validation docs                                                                                         | public command boundary is `npm run objc3c -- <action>`                                                               |
| Canonical behavior fixtures         | `#8144`, `#8150`          | canonical manifest, retired surface matrix, retired-surface absence index, positive-residue audit                                                                                            | retired rows are rejection, strict-error, or absent; positives are canonical only                                     |
| Capability docs and stdlib boundary | `#8145`                   | support matrix/evidence map, README support onboarding, stdlib runbooks and policy surfaces                                                                                                  | docs reject retired adapters, alternate acceptance paths, retired-source lanes, and compatibility-mode support claims |

## Latest Branch Owner Refresh

The latest docs/issues-only refresh for these ownership areas is
`docs/issues/hard_cutover_latest_local_commit_refresh.md`. It folds in the branch
owner wave after `abc203478` through `6d6fa804d` and keeps the same acceptance
boundaries: new owner modules are evidence of canonical ownership, while
retired modes, retired adapters, alternate acceptance paths, and retired-source lane support remain
rejection, strict-error, or absent evidence.
