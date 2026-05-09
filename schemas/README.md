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
| `objc3c-capability-matrix-v1.schema.json` | Public capability states, support claims, evidence entries, command-surface policy, and hard-cutover rules. |
| `objc3c-capability-evidence-map-v1.schema.json` | Flattened capability-to-evidence rows used by docs and release evidence maps. |

## Release And Adoption Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3c-adoption-legibility-evidence-v1.schema.json` | Adoption, onboarding, comparison, and claim-audit evidence without migration-lane support claims. |
| `objc3c-long-horizon-operations-evidence-v1.schema.json` | Support-window, upgrade replay, revert-readiness, soak, and aging-regression evidence. |
| `objc3c-upgrade-support-report-v1.schema.json` | Upgrade support reporting, support windows, warnings, and revert guidance. |
| `objc3c-update-manifest-v1.schema.json` | Release-channel update metadata linked to the upgrade support report. |
| `objc3c-package-lock-v1.schema.json` | Package lock provenance with npm-bridge replay commands. |
| `objc3c-package-offline-mirror-index-v1.schema.json` | Offline package mirror metadata with npm-bridge replay commands. |
| `objc3c-package-install-receipt-v1.schema.json` | Package install receipts with npm-bridge install commands and canonical bootstrap entrypoints. |
| `objc3c-platform-support-matrix-v1.schema.json` | Platform publication surfaces constrained to npm-bridge commands. |
| `objc3c-compiler-throughput-summary-v1.schema.json` | Compiler throughput summaries keyed to the objc3c bridge. |

The canonical data files are:

- `docs/support/capability_matrix.json`
- `docs/support/evidence_map.json`

The human-readable projections are:

- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`

## Hard-Cutover Rules

- Public command evidence must use `npm run objc3c -- <action>`.
- Direct helper commands can be source ownership evidence, but they are not
  user-facing command support.
- `implemented`, `rejected`, `reserved`, and `internal` are the only capability
  states.
- Shims, fallback paths, migration lanes, legacy modes, old modes, and
  prose-only support claims are not alternate states.
- Historical schema words such as compatibility mode, migration lane, or
  rollback may appear only as retired-surface examples; active fields use
  upgrade, adoption, support, and revert terminology.
- Internal rows may identify implementation owners, schema owners, workflow
  owners, or report owners without claiming public Objective-C 3.0 language
  behavior.
