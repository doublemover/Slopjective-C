from __future__ import annotations

from scripts.objc3c_tooling.artifact_identity import (
    artifact_identity_for_platform,
    host_architecture,
)


def test_artifact_identity_preserves_windows_release_paths() -> None:
    identity = artifact_identity_for_platform("windows-x64")

    assert identity.native_executable_relative_path == "artifacts/bin/objc3c-native.exe"
    assert identity.frontend_runner_relative_path == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
    assert identity.runtime_library_relative_path == "artifacts/lib/objc3_runtime.lib"
    assert identity.tampered_runtime_library_relative_path == "artifacts/lib/tampered_objc3_runtime.lib"
    assert identity.host_promotion_state == "supported-boundary"


def test_artifact_identity_resolves_linux_release_paths() -> None:
    identity = artifact_identity_for_platform("linux-x64")

    assert identity.native_executable_relative_path == "artifacts/bin/objc3c-native"
    assert identity.frontend_runner_relative_path == "artifacts/bin/objc3c-frontend-c-api-runner"
    assert identity.runtime_library_relative_path == "artifacts/lib/libobjc3-runtime.so"
    assert identity.tampered_runtime_library_relative_path == "artifacts/lib/tampered_libobjc3-runtime.so"
    assert identity.host_promotion_state == "fail-closed-until-native-host-evidence"


def test_artifact_identity_resolves_darwin_release_paths() -> None:
    identity = artifact_identity_for_platform("darwin-arm64")

    assert identity.native_executable_relative_path == "artifacts/bin/objc3c-native"
    assert identity.frontend_runner_relative_path == "artifacts/bin/objc3c-frontend-c-api-runner"
    assert identity.runtime_library_relative_path == "artifacts/lib/libobjc3-runtime.dylib"
    assert identity.tampered_runtime_library_relative_path == "artifacts/lib/tampered_libobjc3-runtime.dylib"
    assert identity.host_promotion_state == "fail-closed-until-native-host-evidence"


def test_host_architecture_normalizes_common_machine_names() -> None:
    assert host_architecture("AMD64") == "x64"
    assert host_architecture("x86_64") == "x64"
    assert host_architecture("aarch64") == "arm64"
