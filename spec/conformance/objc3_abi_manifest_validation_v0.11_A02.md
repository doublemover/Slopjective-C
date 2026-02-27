# Objective-C 3 ABI Manifest Validation Guidance (v0.11-A02)

Scope: issue #116. This guidance applies to manifests with `manifest_schema = "objc3-abi-2025Q4"` validated by `schemas/objc3-abi-2025Q4.schema.json`.

## Field-level constraints

| Field | Constraint |
| --- | --- |
| `manifest_type` | Required constant: `objc3-abi-manifest`. |
| `manifest_schema` | Required constant: `objc3-abi-2025Q4`. |
| `manifest_version` | Required SemVer string (`major.minor.patch`). |
| `manifest_revision` | Optional integer, `>= 0`. |
| `issue_ref` | Required constant reference: `v0.11-A02/#116`. |
| `generated_at` | Required RFC 3339 date-time string. |
| `producer.toolchain_id` | Required non-empty string. |
| `producer.toolchain_version` | Required version string (`x.y.z` with optional suffix). |
| `producer.build_id` | Required non-empty string. |
| `target_triples[]` | Required non-empty unique list; each entry must be a target triple-shaped token (`arch-vendor-os[-abi]`). |
| `symbol_policy_id` | Required policy ID (`objc3.<name>.vN`). |
| `mangling_policy_id` | Required policy ID (`objc3.<name>.vN`). |
| `metadata_compatibility.producer_schema_major` | Required integer `>= 1`. |
| `metadata_compatibility.producer_schema_minor` | Required integer `>= 0`. |
| `metadata_compatibility.minimum_importer_schema_major` | Required integer `>= 1`; in `strict` mode, this is pinned to `1` in this schema line. |
| `metadata_compatibility.minimum_importer_schema_minor` | Required integer `>= 0`. |
| `metadata_compatibility.compatibility_mode` | Required enum: `strict` or `forward-compatible`. |
| `metadata_compatibility.required_capabilities[]` | Required unique list of capability IDs (`objc3.<name>.vN`). |
| `artifacts[]` | Required non-empty list. |
| `artifacts[].kind` | Required enum: `module-metadata`, `symbol-table`, `abi-diff`, or `binary-interface-summary`. |
| `artifacts[].sha256` | Required lowercase 64-hex digest. |
| Unknown top-level fields | Rejected, except under optional `extensions` object. |

## Versioning expectations

1. `manifest_schema` is quarter-scoped and immutable for this line (`objc3-abi-2025Q4`).
2. Use `manifest_version` major bumps for incompatible schema or semantic interpretation changes.
3. Use `manifest_version` minor/patch bumps for backward-compatible additions, clarifications, or payload corrections.
4. Importers must treat unknown required capabilities as incompatibilities and fail validation.
5. Importers should reject unsupported producer major schema values before consuming ABI metadata.
6. `compatibility_mode = "strict"` should be interpreted as exact-policy operation: no implicit fallback behavior for missing required metadata.

## Validation command

Run from repository root:

```bash
npx --yes ajv-cli validate --spec=draft2020 \
  -s schemas/objc3-abi-2025Q4.schema.json \
  -d reports/conformance/manifests/objc3-abi-2025Q4.example.json
```
