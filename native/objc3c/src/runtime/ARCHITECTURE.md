# objc3 Runtime Architecture

## Runtime Subsystem Model

The live runtime execution path is split into five owned subsystems:

1. compile publication
2. installation and registration
3. selector lookup and dispatch
4. property, storage, and ownership execution
5. acceptance and replay reporting

Owned code paths:

- compile publication:
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- installation and registration:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- selector lookup and dispatch:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- property, storage, and ownership execution:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- acceptance and replay reporting:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/objc3c_public_workflow_runner.py`

Allowed subsystem dependencies:

- compile publication -> installation and registration, selector lookup and dispatch, property/storage/ownership execution
- installation and registration -> runtime-owned state only
- selector lookup and dispatch -> installation and registration
- property, storage, and ownership execution -> installation and registration, selector lookup and dispatch
- acceptance and replay reporting -> compile publication, installation and registration, selector lookup and dispatch, property/storage/ownership execution

Forbidden subsystem shortcuts:

- acceptance or replay reporting must not claim runtime execution from sidecars alone
- selector or property execution must not bypass installation through alternate loader state
- compile publication must not widen the public runtime ABI outside `native/objc3c/src/runtime/objc3_runtime.h`
- milestone-specific closeout helpers must not become runtime subsystem dependencies
