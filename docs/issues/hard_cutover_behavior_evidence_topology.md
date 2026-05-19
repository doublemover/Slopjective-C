# Hard-Cutover Behavior Evidence Topology

This local index explains the behavior-first organization used to close
`#8132`-`#8150`. It mirrors
`tests/conformance/hard_cutover_behavior_evidence_topology.json`.

No scripts, tests, builds, lints, formatters, generators, npm, CMake, GitHub, or
push operations were run while preparing this topology.

| Phase              | Canonical Positive Evidence                                      | Strict / Rejection Evidence                                                                | Owner Area                       |
| ------------------ | ---------------------------------------------------------------- | ------------------------------------------------------------------------------------------ | -------------------------------- |
| parser             | `tests/native/parser/positive/*`                                 | legacy literal aliases, removed compatibility-mode flag, removed parser-retired-route flag | `parser_lexer_ast`               |
| sema               | typed flow, ownership, ObjC interface, control-flow fixtures     | retired adapter gates, unsupported ARC ownership, throws feature claims                    | `semantic_diagnostics`           |
| lowering           | nil receiver elision, statement lowering, ownership lowering     | runtime dispatch link errors, removed runtime retired route flag                           | `lowering_and_ir`                |
| IR                 | module/function/metadata emission and nil runtime-call contracts | non-nil runtime call unresolved symbol contract                                            | `lowering_and_ir`                |
| runtime            | object model, storage, ARC, blocks, concurrency contracts        | unknown receiver dispatch and unresolved runtime symbols                                   | `runtime_dispatch_registration`  |
| e2e                | smoke and feature-matrix fixtures                                | legacy literal and runtime dispatch strict-error execution fixtures                        | `canonical_behavior_fixtures`    |
| generated boundary | none                                                             | generated manifests, tooling catalogs, issue closeout maps                                 | `pipeline_artifacts_config_json` |

Hard-cutover rule: generated artifacts and issue maps are evidence surfaces, not
behavior definitions. Positive behavior remains in hand-authored canonical
fixtures, while retired modes, retired adapters, alternate acceptance paths,
and retired-source lanes are represented only as rejection, strict-error, or
absent support.
