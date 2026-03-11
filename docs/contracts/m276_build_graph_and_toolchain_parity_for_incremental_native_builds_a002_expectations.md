# M276 Build Graph And Toolchain Parity For Incremental Native Builds Expectations (A002)

Contract ID: `objc3c-native-build-graph-toolchain-parity/m276-a002-v1`

## Objective

Freeze the truthful parity requirements between the current authoritative
PowerShell native build and the reserved future incremental backend before the
default local build path changes.

## Required implementation

1. Add a canonical expectations document for the M276-A002 parity freeze.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py`
   - `tests/tooling/test_check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py`
   - `scripts/run_m276_a002_lane_a_readiness.py`
3. Add `M276-A002` anchor text to:
   - `docs/objc3c-native/src/50-artifacts.md`
   - `docs/objc3c-native.md`
   - `README.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `scripts/build_objc3c_native.ps1`
   - `native/objc3c/CMakeLists.txt`
   - `native/objc3c/src/driver/objc3_objectivec_path.cpp`
4. Freeze the current truthful parity state:
   - source inventory parity is already true for the in-tree native source set
   - compile/link/tool/publication parity is not yet true
   - packet-generation parity is not yet true
   - generator and compile-database policy are not yet implemented beyond this freeze
5. Freeze the parity surface that later M276 issues must satisfy:
   - source-file coverage parity for `objc3c-native`
   - source-file coverage parity for `objc3c-frontend-c-api-runner`
   - source-file coverage parity for `objc3_runtime`
   - include-directory parity
   - compile-define parity
   - relevant warning/standard flag parity
   - LLVM/libclang discovery and link parity
   - final output-name parity
   - final artifact-location parity under `artifacts/`
   - compile-database emission at `tmp/build-objc3c-native/compile_commands.json`
   - authoritative toolchain-root propagation from `LLVM_ROOT`
6. Freeze that parity is measured by behaviorally equivalent build products and
   publication semantics, not by identical object-file topology.
7. Freeze the current known parity gaps without pretending they are solved:
   - `scripts/build_objc3c_native.ps1` performs direct `LLVM_ROOT`, `clang++`,
     `llvm-lib`, and `libclang` resolution
   - `native/objc3c/CMakeLists.txt` does not yet model that external toolchain
     discovery/link contract
   - the script publishes final outputs into `artifacts/`, while the current
     CMake target graph does not yet freeze those output directories
   - the script applies `OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1` broadly,
     while the current CMake target graph only wires that define on
     `objc3c_runtime_abi`
   - the current PowerShell build also generates the frontend packet family,
     while the current CMake graph is binary-only
8. `package.json` must wire:
   - `check:objc3c:m276-a002-build-graph-toolchain-parity-contract`
   - `test:tooling:m276-a002-build-graph-toolchain-parity-contract`
   - `check:objc3c:m276-a002-lane-a-readiness`
9. The contract must explicitly hand off to `M276-C001`.

## Canonical models

- Contract model:
  `same-source-set-but-not-yet-same-toolchain-link-publication-or-packet-behavior`
- Parity model:
  `behavioral-build-product-parity-with-stable-publication-paths-not-identical-object-topology`
- Toolchain model:
  `wrapper-owned-llvm-root-and-libclang-discovery-flow-into-future-configure-steps`
- Compile database model:
  `future-incremental-backend-emits-compile-commands-under-tmp-build-objc3c-native`

## Non-goals

- No incremental backend implementation yet.
- No default-path flip yet.
- No packet-family decomposition yet.
- No readiness-runner migration yet.

## Evidence

- `tmp/reports/m276/M276-A002/build_graph_and_toolchain_parity_summary.json`
