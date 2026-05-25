from __future__ import annotations

import hashlib
import importlib
import json
import os
import shutil
import sys
from pathlib import Path
from types import SimpleNamespace

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.objc3c_package_channels import commands as package_commands
from scripts.objc3c_package_channels.model import (
    IMPLEMENTED_CHANNELS,
    MANIFEST_RELATIVE_PATH,
    PackageChannelInputs,
    PackageChannelPaths,
    REQUIRED_PAYLOAD_ENTRIES,
    archive_digest_payloads,
    native_executable_entry_from_runnable_manifest,
    package_channel_paths,
    package_channels_manifest_payload,
    package_channels_report_payload,
    receipt_contract_payloads,
    release_package_artifact_identity_for_platform,
    release_package_channel_id_for_platform,
    release_package_id_for_platform,
    required_payload_entries_for_platform,
)
from scripts.objc3c_package_channels.publication import prepare_package_channel_workspace
from scripts.objc3c_package_channels.rendering import (
    install_script_text,
    offline_bootstrap_script_text,
    uninstall_script_text,
)
from scripts.objc3c_package_channels.validation import validate_manifest_required_fields
from scripts.objc3c_tooling.paths import repo_rel


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
                "package_id",
                "package_channel_id",
                "sanitizer_variant",
                "package_root",
                "installer_signature",
                "archive_digests",
                "payload_contract",
                "receipt_contracts",
                "package_runtime_models",
                "portable_archive",
                "installer_archive",
                "offline_archive",
                "support_truth",
                "native_execution_claimed",
            ],
            "required_receipt_fields": [
                "contract_id",
                "install_root",
                "install_home",
                "channel_id",
                "bootstrap_entrypoint",
                "package_bridge",
                "install_command",
                "payload_manifest",
                "payload_manifest_sha256",
                "payload_required_entries",
                "target_platform_id",
                "package_id",
                "package_channel_id",
                "sanitizer_variant",
                "package_runtime_model",
                "support_truth",
                "native_execution_claimed",
                "installed_at_utc",
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
            "required_payload_contract_fields": [
                "contract_id",
                "source",
                "manifest_relative_path",
                "manifest_artifact",
                "manifest_sha256",
                "target_platform_id",
                "required_entries",
                "entry_digests",
                "clean_room_source_policy",
            ],
            "required_receipt_contract_fields": [
                "contract_id",
                "schema",
                "receipt_path",
                "channel_id",
                "emitted_by",
                "bootstrap_entrypoint",
                "package_bridge",
                "install_command",
                "payload_manifest",
                "payload_required_entries",
                "required_fields",
                "network_policy",
                "rollback_required",
                "package_id",
                "package_channel_id",
                "sanitizer_variant",
                "target_platform_id",
                "package_runtime_model",
                "emitted_platform_fields",
                "support_truth",
                "native_execution_claimed",
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


def sample_installer_signature(paths: PackageChannelPaths | None = None) -> dict[str, str]:
    resolved_paths = (
        paths
        if paths is not None
        else package_channel_paths("unit-run", target_platform_id="windows-x64")
    )
    return {
        "signature_format": "objc3c-local-sha256-v1",
        "signing_key_id": "objc3c-release-operations-local-installer-key-v1",
        "subject": "local-installer",
        "artifact": repo_rel(resolved_paths.installer_archive),
        "sha256": "0" * 64,
        "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
        "trust_scope": "checked-in-artifact-digest",
    }


def sample_archive_digests(
    paths: PackageChannelPaths | None = None,
) -> dict[str, dict[str, str]]:
    resolved_paths = (
        paths
        if paths is not None
        else package_channel_paths("unit-run", target_platform_id="windows-x64")
    )
    return {
        "portable_archive": {
            "digest_format": "sha256",
            "artifact_role": "portable-archive",
            "artifact": repo_rel(resolved_paths.portable_archive),
            "sha256": "1" * 64,
            "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
            "trust_scope": "checked-in-artifact-digest",
        },
        "installer_archive": {
            "digest_format": "sha256",
            "artifact_role": "local-installer",
            "artifact": repo_rel(resolved_paths.installer_archive),
            "sha256": "0" * 64,
            "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
            "trust_scope": "checked-in-artifact-digest",
        },
        "offline_archive": {
            "digest_format": "sha256",
            "artifact_role": "offline-bundle",
            "artifact": repo_rel(resolved_paths.offline_archive),
            "sha256": "2" * 64,
            "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
            "trust_scope": "checked-in-artifact-digest",
        },
    }


def sample_payload_contract(paths: PackageChannelPaths | None = None) -> dict[str, object]:
    resolved_paths = (
        paths
        if paths is not None
        else package_channel_paths("unit-run", target_platform_id="windows-x64")
    )
    required_entries = required_payload_entries_for_platform(
        target_platform_id=resolved_paths.target_platform_id,
    )
    entry_digests = {
        relative_path: {
            "digest_format": "sha256",
            "artifact": relative_path,
            "sha256": f"{index:x}" * 64,
        }
        for index, relative_path in enumerate(required_entries, start=4)
    }
    return {
        "contract_id": "objc3c.packaging.channels.payload-contract.v1",
        "source": "canonical-runnable-toolchain-package",
        "manifest_relative_path": MANIFEST_RELATIVE_PATH,
        "manifest_artifact": f"{repo_rel(resolved_paths.package_root)}/{MANIFEST_RELATIVE_PATH}",
        "manifest_sha256": entry_digests[MANIFEST_RELATIVE_PATH]["sha256"],
        "target_platform_id": resolved_paths.target_platform_id,
        "required_entries": required_entries,
        "entry_digests": entry_digests,
        "clean_room_source_policy": "fresh-owned-tmp-root-only",
    }


def sample_receipt_contracts(
    target_platform_id: str = "windows-x64",
) -> dict[str, dict[str, object]]:
    return receipt_contract_payloads(target_platform_id=target_platform_id)


def write_json(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def release_foundation_reuse_fixture(tmp_path: Path) -> dict[str, Path]:
    fixture_root = ROOT / "tmp" / "tests" / "package-channel-release-reuse" / tmp_path.name
    if fixture_root.exists():
        shutil.rmtree(fixture_root)

    primary_package_manifest = (
        fixture_root
        / "pkg"
        / "run-1"
        / "artifacts"
        / "package"
        / "objc3c-runnable-toolchain-package.json"
    )
    release_evidence_index = fixture_root / "reports" / "release_evidence" / "evidence-index.json"
    repo_superclean_surface = (
        fixture_root / "pkg" / "run-1" / "tmp" / "build-objc3c-native" / "repo_superclean.json"
    )
    manifest_path = (
        fixture_root
        / "artifacts"
        / "release-foundation"
        / "manifest"
        / "objc3c-release-manifest.json"
    )
    sbom_path = (
        fixture_root
        / "artifacts"
        / "release-foundation"
        / "sbom"
        / "objc3c-release-sbom.json"
    )
    attestation_path = (
        fixture_root
        / "artifacts"
        / "release-foundation"
        / "attestation"
        / "objc3c-release-attestation.json"
    )
    manifest_summary_path = (
        fixture_root / "reports" / "release-foundation" / "release-manifest-summary.json"
    )
    abi_api_drift_summary_path = (
        fixture_root / "reports" / "release-foundation" / "abi-api-drift-summary.json"
    )
    publication_summary_path = (
        fixture_root / "reports" / "release-foundation" / "publication-summary.json"
    )

    write_json(primary_package_manifest, {"contract_id": "objc3c.package.unit"})
    write_json(release_evidence_index, {"contract_id": "objc3c.release.evidence.unit"})
    write_json(repo_superclean_surface, {"contract_id": "objc3c.repo.superclean.unit"})
    write_json(
        abi_api_drift_summary_path,
        {
            "contract_id": "objc3c.release.foundation.abi_api_drift.summary.v1",
            "status": "PASS",
        },
    )

    release_payload_digest = "a" * 64
    write_json(
        manifest_path,
        {
            "contract_id": "objc3c.release.foundation.manifest.v1",
            "reproducibility_match": True,
            "primary_package_manifest_sha256": file_sha256(primary_package_manifest),
            "release_payload_digest_sha256": release_payload_digest,
        },
    )
    write_json(
        sbom_path,
        {
            "contract_id": "objc3c.release.foundation.sbom.v1",
            "release_payload_digest_sha256": release_payload_digest,
        },
    )
    write_json(
        attestation_path,
        {
            "contract_id": "objc3c.release.foundation.attestation.v1",
            "attested_digests": {
                "release_manifest_sha256": file_sha256(manifest_path),
                "release_payload_digest_sha256": release_payload_digest,
                "package_manifest_sha256": file_sha256(primary_package_manifest),
                "sbom_sha256": file_sha256(sbom_path),
            },
        },
    )
    write_json(
        manifest_summary_path,
        {
            "contract_id": "objc3c.release.foundation.manifest.summary.v1",
            "status": "PASS",
            "release_manifest_path": repo_rel(manifest_path),
            "reproducibility_match": True,
            "primary_package_manifest_path": repo_rel(primary_package_manifest),
            "primary_package_manifest_sha256": file_sha256(primary_package_manifest),
            "release_evidence_index_path": repo_rel(release_evidence_index),
            "release_evidence_index_sha256": file_sha256(release_evidence_index),
            "abi_api_drift_summary_path": repo_rel(abi_api_drift_summary_path),
            "abi_api_drift_summary_sha256": file_sha256(abi_api_drift_summary_path),
            "repo_superclean_surface_path": repo_rel(repo_superclean_surface),
            "repo_superclean_surface_sha256": file_sha256(repo_superclean_surface),
        },
    )
    write_json(
        publication_summary_path,
        {
            "contract_id": "objc3c.release.foundation.publication.summary.v1",
            "status": "PASS",
            "release_manifest_path": repo_rel(manifest_path),
            "release_manifest_sha256": file_sha256(manifest_path),
            "sbom_path": repo_rel(sbom_path),
            "sbom_sha256": file_sha256(sbom_path),
            "attestation_path": repo_rel(attestation_path),
            "attestation_sha256": file_sha256(attestation_path),
        },
    )
    return {
        "root": fixture_root,
        "manifest": manifest_path,
        "sbom": sbom_path,
        "attestation": attestation_path,
        "manifest_summary": manifest_summary_path,
        "abi_api_drift_summary": abi_api_drift_summary_path,
        "publication_summary": publication_summary_path,
        "integration_summary": fixture_root
        / "reports"
        / "release-foundation"
        / "integration-summary.json",
    }


def runnable_package_reuse_fixture(
    tmp_path: Path,
    *,
    target_platform_id: str = "windows-x64",
    sanitizer_variant: str = "release",
) -> Path:
    fixture_root = ROOT / "tmp" / "tests" / "package-channel-runnable-reuse" / tmp_path.name
    if fixture_root.exists():
        shutil.rmtree(fixture_root)

    required_entries = required_payload_entries_for_platform(
        sanitizer_variant=sanitizer_variant,
        target_platform_id=target_platform_id,
    )
    for relative_path in required_entries:
        if relative_path == MANIFEST_RELATIVE_PATH:
            continue
        artifact_path = fixture_root / relative_path
        artifact_path.parent.mkdir(parents=True, exist_ok=True)
        artifact_path.write_text(f"unit payload for {relative_path}\n", encoding="utf-8")
    write_json(
        fixture_root / MANIFEST_RELATIVE_PATH,
        {
            "contract_id": "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1",
            "runtime_variant": sanitizer_variant,
            "package_root": repo_rel(fixture_root),
            "manifest_artifact": MANIFEST_RELATIVE_PATH,
            "target_platform_id": target_platform_id,
            **release_package_artifact_identity_for_platform(target_platform_id),
            "package_root_layout": required_entries,
            "support_truth": False,
            "native_execution_claimed": False,
        },
    )
    return fixture_root


def configure_release_foundation_reuse_fixture(
    monkeypatch: pytest.MonkeyPatch,
    fixture: dict[str, Path],
) -> None:
    monkeypatch.setattr(
        package_commands,
        "RELEASE_FOUNDATION_MANIFEST_SUMMARY",
        fixture["manifest_summary"],
    )
    monkeypatch.setattr(
        package_commands,
        "RELEASE_FOUNDATION_ABI_API_DRIFT_SUMMARY",
        fixture["abi_api_drift_summary"],
    )
    monkeypatch.setattr(
        package_commands,
        "RELEASE_FOUNDATION_PUBLICATION_SUMMARY",
        fixture["publication_summary"],
    )
    monkeypatch.setattr(package_commands, "RELEASE_FOUNDATION_MANIFEST", fixture["manifest"])
    monkeypatch.setattr(package_commands, "RELEASE_FOUNDATION_SBOM", fixture["sbom"])
    monkeypatch.setattr(
        package_commands,
        "RELEASE_FOUNDATION_ATTESTATION",
        fixture["attestation"],
    )


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


def test_package_channel_cli_accepts_release_target_platform_id() -> None:
    from scripts.objc3c_package_channels import cli as package_cli

    args = package_cli.parse_args(["--target-platform-id", "darwin-arm64"])

    assert args.sanitizer_variant == "release"
    assert args.target_platform_id == "darwin-arm64"


@pytest.mark.parametrize("sanitizer_variant", ["address", "undefined"])
def test_package_channel_cli_rejects_sanitizer_target_platform_override(
    sanitizer_variant: str,
) -> None:
    from scripts.objc3c_package_channels import cli as package_cli

    with pytest.raises(SystemExit) as exc_info:
        package_cli.parse_args(
            [
                "--sanitizer-variant",
                sanitizer_variant,
                "--target-platform-id",
                "windows-x64",
            ]
        )

    assert exc_info.value.code == 2


def test_package_channel_model_rejects_sanitizer_target_platform_override() -> None:
    with pytest.raises(RuntimeError, match="target-platform override"):
        package_channel_paths(
            "unit-asan",
            sanitizer_variant="address",
            target_platform_id="windows-x64",
        )


def test_package_channel_cli_forwards_release_target_platform_id(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    from scripts.objc3c_package_channels import cli as package_cli

    captured: dict[str, object] = {}
    inputs = sample_inputs()
    monkeypatch.delenv("OBJC3C_TARGET_PLATFORM_ID", raising=False)
    monkeypatch.setattr(package_cli, "package_channel_run_id", lambda: "unit-cli")
    monkeypatch.setattr(package_cli, "load_package_channel_surface_inputs", lambda: {})
    monkeypatch.setattr(
        package_cli,
        "load_package_channel_inputs",
        lambda surface_inputs: inputs,
    )
    monkeypatch.setattr(package_cli, "build_support_matrix", lambda: None)

    def fake_build_release_foundation_artifacts(
        *,
        reuse_existing: bool = False,
        reuse_primary_package_root: Path | None = None,
    ) -> None:
        captured["foundation_env_target_platform_id"] = os.environ.get(
            "OBJC3C_TARGET_PLATFORM_ID"
        )
        captured["reuse_existing"] = reuse_existing
        captured["reuse_primary_package_root"] = reuse_primary_package_root

    def fake_build_runnable_package(
        package_root: Path,
        manifest_relative_path: str,
        sanitizer_variant: str = "release",
    ) -> None:
        captured["build_env_target_platform_id"] = os.environ.get(
            "OBJC3C_TARGET_PLATFORM_ID"
        )
        captured["package_root"] = package_root
        captured["manifest_relative_path"] = manifest_relative_path
        captured["sanitizer_variant"] = sanitizer_variant

    def fake_package_channels_manifest_payload(
        *,
        inputs: PackageChannelInputs,
        paths: PackageChannelPaths,
        installer_signature: dict[str, object],
    ) -> dict[str, object]:
        captured["manifest_target_platform_id"] = paths.target_platform_id
        captured["manifest_package_channel_id"] = paths.package_channel_id
        return {"platform_id": paths.target_platform_id}

    monkeypatch.setattr(
        package_cli,
        "build_release_foundation_artifacts",
        fake_build_release_foundation_artifacts,
    )
    monkeypatch.setattr(
        package_cli,
        "prepare_package_channel_workspace",
        lambda paths, preserve_package_root=False: None,
    )
    monkeypatch.setattr(package_cli, "build_runnable_package", fake_build_runnable_package)
    monkeypatch.setattr(package_cli, "publish_portable_archive", lambda paths: None)
    monkeypatch.setattr(package_cli, "publish_installer_archive", lambda paths: None)
    monkeypatch.setattr(package_cli, "publish_offline_bundle", lambda paths: None)
    monkeypatch.setattr(package_cli, "installer_signature_payload", lambda installer_archive: {})
    monkeypatch.setattr(
        package_cli,
        "package_channels_manifest_payload",
        fake_package_channels_manifest_payload,
    )
    monkeypatch.setattr(
        package_cli,
        "validate_manifest_required_fields",
        lambda *, manifest_payload, metadata_surface: None,
    )
    monkeypatch.setattr(
        package_cli,
        "write_package_channel_artifacts",
        lambda *, inputs, paths, manifest_payload: {},
    )
    monkeypatch.setattr(package_cli, "print_package_channel_result", lambda paths: None)

    assert package_cli.main(["--target-platform-id", "darwin-arm64"]) == 0
    assert captured["foundation_env_target_platform_id"] == "darwin-arm64"
    assert captured["build_env_target_platform_id"] == "darwin-arm64"
    assert captured["manifest_target_platform_id"] == "darwin-arm64"
    assert captured["manifest_package_channel_id"] == "darwin-arm64-release"
    assert captured["manifest_relative_path"] == MANIFEST_RELATIVE_PATH
    assert captured["sanitizer_variant"] == "release"
    assert captured["reuse_primary_package_root"] is None
    assert "OBJC3C_TARGET_PLATFORM_ID" not in os.environ


def test_package_channel_public_action_forwards_release_target_platform_args(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    from scripts.objc3c_workflow.actions import (
        release_governance_packaging_artifacts as packaging_artifacts,
    )
    from scripts.objc3c_workflow.actions.release_governance_packaging_contracts import (
        PACKAGING_CHANNEL_ACTION_CONTRACTS,
    )

    captured: dict[str, list[str]] = {}

    def fake_run(command: list[str]) -> int:
        captured["command"] = [str(part) for part in command]
        return 0

    monkeypatch.setattr(packaging_artifacts, "run", fake_run)

    assert (
        packaging_artifacts.action_build_package_channels(
            ["--", "--target-platform-id", "linux-x64"]
        )
        == 0
    )

    assert captured["command"][-2:] == ["--target-platform-id", "linux-x64"]
    contract = next(
        contract
        for contract in PACKAGING_CHANNEL_ACTION_CONTRACTS
        if contract.action == "build-package-channels"
    )
    assert contract.pass_through_args is True


def test_platform_host_evidence_review_action_forwards_review_args(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    from scripts.objc3c_workflow.actions import (
        release_governance_packaging_artifacts as packaging_artifacts,
    )
    from scripts.objc3c_workflow.actions.release_governance_packaging_contracts import (
        PACKAGING_CHANNEL_ACTION_CONTRACTS,
    )

    captured: dict[str, list[str]] = {}

    def fake_run(command: list[str]) -> int:
        captured["command"] = [str(part) for part in command]
        return 0

    monkeypatch.setattr(packaging_artifacts, "run", fake_run)

    assert (
        packaging_artifacts.action_review_platform_host_evidence(
            ["--", "--platform-id", "linux-x64", "--output", "tmp/review.json"]
        )
        == 0
    )

    assert captured["command"][-4:] == [
        "--platform-id",
        "linux-x64",
        "--output",
        "tmp/review.json",
    ]
    assert "scripts\\review_objc3c_platform_host_evidence.py" in captured["command"][1] or (
        "scripts/review_objc3c_platform_host_evidence.py" in captured["command"][1]
    )
    contract = next(
        contract
        for contract in PACKAGING_CHANNEL_ACTION_CONTRACTS
        if contract.action == "review-platform-host-evidence"
    )
    assert contract.pass_through_args is True


def test_package_channel_reuse_accepts_checked_release_foundation_artifacts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    fixture = release_foundation_reuse_fixture(tmp_path)
    configure_release_foundation_reuse_fixture(monkeypatch, fixture)

    assert not fixture["integration_summary"].exists()

    package_commands.require_existing_release_foundation_artifacts()


def test_package_channel_reuse_accepts_checked_runnable_package(
    tmp_path: Path,
) -> None:
    package_root = runnable_package_reuse_fixture(tmp_path)

    assert package_commands.require_existing_runnable_package(repo_rel(package_root)) == package_root.resolve()


def test_package_channel_reuse_rejects_missing_runnable_payload_entry(
    tmp_path: Path,
) -> None:
    package_root = runnable_package_reuse_fixture(tmp_path)
    (package_root / "artifacts" / "lib" / "objc3_runtime.lib").unlink()

    with pytest.raises(RuntimeError, match="missed required payload entries"):
        package_commands.require_existing_runnable_package(repo_rel(package_root))


def test_package_channel_reuse_forwards_primary_package_root_to_release_manifest(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    package_root = runnable_package_reuse_fixture(tmp_path)
    commands: list[list[str]] = []

    monkeypatch.setattr(package_commands, "run", lambda command: commands.append(command))

    package_commands.build_release_foundation_artifacts(
        reuse_primary_package_root=package_root,
    )

    assert "--reuse-primary-package-root" in commands[0]
    assert str(package_root) in commands[0]
    assert commands[1][-1] == str(package_commands.RELEASE_PROVENANCE_PY)


def test_package_channel_cli_reuses_validated_runnable_package_without_rebuild(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    from scripts.objc3c_package_channels import cli as package_cli

    package_root = runnable_package_reuse_fixture(tmp_path)
    captured: dict[str, object] = {}
    inputs = sample_inputs()
    monkeypatch.setattr(package_cli, "package_channel_run_id", lambda: "unit-reuse-cli")
    monkeypatch.setattr(package_cli, "load_package_channel_surface_inputs", lambda: {})
    monkeypatch.setattr(
        package_cli,
        "load_package_channel_inputs",
        lambda surface_inputs: inputs,
    )
    monkeypatch.setattr(package_cli, "build_support_matrix", lambda: None)

    def fake_build_release_foundation_artifacts(
        *,
        reuse_existing: bool = False,
        reuse_primary_package_root: Path | None = None,
    ) -> None:
        captured["reuse_existing"] = reuse_existing
        captured["reuse_primary_package_root"] = reuse_primary_package_root

    def fake_prepare_package_channel_workspace(
        paths: PackageChannelPaths,
        *,
        preserve_package_root: bool = False,
    ) -> None:
        captured["workspace_package_root"] = paths.package_root
        captured["preserve_package_root"] = preserve_package_root

    def fail_build_runnable_package(*_: object, **__: object) -> None:
        raise AssertionError("reused package-channel publication must not rebuild the runnable package")

    def fake_package_channels_manifest_payload(
        *,
        inputs: PackageChannelInputs,
        paths: PackageChannelPaths,
        installer_signature: dict[str, object],
    ) -> dict[str, object]:
        captured["manifest_package_root"] = paths.package_root
        return {"platform_id": paths.target_platform_id}

    monkeypatch.setattr(
        package_cli,
        "build_release_foundation_artifacts",
        fake_build_release_foundation_artifacts,
    )
    monkeypatch.setattr(
        package_cli,
        "prepare_package_channel_workspace",
        fake_prepare_package_channel_workspace,
    )
    monkeypatch.setattr(package_cli, "build_runnable_package", fail_build_runnable_package)
    monkeypatch.setattr(package_cli, "publish_portable_archive", lambda paths: None)
    monkeypatch.setattr(package_cli, "publish_installer_archive", lambda paths: None)
    monkeypatch.setattr(package_cli, "publish_offline_bundle", lambda paths: None)
    monkeypatch.setattr(package_cli, "installer_signature_payload", lambda installer_archive: {})
    monkeypatch.setattr(
        package_cli,
        "package_channels_manifest_payload",
        fake_package_channels_manifest_payload,
    )
    monkeypatch.setattr(
        package_cli,
        "validate_manifest_required_fields",
        lambda *, manifest_payload, metadata_surface: None,
    )
    monkeypatch.setattr(
        package_cli,
        "write_package_channel_artifacts",
        lambda *, inputs, paths, manifest_payload: {},
    )
    monkeypatch.setattr(package_cli, "print_package_channel_result", lambda paths: None)

    assert package_cli.main(["--reuse-runnable-package-root", repo_rel(package_root)]) == 0
    assert captured["reuse_existing"] is False
    assert captured["reuse_primary_package_root"] == package_root.resolve()
    assert captured["workspace_package_root"] == package_root.resolve()
    assert captured["manifest_package_root"] == package_root.resolve()
    assert captured["preserve_package_root"] is True


def test_package_channel_cli_can_reuse_release_foundation_and_runnable_package(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    from scripts.objc3c_package_channels import cli as package_cli

    package_root = runnable_package_reuse_fixture(tmp_path)
    captured: dict[str, object] = {}
    inputs = sample_inputs()
    monkeypatch.setattr(package_cli, "package_channel_run_id", lambda: "unit-reuse-both-cli")
    monkeypatch.setattr(package_cli, "load_package_channel_surface_inputs", lambda: {})
    monkeypatch.setattr(
        package_cli,
        "load_package_channel_inputs",
        lambda surface_inputs: inputs,
    )
    monkeypatch.setattr(package_cli, "build_support_matrix", lambda: None)

    def fake_build_release_foundation_artifacts(
        *,
        reuse_existing: bool = False,
        reuse_primary_package_root: Path | None = None,
    ) -> None:
        captured["reuse_existing"] = reuse_existing
        captured["reuse_primary_package_root"] = reuse_primary_package_root

    def fake_prepare_package_channel_workspace(
        paths: PackageChannelPaths,
        *,
        preserve_package_root: bool = False,
    ) -> None:
        captured["workspace_package_root"] = paths.package_root
        captured["preserve_package_root"] = preserve_package_root

    def fail_build_runnable_package(*_: object, **__: object) -> None:
        raise AssertionError("combined package-channel reuse must not rebuild the runnable package")

    def fake_package_channels_manifest_payload(
        *,
        inputs: PackageChannelInputs,
        paths: PackageChannelPaths,
        installer_signature: dict[str, object],
    ) -> dict[str, object]:
        captured["manifest_package_root"] = paths.package_root
        return {"platform_id": paths.target_platform_id}

    monkeypatch.setattr(
        package_cli,
        "build_release_foundation_artifacts",
        fake_build_release_foundation_artifacts,
    )
    monkeypatch.setattr(
        package_cli,
        "prepare_package_channel_workspace",
        fake_prepare_package_channel_workspace,
    )
    monkeypatch.setattr(package_cli, "build_runnable_package", fail_build_runnable_package)
    monkeypatch.setattr(package_cli, "publish_portable_archive", lambda paths: None)
    monkeypatch.setattr(package_cli, "publish_installer_archive", lambda paths: None)
    monkeypatch.setattr(package_cli, "publish_offline_bundle", lambda paths: None)
    monkeypatch.setattr(package_cli, "installer_signature_payload", lambda installer_archive: {})
    monkeypatch.setattr(
        package_cli,
        "package_channels_manifest_payload",
        fake_package_channels_manifest_payload,
    )
    monkeypatch.setattr(
        package_cli,
        "validate_manifest_required_fields",
        lambda *, manifest_payload, metadata_surface: None,
    )
    monkeypatch.setattr(
        package_cli,
        "write_package_channel_artifacts",
        lambda *, inputs, paths, manifest_payload: {},
    )
    monkeypatch.setattr(package_cli, "print_package_channel_result", lambda paths: None)

    assert (
        package_cli.main(
            [
                "--reuse-release-foundation-artifacts",
                "--reuse-runnable-package-root",
                repo_rel(package_root),
            ]
        )
        == 0
    )
    assert captured["reuse_existing"] is True
    assert captured["reuse_primary_package_root"] is None
    assert captured["workspace_package_root"] == package_root.resolve()
    assert captured["manifest_package_root"] == package_root.resolve()
    assert captured["preserve_package_root"] is True


def test_hosted_packaging_end_to_end_derives_release_foundation_from_runnable_package(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    import scripts.check_objc3c_packaging_channels_end_to_end as packaging_e2e

    evidence_root = tmp_path / "tmp/reports/platform-host-evidence/linux-x64"
    manifest_path = evidence_root / "package/objc3c-runnable-toolchain-package.json"
    manifest_path.parent.mkdir(parents=True)
    manifest_path.write_text(
        json.dumps({"package_root": "tmp/pkg/reusable-linux-package"}) + "\n",
        encoding="utf-8",
    )
    monkeypatch.setattr(packaging_e2e, "ROOT", tmp_path)
    monkeypatch.setattr(
        packaging_e2e,
        "repo_rel",
        lambda path, **_: Path(path).resolve().relative_to(tmp_path.resolve()).as_posix(),
    )
    monkeypatch.setenv("OBJC3C_PLATFORM_ID", "linux-x64")
    monkeypatch.setenv(
        "OBJC3C_PLATFORM_EVIDENCE_ROOT",
        "tmp/reports/platform-host-evidence/linux-x64",
    )
    captured: dict[str, object] = {}

    def fake_run_capture(
        command: list[str],
        *,
        cwd: Path,
        capture_output: bool = True,
        **_: object,
    ) -> SimpleNamespace:
        captured["command"] = command
        captured["cwd"] = cwd
        captured["capture_output"] = capture_output
        return SimpleNamespace(returncode=0)

    packaging_e2e.CURRENT_END_TO_END_CONTEXT.clear()
    monkeypatch.setattr(packaging_e2e, "run_capture", fake_run_capture)

    packaging_e2e.build_package_channels_from_fresh_release_foundation()

    command_value = captured["command"]
    assert isinstance(command_value, list)
    command = [str(part) for part in command_value]
    assert "--reuse-runnable-package-root" in command
    assert "tmp/pkg/reusable-linux-package" in command
    assert "--reuse-release-foundation-artifacts" not in command
    assert "--target-platform-id" in command
    assert "linux-x64" in command
    assert captured["cwd"] == tmp_path
    assert captured["capture_output"] is False
    assert (
        packaging_e2e.CURRENT_END_TO_END_CONTEXT["hosted_runnable_package_manifest"]
        == "tmp/reports/platform-host-evidence/linux-x64/package/objc3c-runnable-toolchain-package.json"
    )


def test_runnable_package_manifest_declares_no_top_level_support_truth_claim() -> None:
    script_text = (
        ROOT / "scripts/package_objc3c_runnable_toolchain/artifact_report_foundation.psm1"
    ).read_text(encoding="utf-8")
    return_section = script_text.split('return [ordered]@{', 1)[1].split(
        'target_platform_id = $targetPlatformId',
        1,
    )[0]

    assert 'support_truth = $false' in return_section
    assert 'native_execution_claimed = $false' in return_section


def test_package_channel_reuse_rejects_release_foundation_digest_drift(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    fixture = release_foundation_reuse_fixture(tmp_path)
    configure_release_foundation_reuse_fixture(monkeypatch, fixture)
    write_json(
        fixture["attestation"],
        {
            "contract_id": "objc3c.release.foundation.attestation.v1",
            "attested_digests": {},
        },
    )

    with pytest.raises(RuntimeError, match="attestation_sha256 drifted"):
        package_commands.require_existing_release_foundation_artifacts()


def test_package_channel_manifest_and_report_are_owned_by_model() -> None:
    paths = package_channel_paths("unit-run", target_platform_id="windows-x64")
    inputs = sample_inputs()
    signature = sample_installer_signature()
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=paths,
        installer_signature=signature,
        archive_digests=sample_archive_digests(),
        payload_contract=sample_payload_contract(paths),
        receipt_contracts=sample_receipt_contracts(),
    )
    report = package_channels_report_payload(inputs=inputs, paths=paths, manifest_payload=manifest)

    assert manifest["contract_id"] == "objc3c.packaging.channels.summary.v1"
    assert manifest["platform_id"] == "windows-x64"
    assert manifest["package_id"] == "org.objc3c.runtime:objc3c-runtime-release"
    assert manifest["package_channel_id"] == "windows-x64-release"
    assert manifest["sanitizer_variant"] == "release"
    assert manifest["support_truth"] is False
    assert manifest["native_execution_claimed"] is False
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
    assert manifest["payload_contract"]["manifest_relative_path"] == MANIFEST_RELATIVE_PATH
    assert manifest["payload_contract"]["target_platform_id"] == "windows-x64"
    assert manifest["payload_contract"]["required_entries"] == REQUIRED_PAYLOAD_ENTRIES
    assert manifest["package_runtime_models"][0]["platform_id"] == "windows-x64"
    assert manifest["package_runtime_models"][0]["support_state"] == "supported"
    assert manifest["receipt_contracts"]["install_receipt"]["channel_id"] == "local-installer"
    assert "package_runtime_model" in manifest["receipt_contracts"]["install_receipt"]["required_fields"]
    assert manifest["receipt_contracts"]["install_receipt"]["target_platform_id"] == "windows-x64"
    assert manifest["receipt_contracts"]["offline_install_receipt"]["channel_id"] == "offline-bundle"
    assert manifest["receipt_contracts"]["offline_install_receipt"]["network_policy"] == "no-network"
    assert manifest["portable_archive"].endswith("objc3c-windows-x64-portable.zip")
    assert report["manifest_path"].endswith("objc3c-package-channels-manifest.json")
    assert report["implemented_channels"] == IMPLEMENTED_CHANNELS
    assert report["interop_loader_metadata"]["tamper_rejection_diagnostic"] == "O3PKG8054"
    assert report["installer_signature"]["signature_format"] == "objc3c-local-sha256-v1"
    assert report["archive_digests"]["installer_archive"]["digest_format"] == "sha256"
    assert report["payload_contract"]["clean_room_source_policy"] == "fresh-owned-tmp-root-only"
    assert report["receipt_contracts"]["offline_install_receipt"]["delegates_to"] == "local-installer"
    assert report["package_runtime_models"] == manifest["package_runtime_models"]


def test_package_channel_release_paths_and_scripts_are_platform_aware() -> None:
    paths = package_channel_paths("unit-linux", target_platform_id="linux-x64")
    linux_entries = required_payload_entries_for_platform(target_platform_id="linux-x64")
    install_text = install_script_text(target_platform_id="linux-x64")
    uninstall_text = uninstall_script_text(target_platform_id="linux-x64")

    assert paths.target_platform_id == "linux-x64"
    assert paths.package_id == release_package_id_for_platform("linux-x64")
    assert paths.package_channel_id == release_package_channel_id_for_platform("linux-x64")
    assert paths.portable_archive.name == "objc3c-linux-x64-portable.zip"
    assert linux_entries == [
        MANIFEST_RELATIVE_PATH,
        "artifacts/bin/objc3c-native",
        "artifacts/lib/libobjc3-runtime.so",
        "stdlib/workspace.json",
        "stdlib/modules/objc3.core/module.json",
        "docs/runbooks/objc3c_packaging_channels.md",
    ]
    assert '$targetPlatformId = "linux-x64"' in install_text
    assert "org.objc3c.runtime:objc3c-runtime-linux-x64-release" in install_text
    assert "artifacts/bin/objc3c-native" in install_text
    assert "artifacts/lib/libobjc3-runtime.so" in install_text
    assert '$targetPlatformId = "linux-x64"' in uninstall_text


def test_package_channel_native_executable_entry_is_manifest_bound() -> None:
    paths = package_channel_paths("unit-linux", target_platform_id="linux-x64")
    payload_contract = sample_payload_contract(paths)

    assert (
        native_executable_entry_from_runnable_manifest(
            {"native_executable": "artifacts/bin/objc3c-native"},
            expected_payload_entries=payload_contract["required_entries"],
            payload_contract=payload_contract,
        )
        == "artifacts/bin/objc3c-native"
    )


def test_package_channel_native_executable_entry_rejects_platform_name_drift() -> None:
    paths = package_channel_paths("unit-linux", target_platform_id="linux-x64")
    payload_contract = sample_payload_contract(paths)

    with pytest.raises(
        RuntimeError,
        match="native executable is not part of the expected payload",
    ):
        native_executable_entry_from_runnable_manifest(
            {"native_executable": "artifacts/bin/objc3c-native.exe"},
            expected_payload_entries=payload_contract["required_entries"],
            payload_contract=payload_contract,
        )


def test_package_channel_native_executable_entry_rejects_unsafe_manifest_path() -> None:
    with pytest.raises(RuntimeError, match="must not contain traversal segments"):
        native_executable_entry_from_runnable_manifest(
            {"native_executable": "artifacts/bin/../objc3c-native"},
        )


def test_package_channel_validation_accepts_linux_release_identity() -> None:
    paths = package_channel_paths("unit-linux", target_platform_id="linux-x64")
    inputs = sample_inputs()
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=paths,
        installer_signature=sample_installer_signature(paths),
        archive_digests=sample_archive_digests(paths),
        payload_contract=sample_payload_contract(paths),
        receipt_contracts=sample_receipt_contracts("linux-x64"),
    )

    validate_manifest_required_fields(
        manifest_payload=manifest,
        metadata_surface=inputs.metadata_surface,
    )

    assert manifest["platform_id"] == "linux-x64"
    assert manifest["package_id"] == "org.objc3c.runtime:objc3c-runtime-linux-x64-release"
    assert manifest["payload_contract"]["required_entries"][1] == "artifacts/bin/objc3c-native"
    assert (
        manifest["receipt_contracts"]["install_receipt"]["package_runtime_model"][
            "package_root_layout"
        ][2]
        == "artifacts/lib/libobjc3-runtime.so"
    )


def test_package_channel_archive_digest_payloads_are_owned_by_model(tmp_path: Path) -> None:
    build_root = ROOT / "tmp" / "tests" / "package-channel-digests" / tmp_path.name
    paths = PackageChannelPaths(
        run_id="unit-run",
        sanitizer_variant="release",
        package_id="org.objc3c.runtime:objc3c-runtime-release",
        package_channel_id="windows-x64-release",
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
        paths=package_channel_paths("unit-run", target_platform_id="windows-x64"),
        installer_signature=sample_installer_signature(),
        archive_digests=sample_archive_digests(),
        payload_contract=sample_payload_contract(),
        receipt_contracts=sample_receipt_contracts(),
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
        paths=package_channel_paths("unit-run", target_platform_id="windows-x64"),
        installer_signature={
            **sample_installer_signature(),
            "artifact": "tmp/artifacts/package-channels/unit-run/windows-x64/installer/drifted.zip",
        },
        archive_digests=sample_archive_digests(),
        payload_contract=sample_payload_contract(),
        receipt_contracts=sample_receipt_contracts(),
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
        paths=package_channel_paths("unit-run", target_platform_id="windows-x64"),
        installer_signature=sample_installer_signature(),
        archive_digests=archive_digests,
        payload_contract=sample_payload_contract(),
        receipt_contracts=sample_receipt_contracts(),
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
        paths=package_channel_paths("unit-run", target_platform_id="windows-x64"),
        installer_signature=sample_installer_signature(),
        archive_digests=archive_digests,
        payload_contract=sample_payload_contract(),
        receipt_contracts=sample_receipt_contracts(),
    )

    with pytest.raises(RuntimeError, match="installer_signature digest drifted"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_validation_fails_closed_on_payload_contract_drift() -> None:
    inputs = sample_inputs()
    payload_contract = sample_payload_contract()
    payload_contract["manifest_sha256"] = "not-a-sha256"
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=package_channel_paths("unit-run", target_platform_id="windows-x64"),
        installer_signature=sample_installer_signature(),
        archive_digests=sample_archive_digests(),
        payload_contract=payload_contract,
        receipt_contracts=sample_receipt_contracts(),
    )

    with pytest.raises(RuntimeError, match="payload_contract manifest_sha256"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_validation_fails_closed_on_receipt_contract_drift() -> None:
    inputs = sample_inputs()
    receipt_contracts = sample_receipt_contracts()
    receipt_contracts["offline_install_receipt"] = {
        **receipt_contracts["offline_install_receipt"],
        "network_policy": "network-bootstrap",
    }
    manifest = package_channels_manifest_payload(
        inputs=inputs,
        paths=package_channel_paths("unit-run", target_platform_id="windows-x64"),
        installer_signature=sample_installer_signature(),
        archive_digests=sample_archive_digests(),
        payload_contract=sample_payload_contract(),
        receipt_contracts=receipt_contracts,
    )

    with pytest.raises(RuntimeError, match="offline_install_receipt network_policy"):
        validate_manifest_required_fields(
            manifest_payload=manifest,
            metadata_surface=inputs.metadata_surface,
        )


def test_package_channel_workspace_preparation_removes_owned_stale_roots(tmp_path: Path) -> None:
    paths = package_channel_paths(
        f"unit-{tmp_path.name}",
        target_platform_id="windows-x64",
    )
    stale_package_file = paths.package_root / "stale-package.txt"
    stale_build_file = paths.build_root / "stale-build.txt"
    stale_package_file.parent.mkdir(parents=True, exist_ok=True)
    stale_build_file.parent.mkdir(parents=True, exist_ok=True)
    stale_package_file.write_text("stale package", encoding="utf-8")
    stale_build_file.write_text("stale build", encoding="utf-8")

    prepare_package_channel_workspace(paths)

    assert not paths.package_root.parent.exists()
    assert not paths.build_root.exists()


def test_package_channel_script_rendering_is_owned_by_rendering_module() -> None:
    install_text = install_script_text()
    offline_text = offline_bootstrap_script_text()

    assert "objc3c.packaging.channels.install-receipt.v1" in install_text
    assert "channel_id = $ChannelId" in install_text
    assert "bootstrap_entrypoint = \"Bootstrap-objc3cEnvironment.ps1\"" in install_text
    assert "package_bridge = \"objc3c\"" in install_text
    assert "install_command = \"npm run objc3c -- build-package-channels\"" in install_text
    assert "SanitizerVariant = \"release\"" in install_text
    assert "payload_manifest = $payloadManifest" in install_text
    assert "payload_manifest_sha256 = $payloadManifestSha256" in install_text
    assert "Copy-Item -LiteralPath $sourceRoot" in install_text
    assert "OfflineBootstrap-objc3c.ps1" not in install_text
    assert "objc3c-windows-x64-installer.zip" in offline_text
    assert "Expand-Archive -LiteralPath $installerArchive" in offline_text
    assert '-ChannelId "offline-bundle"' in offline_text
    assert '-SanitizerVariant "release"' in offline_text
