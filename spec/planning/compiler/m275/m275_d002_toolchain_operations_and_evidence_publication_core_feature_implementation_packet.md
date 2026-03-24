# M275-D002 Packet: Toolchain operations and evidence publication - Core feature implementation

## Scope

- Publish real native validation-time sidecars for advanced-feature release-evidence operations and dashboard-ready status.
- Keep the implementation bounded to emitted operator artifacts instead of claiming that the compiler owns CI execution itself.

## Surface

- `module.objc3-release-evidence-operation.json`
- `module.objc3-dashboard-status.json`

## Dependencies

- `M275-D001` CI/runbook/dashboard operator contract
- `M275-C003` corpus sharding and release-evidence packaging
- existing `M264-D002` validation path

## Required proof

- native validation writes both new sidecars deterministically
- the release-evidence operation sidecar publishes gate command tokens, targeted profile ids, shard ids, and release-evidence artifact ids
- the dashboard sidecar publishes dashboard schema path plus current profile-status truth
- `core` remains the only pass profile while advanced profiles remain blocked release-evidence targets

## Next issue

- `M275-E001` integrated advanced-feature conformance gate - Contract and architecture freeze
