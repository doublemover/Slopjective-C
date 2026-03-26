# M314-B002 Planning Packet

## Summary

Implement the compact public package-script surface promised by `M314-B001` without removing compatibility aliases prematurely.

## Implementation shape

- Add one explicit `package.json` public command surface block.
- This is the package.json public command surface block for operators.

- Freeze the allowed public commands to a compact subset under the `25`-command budget.
- Update README workflow guidance to use public wrappers where those wrappers already exist.
- Leave the broader package alias mass in place as compatibility until `M314-B004`.

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

- Do not remove milestone-local package aliases yet.
- Do not remove historical compatibility scripts yet.
- Do not collapse README transitional `--check` docs-builder usage yet.

## Evidence

- `spec/planning/compiler/m314/m314_b002_public_package_script_collapse_core_feature_implementation_surface.json`

Next issue: `M314-B003`.
