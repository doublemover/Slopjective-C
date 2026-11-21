# Native compiler maintainer workflows

Use the [current CLI appendix](objc3c_public_command_surface.md) and
[contributor checks](../../CONTRIBUTING.md). The source owners are the native
compiler/frontend/runtime, maintained LLVM patches and their focused fixtures.

## Native compiler edits

Prepare the pinned development tree using the README instructions. Edit its
real source files, then export the paths you changed into the maintained patch
stack. For an edit to `SemaExpr.cpp`, the export command is:

```sh
python -B -m scripts.objc3c_toolchain.export_development_patches --paths clang/lib/Sema/SemaExpr.cpp
python -B -m scripts.objc3c_toolchain seal-development
python -B -m scripts.objc3c_toolchain verify-development
```

Include all changed native source, headers and tests in the export. Commit the
maintained patches and manifest with the frontend/runtime integration and
fixtures. Do not commit the nested LLVM checkout or add a parallel draft tree.

Inspect the Linux build plan before a substantial compiler build:

```sh
python -B -m scripts.objc3c_toolchain check-host-tools --runtime
python -B -m scripts.objc3c_toolchain build-plan build --host linux-x86_64
```

Use bounded compile/link pools and focused checks while implementing a coherent
span. Native changes require a matching sealed/staged foundation before rebinding
the facade. Frontend/runtime work uses the affected CMake targets and tooling
pytest suites; document any postponed validation until it is completed.

## Documentation and onboarding

For site source changes, regenerate the checked-in page with
`python -B scripts/build_pages.py`. Run the route guard and focused tests:

```sh
python -B -m scripts.objc3c_toolchain.check_onboarding
python -B -m pytest -q tests/tooling/test_current_onboarding.py
```

The contributor guide and both command appendices are maintained documentation,
not outputs of the historical command generators. Reader routes must use the
current native CLI and supported helper entrypoints.

## Coordinated Linux delivery

When an implementation batch is ready, finish its postponed validation and run:

```sh
python -B -m scripts.objc3c_toolchain build-sdk
```

This operation builds/tests the foundation, runtime and facade, runs installed
qualification, seals an archive and activates the qualified installation. Reuse a
verified foundation when it has the same compiler source/build identity. Keep
source, scripts and fixtures fixed throughout the candidate run.

Fresh Debian/Ubuntu archive qualification is a separate operation owned by
`scripts/qualify_linux_distributions.py`; inspect its `--help` for the release
receipt and requested qualification scope. Record the archive checksum, SDK
seal, native build identity and actual checks in the acceptance receipt.

Do not repeat SDK/archive qualification for prose-only edits. Reclaim completed
test executables and duplicate stages through their existing owners, retain the
active SDK/archive, and keep build products and temporary evidence ignored.
