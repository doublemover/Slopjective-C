# objc3c Schema Registry

Checked-in schemas are part of the hard-cutover support boundary. Docs, spec,
site pages, and evidence-producing tools should cite these files instead of
restating JSON shapes in prose.

Schema identifiers use `https://objc3c.dev/schemas/<file-name>`. Placeholder
domains, product-host aliases, relative IDs, and checkout-scoped schema IDs are
not canonical schema ownership.

## Capability Boundary Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3c-capability-matrix-v1.schema.json` | Public capability states, support claims, evidence entries, command-surface policy, projection policy, responsibility rules, and hard-cutover rules. |
| `objc3c-capability-evidence-map-v1.schema.json` | Flattened capability-to-evidence rows, public-command evidence policy, and no-command ownership boundaries used by docs and release evidence maps. |
| `objc3c-umbrella-readiness-v1.schema.json` | Promotion gates, blockers, source/fixture/doc prerequisites, generated-output boundaries, and final criteria for broad reserved umbrella rows. |

`docs/support/capability_matrix.schema.json` is a support-directory entrypoint
for docs and editor tooling. It delegates to
`schemas/objc3c-capability-matrix-v1.schema.json` with `$ref`; it has no
independent `$id` and is not a separate schema owner, registry ID, or copied
capability-matrix shape.

## Conformance Evidence Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3-conformance-dashboard-status-v1.schema.json` | Dashboard-ready conformance status snapshots used by public reporting and release gates. |
| `objc3-conformance-evidence-bundle-v1.schema.json` | Aggregated conformance manifests, profile claims, test evidence, and known-deviation records for release evidence. |
| `objc3c-tooling-release-evidence-operation-v1.schema.json` | Release-evidence operation sidecars that connect validation, dashboard publication, runbooks, and checklist/schema references. |
| `objc3c-tooling-integrated-advanced-feature-gate-v1.schema.json` | Integrated advanced-feature gate sidecars over report, publication, validation, release-evidence, and dashboard artifacts. |
| `objc3c-tooling-release-candidate-execution-matrix-v1.schema.json` | Release-candidate execution matrix sidecars for final conformance publication bundle readiness. |
| `objc3-runtime-2025Q4.manifest.schema.json` | Runtime artifact manifests for the `NR-OBJC-RUNTIME` normative reference. |
| `objc3-abi-2025Q4.schema.json` | ABI artifact manifests for the `NR-ABI-PLATFORM` normative reference. |

## Release And Adoption Schemas

These schemas are shared-registry entries owned by
`scripts/objc3c_shared/schema_registry.py`. Docs and runbooks cite the
registry-backed schema files and must not copy their JSON shapes into local
tables or examples.

| Schema | Owns |
| ------ | ---- |
| `objc3c-adoption-legibility-evidence-v1.schema.json` | Adoption, onboarding, comparison, current-support replay, and claim-audit evidence without retired-surface support claims. |
| `objc3c-long-horizon-operations-evidence-v1.schema.json` | Support-window, upgrade replay, revert-readiness, soak, and aging-regression evidence. |
| `objc3c-upgrade-support-report-v1.schema.json` | Upgrade support reporting, support windows, warnings, and revert guidance. |
| `objc3c-update-manifest-v1.schema.json` | Release-channel update metadata linked to the upgrade support report. |
| `objc3c-package-lock-v1.schema.json` | Package lock provenance with npm-bridge replay commands. |
| `objc3c-package-offline-mirror-index-v1.schema.json` | Offline package mirror metadata with npm-bridge replay commands. |
| `objc3c-package-hosted-registry-service-v1.schema.json` | Hermetic hosted-registry service contracts for fixture auth, trust-root operation, revocation, moderation, availability, and no-network fail-closed behavior. |
| `objc3c-package-install-receipt-v1.schema.json` | Package install receipts with npm-bridge install commands, canonical bootstrap entrypoints, and reserved sanitizer package-variant/runtime-library receipt fields. |
| `objc3c-sanitizer-runtime-library-manifest-v1.schema.json` | Dedicated ASan/UBSan runtime-library manifests with exact Windows x64 runtime artifact sets, required digests, missing-runtime fail-closed behavior, and no support/native-execution promotion. |
| `objc3c-sanitizer-runtime-promotion-evidence-v1.schema.json` | Source-owned ASan/UBSan promotion contracts that require durable fixture rows, regenerated package/probe evidence, packaged execution smoke, expected sanitizer detection, digest checks, and fail-closed negative cases before any sanitizer support promotion. |
| `objc3c-package-install-distribution-receipt-v1.schema.json` | From-nothing package install distribution receipts with clean owned roots and explicit replay commands. |
| `objc3c-package-install-distribution-operation-receipt-v1.schema.json` | Package install distribution update/uninstall plan receipts bound to clean local install evidence. |
| `objc3c-platform-toolchain-support-evidence-v1.schema.json` | Source-owned host/toolchain/package/sanitizer support evidence, fail-closed unsupported rows, and reserved sanitizer package metadata. |
| `objc3c-platform-hosted-runner-capability-summaries-v1.schema.json` | Checked hosted-runner platform summaries that explain Windows support, Linux/macOS rejection, sanitizer reservation, and LLVM fail-closed states without promoting summary-only evidence. |
| `objc3c-platform-support-source-truth-v1.schema.json` | Checked source-truth projection that keeps platform rows, package variants, and sanitizer variants aligned with platform-hardening evidence. |
| `objc3c-platform-support-matrix-v1.schema.json` | Platform publication surfaces constrained to npm-bridge commands. |
| `objc3c-compiler-throughput-summary-v1.schema.json` | Compiler throughput summaries keyed to the objc3c bridge. |
| `objc3c-performance-telemetry-v1.schema.json` | Performance telemetry samples with npm-bridge command strings. |
| `objc3c-optimization-runtime-debug-safety-v1.schema.json` | Optimization runtime/debug safety governance for checked budget records, deoptimization/invalidation boundaries, debug source-map preservation, and fail-closed overclaim cases. |
| `objc3c-debug-source-maps-v1.schema.json` | Debug source-map bundles with compiler-owned source spans, debug maps, native line-table rows, native debug-info evidence, and inline-frame preservation records. |
| `objc3c-debugger-integration-replay-v1.schema.json` | Replayable LLDB protocol fixtures, checked stepping plans, value-inspection records, supported runtime metadata, and fail-closed debugger negative cases. |
| `objc3c-typed-keypath-debugger-lowering-v1.schema.json` | Typed keypath descriptor debugger metadata, nested component owner/member/type identity paths, source-map and diagnostic anchors, and no-fallback lowering policy. |
| `objc3c-generic-callable-model-v1.schema.json` | Generic free-function and Objective-C generic-method metadata identity, reification policy, mangling policy, selector interaction, and fail-closed negative case records. |
| `objc3c-full-envelope-dashboard-summary-v1.schema.json` | Full-envelope claimability dashboard summaries over support, conformance, release, performance, and trust evidence. |
| `objc3c-developer-tooling-editor-surface-v1.schema.json` | Combined editor tooling surface summaries for diagnostics, navigation, formatting, debug, and unpublished capability metadata. |
| `objc3c-application-architecture-evidence-summary-v1.schema.json` | Application architecture and testing evidence summaries for canonical workspace/template artifacts. |
| `objc3c-artifact-authenticity-v1.schema.json` | Artifact authenticity envelopes that classify generated outputs, fixtures, and archive references without support-claim inflation. |
| `source-hygiene-hard-cutover-report-v1.schema.json` | Source-hygiene hard-cutover reports for retired-surface residue, tracked generated-output rows, and active rejection findings. |

