# objc3c Schema Registry

Checked-in schemas are part of the hard-cutover support contract. Docs, spec,
site pages, and report-producing tools should cite these files instead of
restating local JSON shapes in prose.

Schema identifiers use `https://objc3c.dev/schemas/<file-name>`. Placeholder
domains, product-host aliases, relative IDs, and local-only schema IDs are not
canonical support truth.

## Capability Truth Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3c-capability-matrix-v1.schema.json` | Public capability states, support claims, evidence entries, command-surface policy, projection policy, and hard-cutover rules. |
| `objc3c-capability-evidence-map-v1.schema.json` | Flattened capability-to-evidence rows, public-command evidence policy, and no-command ownership boundaries used by docs and release evidence maps. |

## Conformance Evidence Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3-conformance-dashboard-status-v1.schema.json` | Dashboard-ready conformance status snapshots used by public reporting and release gates. |
| `objc3-conformance-evidence-bundle-v1.schema.json` | Aggregated conformance manifests, profile claims, test evidence, and known-deviation records for release evidence. |
| `objc3-runtime-2025Q4.manifest.schema.json` | Runtime artifact manifests for the `NR-OBJC-RUNTIME` normative reference. |
| `objc3-abi-2025Q4.schema.json` | ABI artifact manifests for the `NR-ABI-PLATFORM` normative reference. |

## Release And Adoption Schemas

These schemas are shared-registry entries owned by
`scripts/objc3c_shared/schema_registry.py`. Docs and runbooks cite the
registry-backed schema files and must not copy their JSON shapes into local
tables or examples.

| Schema | Owns |
| ------ | ---- |
| `objc3c-adoption-legibility-evidence-v1.schema.json` | Adoption, onboarding, comparison, and claim-audit evidence without retired-surface support claims. |
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

The canonical data files are:

- `docs/support/capability_matrix.json`
- `docs/support/evidence_map.json`

The human-readable projections are:

- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`

Schema files under `schemas/` are the only schema source of truth. Support
directory JSON files consume these schemas through
`scripts/objc3c_shared/schema_registry.py`; they must not carry local schema
mirrors or copied schema fragments.

## Hard-Cutover Rules

- Public command evidence must use `npm run objc3c -- <action>`.
- Direct helper commands can be source ownership evidence, but they are not
  user-facing command support.
- `implemented`, `rejected`, `reserved`, and `internal` are the only capability
  states.
- Retired surface terms listed in `docs/support/capability_matrix.json` are
  not alternate states.
- Historical wording may appear only through retired-surface examples; active
  fields use upgrade, adoption, support, and revert terminology.
- Internal rows may identify implementation owners, schema owners, workflow
  owners, or report owners without claiming public Objective-C 3.0 language
  behavior.
