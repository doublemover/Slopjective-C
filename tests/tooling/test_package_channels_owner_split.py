from __future__ import annotations

import importlib
from pathlib import Path

import pytest

from scripts.objc3c_package_channels.model import (
    IMPLEMENTED_CHANNELS,
    PackageChannelInputs,
    PackageChannelPaths,
    archive_digest_payloads,
    package_channel_paths,
    package_channels_manifest_payload,
    package_channels_report_payload,
)
from scripts.objc3c_package_channels.rendering import (
    install_script_text,
    offline_bootstrap_script_text,
)
from scripts.objc3c_package_channels.validation import validate_manifest_required_fields


ROOT = Path(__file__).resolve().parents[2]


OWNER_MODULES = (
    "scripts.objc3c_package_channels.paths",
    "scripts.objc3c_package_channels.commands",
    "scripts.objc3c_package_channels.model",
    "scripts.objc3c_package_channels.loading",
    "scripts.objc3c_package_channels.validation",
    "scripts.objc3c_package_channels.rendering",
    "scripts.objc3c_package_channels.publication",
    "scripts.objc3c_package_channels.cli",
)


def sample_inputs() -> PackageChannelInputs:
    return PackageChannelInputs(
        supported_platforms={"default_platform_id": "windows-x64"},
        metadata_surface={
            "required_manifest_fields": [
                "contract_id",
                "platform_id",
                "package_root",
                "installer_signature",
                "archive_digests",
                "portable_archive",
                "installer_archive",
                "offline_archive",
            ],
            "required_installer_signature_fields": [
                "signature_format",
                "signing_key_id",
                "subject",
                "artifact",
                "sha256",
                "verification_command",
                "trust_scope",
            ],
            "required_archive_digest_fields": [
                "digest_format",
                "artifact_role",
                "artifact",
                "sha256",
                "verification_command",
                "trust_scope",
            ],
        },
        platform_support_matrix={
            "claim_boundary": {"supported_platform_ids": ["windows-x64"]},
            "tiers": [{"platform_id": "windows-x64", "tier": "release"}],
        },
        interop_loader_metadata={
            "contract_id": "objc3c.package_ecosystem.mixed_image_interop_loader_metadata.v1",
            "source": "tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json",
            "support": "local-mixed-image-metadata-digest-checked",
            "package_count": 2,
            "package_ids": ["showcase:patchKit", "stdlib:objc3.system"],
            "header_import_count": 5,
            "header_export_count": 3,
            "abi_alignment_count": 2,
            "foreign_type_count": 2,
            "mixed_image_count": 3,
            "bridge_surface_count": 4,
            "objcxx_bridge_surface_count": 2,
            "swift_bridge_surface_count": 2,
            "positive_fixture_count": 5,
            "negative_fixture_count": 4,
            "tamper_rejection_diagnostic": "O3PKG8054",
            "unsupported_surfaces": [
                "hosted registry mixed-image restore",
                "network-resolved interop metadata",
                "unchecked ABI alignment fallback",
            ],
        },
    )


def sample_installer_signature() -> dict[str, str]:
    return {
        "signature_format": "objc3c-local-sha256-v1",
        "signing_key_id": "objc3c-release-operations-local-installer-key-v1",
        "subject": "local-installer",
        "artifact": "tmp/artifacts/package-channels/unit-run/windows-x64/installer/objc3c-windows-x64-installer.zip",
        "sha256": "0" * 64,
        "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
        "trust_scope": "checked-in-artifact-digest",
    }


def sample_archive_digests() -> dict[str, dict[str, str]]:
    return {
        "portable_archive": {
            "digest_format": "sha256",
            "artifact_role": "portable-archive",
            "artifact": "tmp/artifacts/package-channels/unit-run/windows-x64/portable/objc3c-windows-x64-portable.zip",
            "sha256": "1" * 64,
            "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
            "trust_scope": "checked-in-artifact-digest",
        },
        "installer_archive": {
            "digest_format": "sha256",
            "artifact_role": "local-installer",
            "artifact": "tmp/artifacts/package-channels/unit-run/windows-x64/installer/objc3c-windows-x64-installer.zip",
            "sha256": "0" * 64,
            "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
            "trust_scope": "checked-in-artifact-digest",
        },
        "offline_archive": {
            "digest_format": "sha256",
            "artifact_role": "offline-bundle",
            "artifact": "tmp/artifacts/package-channels/unit-run/windows-x64/offline/objc3c-windows-x64-offline-bundle.zip",
            "sha256": "2" * 64,
            "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
            "trust_scope": "checked-in-artifact-digest",
        },
    }


def test_package_channel_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_package_channel_entrypoint_delegates_to_owner_package() -> None:
    script_text = (ROOT / "scripts" / "build_objc3c_package_channels.py").read_text(encoding="utf-8")
    assert "objc3c_package_channels.cli" in script_text
    assert "def install_script_text" not in script_text
    assert "zipfile." not in script_text
    assert "shutil.copytree" not in script_text
    assert "write_json_file" not in script_text


