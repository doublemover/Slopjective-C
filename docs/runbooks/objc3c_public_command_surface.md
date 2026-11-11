# Current Objective-C 3 command surface

The repository command owner is [the native toolchain CLI](../../scripts/objc3c_toolchain/cli.py).
`npm run objc3c -- ...` invokes the same module as
`python -B -m scripts.objc3c_toolchain ...`. Discover its current arguments with:

```sh
python -B -m scripts.objc3c_toolchain --help
python -B -m scripts.objc3c_toolchain compile --help
```

## Repository commands

| Command | Purpose |
| --- | --- |
| `prepare` | Materialize the pinned LLVM source and maintained patches. |
| `verify` | Verify prepared source against its manifest. |
| `seal-development` | Seal the canonical development tree after patch export. |
| `verify-development` | Compare development source with the sealed patch stack. |
| `build-plan` | Print an identified configure/build/test/stage command without executing it. |
| `build` | Build and test the sealed compiler foundation; optionally stage it. |
| `facade-identity` | Identify frontend source and CMake inputs. |
| `check-host-tools` | Check pinned tool references; optionally probe installed versions. |
| `build-sdk` | Build, qualify, package and install the native Linux SDK. |
| `package-sdk` | Seal and cache an installed SDK using its validation report. |
| `compile` | Invoke the public compiler from the selected installed SDK. |
| `build-packages` | Build a locked source-package workspace. |
| `packages` | Publish, install and consume immutable offline packages. |

These commands are maintained directly; this appendix is not generated from the
retired workflow inventory. Update it when the CLI command set changes.

## Installed compiler and package tools

Activate an installed SDK as described in the [README](../../README.md). Use
`objc3c --help` for compiler arguments and `objc3-packages --help` for immutable
package operations. The installed SDK supplies native Clang, LLVM tools, runtime
libraries, headers and package examples. Compiling an application against that
SDK does not rebuild the compiler.

## Contributor checks

Formatting, Markdown lint, focused pytest checks and the reader-route guard are
separate tools. Their supported invocations are listed in
[Contributing](../../CONTRIBUTING.md). Compiler patch export and coordinated
SDK qualification are described in the [maintainer guide](objc3c_maintainer_workflows.md).
