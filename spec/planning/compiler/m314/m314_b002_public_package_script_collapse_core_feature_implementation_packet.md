# M314-B002 Planning Packet

## Summary

Implement the compact public package-script surface promised by `M314-B001` and remove the compatibility alias mass immediately.

## Implementation shape

- Add one explicit `package.json` public command surface block.
- This is the package.json public command surface block for operators.

- Freeze the allowed public commands to a compact subset under the `25`-command budget.
- Update README workflow guidance to use public wrappers where those wrappers already exist.
- Remove the broader package alias mass instead of preserving it through `package.json`.

## Public subset scope

- `build`
- `build:objc3c-native`
- `build:objc3c-native:contracts`
- `build:objc3c-native:full`
- `build:objc3c-native:reconfigure`
- `build:spec`
- `compile:objc3c`
- `lint:spec`
- `test`
- `test:smoke`
- `test:ci`
- `test:objc3c`
- `test:objc3c:execution-smoke`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:full`
- `package:objc3c-native:runnable-toolchain`
- `proof:objc3c`

## Explicit non-goals

- Do not broaden the public package surface beyond the frozen subset.
- Do not reintroduce a package-level compatibility alias window.
- Do not collapse README transitional `--check` docs-builder usage yet.

## Evidence

- `spec/planning/compiler/m314/m314_b002_public_package_script_collapse_core_feature_implementation_surface.json`

Next issue: `M314-B003`.
