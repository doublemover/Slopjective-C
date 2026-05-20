# Lowering and ABI Bucket

Minimum scope:

- `throws` ABI lowering stability,
- `async`/executor hop lowering behavior,
- cancellation unwind ordering and cleanup semantics,
- async-to-C/exported thunk ABI behavior (profile/platform where applicable).

## ARC preservation fixture set (issue #67)

- `ARC-67-01.json`: positive ARC semantic-preservation checks across lowering
  modes.
- `ARC-67-02.json`: ownership-hazard negative checks with stable diagnostics.
- `ARP-68-RT-01.json`, `ARP-68-RT-02.json`: suspension-point autorelease pool
  creation/drain traces across async slice boundaries.
- `DEF-71-RT-01.json`, `DEF-71-RT-02.json`: `defer` normal/unwind execution
  order and exactly-once checks.
- `THR-76-ABI-01.json`, `THR-76-ABI-02.json`: `throws` ABI field stability
  checks across optimization levels.
- `CORO-01.json`..`CORO-03.json`: coroutine-based `async` lowering and ABI
  stability checks.
- `CAN-05.json`..`CAN-07.json`: cancellation propagation/task-context lowering
  and non-throwing cancellation status checks.
- `EXE-04.json`: executor-hop lowering metadata preservation checks.
- `ACT-05.json`..`ACT-07.json`: actor isolation boundary-lowering checks and
  actor metadata validation behavior.
- `RES-04.json`, `RES-05.json`, `AGR-RT-01.json`..`AGR-RT-03.json`: resource
  cleanup runtime ordering and aggregate cleanup contracts.
- `LIFE-04.json`, `LIFE-05.json`: `withLifetime`/`keepAlive` ARC interaction
  across scope and suspension boundaries.
- `INT-C-07.json`..`INT-C-12.json`: C/ObjC runtime interop ABI contracts for
  async thunk status/error invariants, ownership boundaries, and
  separate-compilation stability.
- `INT-CXX-05.json`..`INT-CXX-08.json`: ObjC++ lowering parity fixtures for
  ownership boundaries, throws mapping, and async thunk ABI shape consistency.
- `M26-C001.json`..`M26-C010.json`: M26 Lane C lowering scope-freeze fixtures
  mapped to issues `#2856`..`#2865`.
- `M145-D001.json`: M145 Lane D direct LLVM object-emission fail-closed matrix
  fixture covering deterministic `llvm-direct` backend failure diagnostics
  (`O3E001`, `O3E002`) and integration coverage for issue `#4317`.
- `M177-D001.json`: M177 Lane D namespace collision/shadowing lowering replay
  fixture covering deterministic replay-key/profile/metadata parity anchors and
  integration coverage for issue `#4477`.
- `M178-D001.json`: M178 Lane D public/private API partition lowering replay
  fixture covering deterministic replay-key/profile/metadata parity anchors and
  integration coverage for issue `#4482`.
- `M179-D001.json`: M179 Lane D incremental module cache/invalidation lowering
  replay fixture covering deterministic replay-key/profile/metadata parity
  anchors and integration coverage for issue `#4487`.
- `M180-D001.json`: M180 Lane D cross-module conformance lowering replay
  fixture covering deterministic replay-key/profile/metadata parity anchors and
  integration coverage for issue `#4492`.
- `M181-D001.json`: M181 Lane D throws-propagation lowering replay fixture
  covering deterministic replay-key/profile/metadata parity anchors and
  integration coverage for issue `#4497`.
- `M182-D001.json`: M182 Lane D result-like lowering replay fixture covering
  deterministic replay-key/profile/metadata parity anchors and integration
  coverage for issue `#4502`.
- `M183-D001.json`: M183 Lane D NSError-bridging lowering replay fixture
  covering deterministic replay-key/profile/metadata parity anchors and
  integration coverage for issue `#4507`.
- `OBJIR-8016-01.json`, `OBJIR-8016-02.json`: object-model IR lowering
  closure fixtures for issue `#8016`, covering inherited ivar offset
  serialization, replay-key descriptor payloads, runtime layout validation, and
  cyclic-inheritance fail-closed diagnostics.
- `RTBACK-8017-01.json`, `RTBACK-8017-02.json`: runtime-backed semantics
  closure fixtures for issue `#8017`, covering block promotion/copy-dispose,
  ARC helper automation, error runtime bridges, continuation/task/actor helper
  calls, runtime-import helper surfaces, and fail-closed diagnostics for
  unsupported escape, throw, task-group, and actor-hop edges.
- `TRUTH-8018-01.json`, `TRUTH-8018-02.json`: manifest/object/IR truth-gate
  fixtures for issue `#8018`, covering deterministic manifest, IR, object,
  registration, runtime metadata, discovery/linker, conformance sidecars, and
  fail-closed diagnostics when semantic layout drift blocks artifact emission.

See `tests/conformance/lowering_abi/manifest.json` for machine-readable
indexing.
