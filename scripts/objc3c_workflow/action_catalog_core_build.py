"""Core build and compile action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_BUILD_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-default": ActionSpec("build-default", "default public build entrypoint", "runner-internal"),
    "build-native-binaries": ActionSpec("build-native-binaries", "build native binaries", "pwsh:scripts/build_objc3c_native.ps1"),
    "build-native-contracts": ActionSpec("build-native-contracts", "build native contracts/binaries contract-artifact surface", "pwsh:scripts/build_objc3c_native.ps1"),
    "build-native-full": ActionSpec("build-native-full", "run full native build", "pwsh:scripts/build_objc3c_native.ps1"),
    "build-native-reconfigure": ActionSpec("build-native-reconfigure", "force native reconfigure build", "pwsh:scripts/build_objc3c_native.ps1"),
    "compile-objc3c": ActionSpec("compile-objc3c", "compile one Objective-C 3 fixture through the native compiler", "pwsh:scripts/objc3c_native_compile.ps1", pass_through_args=True),
}
