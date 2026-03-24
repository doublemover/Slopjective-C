# M272 Direct, Sealed, And Final Dispatch Lowering Expectations (C002)

Issue: `M272-C002`

## Required behavior

- Lower effective `objc_direct` sends on concrete `self` receivers as exact LLVM direct calls.
- Lower effective `objc_direct` sends on known class receivers as exact LLVM direct calls.
- Preserve `objc_dynamic` opt-out sites on the runtime dispatch entrypoint.
- Preserve effective direct/final callable intent in emitted method-list payloads.
- Preserve final/sealed container intent in emitted class/metaclass descriptor payloads.

## Deliberate bounds

- This issue does not introduce speculative devirtualization.
- This issue does not claim broad dynamic-receiver direct dispatch.
- This issue does not add a new public runtime ABI surface.

## Positive proof

- A deterministic positive fixture must compile through `objc3c-frontend-c-api-runner` and emit:
  - `module.manifest.json`
  - `module.ll`
  - `module.obj`
- The emitted manifest must preserve the `M272-C001` lowering-contract packet with truthful counts.
- The emitted IR must prove:
  - direct call from `main` into the class caller body
  - direct calls for effective-direct concrete `self` / known-class sends
  - runtime dispatch retained for the `objc_dynamic` opt-out site
  - widened method-entry payloads carrying direct/final bits
  - widened class/metaclass payloads carrying final/sealed bits
