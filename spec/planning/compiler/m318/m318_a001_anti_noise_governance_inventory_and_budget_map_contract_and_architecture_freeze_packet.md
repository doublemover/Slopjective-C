# M318-A001 Planning Packet

Issue: `#7835`  
Title: `Anti-noise governance inventory and budget map`

## Objective

Freeze the anti-noise governance inventory that later `M318` issues will enforce in
CI, planning hygiene, and maintainer workflows.

## Scope

- Reuse the live command-surface budget from `M314`.
- Reuse the live validation-growth ratchet and exception-policy boundary from `M313`.
- Reuse the live source-hygiene, residue, and authenticity boundaries from `M315`.
- Map each budget family to its current owner and to the `M318` issue that will
  automate or enforce it.

## Frozen budget families

- Public command surface:
  - maximum public entrypoints: `25`
  - current public entrypoints: `17`
  - source: `package.json#objc3cCommandSurface`
- Validation-growth surface:
  - closeout maximums: `558` checkers / `179` readiness runners / `555` pytest wrappers
  - no new growth without exception: `true`
  - source: `M313-A002` + `M313-B005`
- Source-hygiene and residue surface:
  - zero-target residue classes remain removed:
    - `transitional_source_model`
    - `legacy_m248_surface_identifier`
  - quarantined residual classes remain bounded to the `M315-D002` result
  - current native-source milestone token lines: `57`
- Artifact-authenticity surface:
  - explicit synthetic stub `.ll` fixtures: `2`
  - replay `.ll` without frontend header remain a transitional bridge owned by `M315-C003`
- Exception-governance transition surface:
  - current active exception registry is the empty-by-default `M313-B005` registry
  - `M318-A002` owns the durable replacement model

## Validation posture

Static verification is justified because this issue freezes an inventory over already
landed policy surfaces. The checker must validate the budget map against the live
`M313`, `M314`, and `M315` source-of-truth files.

Next issue: `M318-A002`.
