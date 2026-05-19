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
| `objc3c-package-install-receipt-v1.schema.json` | Package install receipts with npm-bridge install commands and canonical bootstrap entrypoints. |
| `objc3c-platform-support-matrix-v1.schema.json` | Platform publication surfaces constrained to npm-bridge commands. |
| `objc3c-compiler-throughput-summary-v1.schema.json` | Compiler throughput summaries keyed to the objc3c bridge. |
| `objc3c-performance-telemetry-v1.schema.json` | Performance telemetry samples with npm-bridge command strings. |
| `objc3c-full-envelope-dashboard-summary-v1.schema.json` | Full-envelope claimability dashboard summaries over support, conformance, release, performance, and trust evidence. |
| `objc3c-developer-tooling-editor-surface-v1.schema.json` | Combined editor tooling surface summaries for diagnostics, navigation, formatting, debug, and unpublished capability metadata. |
| `objc3c-application-architecture-evidence-summary-v1.schema.json` | Application architecture and testing evidence summaries for canonical workspace/template artifacts. |
| `objc3c-artifact-authenticity-v1.schema.json` | Artifact authenticity envelopes that classify generated outputs, fixtures, and archive references without support-claim inflation. |
| `source-hygiene-hard-cutover-report-v1.schema.json` | Source-hygiene hard-cutover reports for retired-surface residue, tracked generated-output rows, and active rejection findings. |

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

The support-directory schema entrypoint is:

- `docs/support/capability_matrix.schema.json`

The human-readable projections are:

- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`
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
