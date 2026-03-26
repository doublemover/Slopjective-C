# M318-A002 Planning Packet

Issue: `#7836`  
Title: `Exception process and ownership model`

## Objective

Create the durable anti-noise exception process so future governance automation has
one registry and one record shape for budget waivers.

## Scope

- Replace the cleanup-only exception process from `M313-B005` with a governance-owned
  model under `spec/governance/`.
- Keep the registry empty by default.
- Publish package metadata that points to the governance budget map and registry.

## Frozen process rules

- Every active exception must include:
  - stable id
  - budget family
  - surface family
  - owner issue
  - created-by issue
  - rationale
  - approval issue
  - expiry issue
  - review issue
  - rollback condition
  - replacement target
  - status
- Allowed statuses:
  - `active`
  - `expired`
  - `retired`
  - `rejected`
- Expired exceptions are not silent debt; later `M318` enforcement must fail closed on
  active expired records.
- The durable registry starts at `0` active exceptions.

## Source-complete outputs

- `spec/governance/objc3c_anti_noise_exception_process.json`
- `spec/governance/objc3c_anti_noise_exception_registry.json`
- `package.json#objc3cGovernance`

## Validation posture

Static verification is justified because this issue defines a source-complete process
and registry shape, not live CI behavior. The checker must validate the process file,
registry file, package metadata, and alignment with `M318-A001`.

Next issue: `M318-B001`.