def test_package_channel_manifest_and_report_are_owned_by_model() -> None:
    paths = package_channel_paths("unit-run")
    inputs = sample_inputs()
    signature = sample_installer_signature()
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=paths,
        installer_signature=signature,
        archive_digests=sample_archive_digests(),
    )
    report = package_channels_report_payload(inputs=inputs, paths=paths, manifest_payload=manifest)

    assert manifest["contract_id"] == "objc3c.packaging.channels.summary.v1"
    assert manifest["platform_id"] == "windows-x64"
    assert manifest["implemented_channels"] == IMPLEMENTED_CHANNELS
    assert manifest["interop_loader_metadata"]["support"] == "local-mixed-image-metadata-digest-checked"
    assert manifest["interop_loader_metadata"]["header_import_count"] == 5
    assert manifest["interop_loader_metadata"]["objcxx_bridge_surface_count"] == 2
    assert manifest["interop_loader_metadata"]["swift_bridge_surface_count"] == 2
    assert manifest["installer_signature"] == signature
    assert manifest["archive_digests"]["portable_archive"]["artifact_role"] == "portable-archive"
    assert manifest["archive_digests"]["installer_archive"]["sha256"] == signature["sha256"]
    assert manifest["archive_digests"]["offline_archive"]["artifact"].endswith(
        "objc3c-windows-x64-offline-bundle.zip"
    )
    assert manifest["portable_archive"].endswith("objc3c-windows-x64-portable.zip")
    assert report["manifest_path"].endswith("objc3c-package-channels-manifest.json")
    assert report["implemented_channels"] == IMPLEMENTED_CHANNELS
    assert report["interop_loader_metadata"]["tamper_rejection_diagnostic"] == "O3PKG8054"
    assert report["installer_signature"]["signature_format"] == "objc3c-local-sha256-v1"
    assert report["archive_digests"]["installer_archive"]["digest_format"] == "sha256"


def test_package_channel_archive_digest_payloads_are_owned_by_model(tmp_path: Path) -> None:
    build_root = ROOT / "tmp" / "tests" / "package-channel-digests" / tmp_path.name
    paths = PackageChannelPaths(
        run_id="unit-run",
        package_root=build_root / "runnable",
        build_root=build_root,
        portable_archive=build_root / "portable" / "objc3c-windows-x64-portable.zip",
        installer_image_root=build_root / "installer" / "image",
        installer_archive=build_root / "installer" / "objc3c-windows-x64-installer.zip",
        offline_bundle_root=build_root / "offline" / "bundle",
        offline_archive=build_root / "offline" / "objc3c-windows-x64-offline-bundle.zip",
        manifest_path=build_root / "objc3c-package-channels-manifest.json",
    )
    archive_payloads = {
        paths.portable_archive: b"portable",
        paths.installer_archive: b"installer",
        paths.offline_archive: b"offline",
    }
    for archive_path, payload in archive_payloads.items():
        archive_path.parent.mkdir(parents=True, exist_ok=True)
        archive_path.write_bytes(payload)

    digests = archive_digest_payloads(paths)

    assert set(digests) == {"portable_archive", "installer_archive", "offline_archive"}
    assert digests["portable_archive"]["artifact_role"] == "portable-archive"
    assert digests["installer_archive"]["artifact_role"] == "local-installer"
    assert digests["offline_archive"]["artifact_role"] == "offline-bundle"
    assert all(len(record["sha256"]) == 64 for record in digests.values())


def test_package_channel_validation_fails_closed_on_required_manifest_drift() -> None:
    inputs = sample_inputs()
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=package_channel_paths("unit-run"),
        installer_signature=sample_installer_signature(),
        archive_digests=sample_archive_digests(),
    )
    del manifest["offline_archive"]

    with pytest.raises(RuntimeError, match="offline_archive"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_validation_fails_closed_on_signature_drift() -> None:
    inputs = sample_inputs()
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=package_channel_paths("unit-run"),
        installer_signature={
            **sample_installer_signature(),
            "artifact": "tmp/artifacts/package-channels/unit-run/windows-x64/installer/drifted.zip",
        },
        archive_digests=sample_archive_digests(),
    )

    with pytest.raises(RuntimeError, match="installer_signature artifact"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_validation_fails_closed_on_archive_digest_drift() -> None:
    inputs = sample_inputs()
    archive_digests = sample_archive_digests()
    archive_digests["offline_archive"] = {
        **archive_digests["offline_archive"],
        "sha256": "not-a-sha256",
    }
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=package_channel_paths("unit-run"),
        installer_signature=sample_installer_signature(),
        archive_digests=archive_digests,
    )

    with pytest.raises(RuntimeError, match=r"archive_digests\.offline_archive sha256"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_validation_requires_installer_signature_digest_parity() -> None:
    inputs = sample_inputs()
    archive_digests = sample_archive_digests()
    archive_digests["installer_archive"] = {
        **archive_digests["installer_archive"],
        "sha256": "3" * 64,
    }
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=package_channel_paths("unit-run"),
        installer_signature=sample_installer_signature(),
        archive_digests=archive_digests,
    )

    with pytest.raises(RuntimeError, match="installer_signature digest drifted"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_script_rendering_is_owned_by_rendering_module() -> None:
    install_text = install_script_text()
    offline_text = offline_bootstrap_script_text()

    assert "objc3c.packaging.channels.install-receipt.v1" in install_text
    assert "bootstrap_entrypoint = \"Bootstrap-objc3cEnvironment.ps1\"" in install_text
    assert "package_bridge = \"objc3c\"" in install_text
    assert "install_command = \"npm run objc3c -- build-package-channels\"" in install_text
    assert "Copy-Item -LiteralPath $sourceRoot" in install_text
    assert "OfflineBootstrap-objc3c.ps1" not in install_text
    assert "objc3c-windows-x64-installer.zip" in offline_text
    assert "Expand-Archive -LiteralPath $installerArchive" in offline_text
