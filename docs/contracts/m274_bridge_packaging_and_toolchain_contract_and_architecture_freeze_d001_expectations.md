Contract ID: `objc3c-part11-bridge-packaging-and-toolchain-contract/m274-d001-v1`
Issue: `#7370`

Current expectation:
- the current Part 11 packaging/toolchain boundary is frozen over the packaged runtime archive, runtime registration manifest, cross-module runtime link plan, and linker-response sidecar
- the emitted IR carries:
  - `; part11_bridge_packaging_toolchain_contract = ...`
  - `!objc3.objc_part11_bridge_packaging_and_toolchain_contract = !{!111}`
- the private runtime proof surface includes `objc3_runtime_copy_part11_bridge_packaging_toolchain_snapshot_for_testing`
- mixed-module link plans preserve imported Part 11 metadata/interface preservation facts and fail closed on drift
- header generation, module generation, and bridge generation remain deferred to `M274-D002`
- the issue-local checker is fail-closed