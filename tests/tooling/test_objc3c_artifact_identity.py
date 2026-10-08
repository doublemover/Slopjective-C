from __future__ import annotations

import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.objc3c_package_channels import model as package_model
from scripts.objc3c_tooling.artifact_identity import (
    artifact_identity_for_platform,
    host_architecture,
)


def test_artifact_identity_preserves_windows_release_paths() -> None:
    identity = artifact_identity_for_platform("windows-x64")

    assert identity.native_executable_relative_path == "artifacts/bin/objc3c-native.exe"
    assert identity.native_executable_name == "objc3c-native.exe"
    assert identity.frontend_runner_relative_path == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
    assert identity.frontend_runner_name == "objc3c-frontend-c-api-runner.exe"
    assert identity.runtime_library_relative_path == "artifacts/lib/objc3_runtime.lib"
    assert identity.runtime_library_directory_relative_path == "artifacts/lib"
    assert identity.tampered_runtime_library_relative_path == "artifacts/lib/tampered_objc3_runtime.lib"
    assert identity.object_file_extension == ".obj"
    assert identity.object_artifact_name("module") == "module.obj"
    assert identity.module_object_artifact_name == "module.obj"
    assert identity.object_format == "COFF"
    assert identity.debug_format == "CodeView/PDB"
    assert identity.runtime_library_kind == "static-archive"
    assert identity.host_promotion_state == "supported-boundary"


def test_artifact_identity_resolves_linux_release_paths() -> None:
    identity = artifact_identity_for_platform("linux-x64")

    assert identity.native_executable_relative_path == "artifacts/bin/objc3c-native"
    assert identity.native_executable_name == "objc3c-native"
    assert identity.frontend_runner_relative_path == "artifacts/bin/objc3c-frontend-c-api-runner"
    assert identity.frontend_runner_name == "objc3c-frontend-c-api-runner"
    assert identity.runtime_library_relative_path == "artifacts/lib/libobjc3-runtime.so"
    assert identity.runtime_library_directory_relative_path == "artifacts/lib"
    assert identity.tampered_runtime_library_relative_path == "artifacts/lib/tampered_libobjc3-runtime.so"
    assert identity.object_file_extension == ".o"
    assert identity.object_artifact_name("module") == "module.o"
    assert identity.module_object_artifact_name == "module.o"
    assert identity.object_format == "ELF"
    assert identity.debug_format == "DWARF"
    assert identity.runtime_library_kind == "shared-library"
    assert identity.host_promotion_state == "fail-closed-until-native-host-evidence"


def test_artifact_identity_resolves_darwin_release_paths() -> None:
    identity = artifact_identity_for_platform("darwin-arm64")

    assert identity.native_executable_relative_path == "artifacts/bin/objc3c-native"
    assert identity.native_executable_name == "objc3c-native"
    assert identity.frontend_runner_relative_path == "artifacts/bin/objc3c-frontend-c-api-runner"
    assert identity.frontend_runner_name == "objc3c-frontend-c-api-runner"
    assert identity.runtime_library_relative_path == "artifacts/lib/libobjc3-runtime.dylib"
    assert identity.runtime_library_directory_relative_path == "artifacts/lib"
    assert identity.tampered_runtime_library_relative_path == "artifacts/lib/tampered_libobjc3-runtime.dylib"
    assert identity.object_file_extension == ".o"
    assert identity.object_artifact_name("module") == "module.o"
    assert identity.module_object_artifact_name == "module.o"
    assert identity.object_format == "Mach-O"
    assert identity.debug_format == "DWARF/dSYM"
    assert identity.runtime_library_kind == "shared-library"
    assert identity.host_promotion_state == "fail-closed-until-native-host-evidence"


def test_host_architecture_normalizes_common_machine_names() -> None:
    assert host_architecture("AMD64") == "x64"
    assert host_architecture("x86_64") == "x64"
    assert host_architecture("aarch64") == "arm64"


def test_package_channel_release_identity_uses_artifact_identity_helpers() -> None:
    for platform_id in package_model.RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES:
        identity = artifact_identity_for_platform(platform_id)
        expected_identity = {
            "target_triple": identity.target_triple,
            "object_format": identity.object_format,
            "debug_format": identity.debug_format,
            "runtime_library_kind": identity.runtime_library_kind,
            "native_executable": identity.native_executable_relative_path,
            "runtime_library": identity.runtime_library_relative_path,
            "runtime_library_name": identity.runtime_library_name,
        }

        assert package_model.release_package_artifact_identity_for_platform(
            platform_id
        ) == expected_identity
        assert (
            package_model.RELEASE_PACKAGE_ARTIFACT_IDENTITY_BY_PLATFORM[platform_id]
            == expected_identity
        )
        assert package_model.RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM[platform_id] == [
            identity.runtime_library_name
        ]


def test_package_channel_release_layout_uses_artifact_identity_paths() -> None:
    for platform_id in package_model.RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES:
        identity = artifact_identity_for_platform(platform_id)
        expected_layout = [
            package_model.MANIFEST_RELATIVE_PATH,
            identity.native_executable_relative_path,
            identity.runtime_library_relative_path,
            "stdlib/workspace.json",
            "stdlib/modules/objc3.core/module.json",
            "docs/runbooks/objc3c_packaging_channels.md",
        ]

        assert (
            package_model.release_package_layout_for_platform(platform_id)
            == expected_layout
        )
        assert (
            package_model.RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[platform_id]
            == expected_layout
        )
        assert (
            package_model.required_payload_entries_for_platform(
                target_platform_id=platform_id
            )
            == expected_layout
        )

    assert package_model.REQUIRED_PAYLOAD_ENTRIES == (
        package_model.RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[
            package_model.DEFAULT_TARGET_PLATFORM_ID
        ]
    )


def test_package_channel_direct_identity_helpers_reject_non_release_platforms() -> None:
    with pytest.raises(RuntimeError, match="unsupported package-channel target platform"):
        package_model.release_package_artifact_identity_payload("freebsd-x64")

    with pytest.raises(RuntimeError, match="unsupported package-channel target platform"):
        package_model.release_package_layout_for_platform("freebsd-x64")
