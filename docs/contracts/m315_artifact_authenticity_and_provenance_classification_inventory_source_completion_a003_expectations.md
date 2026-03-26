# M315-A003 Expectations

Contract ID: `objc3c-cleanup-artifact-authenticity-inventory/m315-a003-v1`

`M315-A003` freezes the tracked artifact-authenticity candidate set that later `M315`
policy and implementation work must clean up. The scope intentionally covers tracked
`.ll` plus authenticity-sensitive tracked `.json` surfaces while excluding
configuration-only JSON such as `package.json`, `package-lock.json`, and
`.prettierrc.json`.

The inventory must:
- distinguish synthetic fixtures, sample/example artifacts, generated/report artifacts,
  replay candidates missing embedded provenance, and schema/policy contract JSON;
- record provenance signals as they exist today rather than pretending they are
  stronger than they are;
- identify concrete replay and report hotspots that require later provenance or
  labeling work.

The frozen baseline for this issue is:
- `2514` tracked artifact candidates total;
- `78` tracked `.ll` files and `2436` authenticity-sensitive tracked `.json` files;
- `152` replay candidates missing embedded provenance;
- `46` replay `.ll` files without even the `objc3c native frontend IR` header;
- `2` explicit synthetic stub `.ll` parity fixtures;
- `17` tracked report artifacts without embedded generation metadata;
- `59` sample/example artifacts without embedded generation metadata.

The inventory must explicitly hand off:
- stable feature-surface identifiers and annotation naming to `M315-B001`;
- product-code provenance rules to `M315-B002`;
- replay artifact regeneration and provenance capture to `M315-C003`;
- synthetic fixture fencing and relocation to `M315-C004`.
