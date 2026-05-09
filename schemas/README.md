# objc3c Schema Registry

Checked-in schemas are part of the hard-cutover support contract. Docs, spec,
site pages, and report-producing tools should cite these files instead of
restating local JSON shapes in prose.

## Capability Truth Schemas

| Schema | Owns |
| ------ | ---- |
| `objc3c-capability-matrix-v1.schema.json` | Public capability states, support claims, evidence entries, command-surface policy, and hard-cutover rules. |
| `objc3c-capability-evidence-map-v1.schema.json` | Flattened capability-to-evidence rows used by docs and release evidence maps. |

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
- Internal rows may identify implementation owners, schema owners, workflow
  owners, or report owners without claiming public Objective-C 3.0 language
  behavior.
