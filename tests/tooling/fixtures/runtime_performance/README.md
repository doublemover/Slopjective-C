# objc3c Runtime Performance Fixtures

This fixture root defines the checked-in runtime-performance workload surface
for `M294`.

Use these workloads to measure:

- startup registration and replay
- selector lookup and dispatch cache behavior
- realized class/property/protocol reflection
- ARC/current-property/weak/autoreleasepool helper traffic
- runtime-backed ownership and reflection coupling
- stdlib core helper runtime traffic
- stdlib concurrency scheduler/task-group/helper traffic

Authoritative workload inventory:

- `workload_manifest.json`
- `source_surface.json`
- `behavior_owner_splits/index.json`
- `workload_replay_contract.json`
- `metadata_resilience_contract.json`
- `stress_sanitizer_contract.json`

What does not count:

- shared probe copies
- sidecar-only timing claims with no coupled runtime probe
- ad hoc workload substitutions that are not recorded in the manifest
- generated runtime-performance packets, cache/counter snapshots, or runnable
  package artifacts as canonical behavior support without a behavior owner split
