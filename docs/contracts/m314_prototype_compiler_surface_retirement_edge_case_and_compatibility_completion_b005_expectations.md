# M314-B005 Expectations

Contract ID: `objc3c-cleanup-prototype-compiler-surface-retirement/m314-b005-v1`

`M314-B005` retires the last tracked Python prototype compiler source from the
live repository surface without destroying historical traceability.

Expected outcomes:

- No tracked non-`__pycache__` source files remain under `compiler/`.
- The former prototype semantic pass is preserved only as archival text under
  `spec/governance/retired_surfaces/compiler_objc3c/semantic.py.txt`.
- `README.md` states that `native/objc3c/` is the only supported compiler
  implementation root and that the earlier prototype surface is archival only.
- `package.json` carries machine-readable retirement state and archive-path
  metadata under `objc3cCommandCompatibility.prototypeSurface`.
- Any residual references to `compiler/objc3c/semantic.py` are confined to
  historical planning, contract, checker, test, or archival materials.

Compatibility notes:

- Historical milestone packets and baseline checkers may continue to mention the
  original tracked path because they freeze pre-retirement state.
- `M314-C001` inherits a cleaner workflow/API surface and must not reintroduce
  a second compiler implementation path.
