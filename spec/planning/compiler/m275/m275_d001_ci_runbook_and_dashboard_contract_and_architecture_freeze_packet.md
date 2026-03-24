# M275-D001 Packet: CI, runbook, and dashboard contract - Contract and architecture freeze

## Scope

- Freeze the operator-facing advanced-feature references carried by the existing conformance publication and validation sidecars.
- Keep the contract truthful to current implementation status rather than implying new dashboard exports or runnable advanced profiles.

## Surface

- `objc3c-advanced-feature-ci-runbook-dashboard-contract/m275-d001-v1`
- existing sidecars:
  - `module.objc3-conformance-publication.json`
  - `module.objc3-conformance-validation.json`

## Dependencies

- `M264-D001` driver publication contract
- `M264-D002` CLI/toolchain conformance-claim operations
- `M275-C002` feature-aware conformance report emission
- `M275-C003` corpus sharding and release-evidence packaging

## Required proof

- publication sidecars carry the advanced-feature operator contract id plus the `M275-C002` and `M275-C003` dependency contract ids
- publication and validation sidecars carry the same gate script, runbook, dashboard schema, and targeted advanced profile ids
- native CLI publication, native validation, and frontend C API publication all preserve the same operator references
- the issue does not introduce a new report sidecar, new dashboard payload, or promoted runnable advanced profile

## Next issue

- `M275-D002` release evidence gate and dashboard publication implementation - Core feature implementation
