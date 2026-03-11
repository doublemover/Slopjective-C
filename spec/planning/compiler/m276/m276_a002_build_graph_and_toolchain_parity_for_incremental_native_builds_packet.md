# M276-A002 Build Graph And Toolchain Parity For Incremental Native Builds Packet

Packet: `M276-A002`

Issue: `#7392`

## Objective

Freeze the truthful parity requirements between the current monolithic
PowerShell native build and the future incremental backend before any default
command-surface change.

## Dependencies

- `M276-A001`

## Contract

- contract id
  `objc3c-native-build-graph-toolchain-parity/m276-a002-v1`
- contract model
  `same-source-set-but-not-yet-same-toolchain-link-publication-or-packet-behavior`
- parity model
  `behavioral-build-product-parity-with-stable-publication-paths-not-identical-object-topology`
- toolchain model
  `wrapper-owned-llvm-root-and-libclang-discovery-flow-into-future-configure-steps`
- compile database model
  `future-incremental-backend-emits-compile-commands-under-tmp-build-objc3c-native`

## Required anchors

- `docs/contracts/m276_build_graph_and_toolchain_parity_for_incremental_native_builds_a002_expectations.md`
- `scripts/check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py`
- `tests/tooling/test_check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py`
- `scripts/run_m276_a002_lane_a_readiness.py`
- `check:objc3c:m276-a002-build-graph-toolchain-parity-contract`
- `check:objc3c:m276-a002-lane-a-readiness`

## Frozen truthful current state

- the current PowerShell build and current `native/objc3c/CMakeLists.txt`
  already cover the same in-tree native source set
- the current PowerShell build remains authoritative because it still owns:
  - LLVM/libclang discovery
  - warning/define environment shape
  - final `artifacts/` publication
  - frontend packet generation
- the current CMake target graph is not parity-complete yet because it does not
  yet freeze:
  - LLVM include/libclang discovery and link behavior
  - warning-level parity
  - define-coverage parity for `OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION`
  - final artifact publication into `artifacts/bin` and `artifacts/lib`
  - packet-generation behavior

## Frozen parity requirements

- source coverage parity for:
  - `objc3c-native`
  - `objc3c-frontend-c-api-runner`
  - `objc3_runtime`
- include-directory parity
- compile-define parity
- relevant warning/standard parity
- LLVM/libclang discovery and link parity
- output-name and final publication parity under `artifacts/`
- compile-database emission at
  `tmp/build-objc3c-native/compile_commands.json`
- authoritative toolchain-root propagation from `LLVM_ROOT`
- parity measured by behaviorally equivalent published build products, not by
  identical compile topology

## Known gaps reserved for implementation issues

- `M276-C001`
  - backend/toolchain parity implementation
- `M276-C002`
  - binary/publication path split from packet generation
- `M276-C003`
  - packet dependency-shape decomposition

## Handoff

`M276-C001` is the explicit next handoff after this freeze closes.