## Language Feature Contract Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3c-typed-throws-effect-contract-v1.schema.json` | Typed throws source effect identity, exact callable compatibility policy, catch compatibility records, bridge-to-`id<Error>` policy, unsupported foreign-carrier fail-closed records, interface anchors, hidden error-out ABI lowering readiness, and negative rejection records. |
| `objc3c-value-optionals-contract-v1.schema.json` | Value optional source/interface semantic contracts, ABI-layout identity, presence/payload records, runtime/lowering fail-closed boundaries, and rejection records. |

## Workflow Registry Schemas

The workflow registry schemas live beside the workflow implementation under
`scripts/objc3c_workflow/schemas/`, but their `$id` values still use the
canonical `https://objc3c.dev/schemas/<file-name>` namespace. They are schema
truth for the npm bridge and registry payloads, not local-host aliases.

| Schema | Owns |
| ------ | ---- |
| `action-registry-v1.schema.json` | Public workflow action registry payloads and capability-boundary fields exposed by `npm run objc3c -- --list-json`. |
| `schema-index-v1.schema.json` | Machine-readable workflow schema index, registry owner surfaces, and capability boundary schema IDs. |
| `workflow-report-v1.schema.json` | Public workflow report shape emitted under the public-workflow generated-output family. |

The canonical data files are:

- `docs/support/capability_matrix.json`
- `docs/support/evidence_map.json`
- `docs/support/umbrella_readiness.json`

The support-directory schema entrypoint is:

- `docs/support/capability_matrix.schema.json`

The human-readable projections are:

- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`
- `docs/support/umbrella_readiness.md`
- `docs/support/capability_claim_responsibility.md`

Schema files under `schemas/` are the schema owner inputs. Support
directory JSON files consume these schemas through
`scripts/objc3c_shared/schema_registry.py` and
`native/objc3c/src/artifacts/json/capability_support_schema_records.cpp`; they
must not carry local schema mirrors or copied schema fragments. The
support-directory schema entrypoint may only delegate to the canonical schema.

## Hard-Cutover Rules

- Public command evidence must use `npm run objc3c -- <action>`.
- Direct helper commands can be source ownership evidence, but they are not
  user-facing command support.
- `implemented`, `rejected`, `reserved`, and `internal` are the only capability
  states.
- Retired surface terms listed in `docs/support/capability_matrix.json` are
  not alternate states.
- Historical wording may appear only through retired-surface examples or
  explicitly owned evidence-section identifiers. Active prose, support rows,
  and capability claims use upgrade, adoption, support, replay, and revert
  terminology.
- Workflow registry schemas must use canonical `objc3c.dev` schema IDs; local
  host or product-host aliases are not schema ownership.
- Internal rows may identify implementation owners, schema owners, workflow
  owners, or evidence-output owners without claiming public Objective-C 3.0 language
  behavior.
