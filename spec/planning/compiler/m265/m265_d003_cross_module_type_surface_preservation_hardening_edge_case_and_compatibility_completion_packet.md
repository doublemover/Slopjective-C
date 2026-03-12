# M265-D003 Packet

- Lane: `D`
- Contract id: `objc3c-part3-cross-module-type-surface-preservation/m265-d003-v1`
- Objective: harden separate-compilation and cross-module preservation so optional/key-path runtime packets remain visible after import-surface ingestion.
- Required artifacts:
  - `module.runtime-import-surface.json`
  - `module.cross-module-runtime-link-plan.json`
  - `module.cross-module-runtime-linker-options.rsp`
- Required preserved packets:
  - `objc_part3_optional_keypath_lowering_contract`
  - `objc_part3_optional_keypath_runtime_helper_contract`
- Required proof:
  - compile provider and consumer separately
  - ingest the provider import surface from the consumer
  - link a cross-module probe and prove imported typed key-path runtime metadata survives startup registration
