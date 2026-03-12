# M264-D002 Expectations

Issue: `#7241`
Packet: `M264-D002`
Milestone: `M264`
Lane: `D`

The native toolchain must expose truthful conformance-claim operations instead of
leaving the emitted sidecars as passive files.

Canonical identifiers:

- contract id: `objc3c-toolchain-conformance-claim-operations/m264-d002-v1`
- schema id: `objc3c-driver-conformance-validation-v1`

Required behavior:

- native CLI accepts `--emit-objc3-conformance`
- native CLI accepts `--emit-objc3-conformance-format <json|yaml>`
- current runnable format is `json`
- `--emit-objc3-conformance-format yaml` fails closed
- native CLI accepts validation-only mode `--validate-objc3-conformance <report.json>`
- validation consumes both `module.objc3-conformance-report.json` and sibling `module.objc3-conformance-publication.json`
- validation writes `module.objc3-conformance-validation.json`
- validation artifact records:
  - report/publication artifact names
  - report/publication contract and schema ids
  - selected/supported/rejected profile ids
  - effective compatibility mode
  - migration-assist state
  - publication surface kind
- selected profile remains `core`
- strict, strict-concurrency, and strict-system remain rejected/fail-closed
- validation does not widen runtime/profile claims beyond the already emitted JSON sidecars
