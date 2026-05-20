from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_objc3c_package_registry_mirror_reproducibility import (  # noqa: E402
    collect_interop_loader_metadata_failures,
    collect_offline_mirror_cache_failures,
    expected_cache_payload,
    file_digest,
    stable_digest,
)
from package_ecosystem_contracts import (  # noqa: E402
    PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    load_package_loader_interop_metadata,
    normalize_package_loader_interop_metadata,
    package_loader_metadata_by_package,
    package_loader_metadata_channel_summary,
)


def normalized_interop_metadata() -> dict[str, dict[str, object]]:
    payload = load_package_loader_interop_metadata(ROOT)
    by_package = package_loader_metadata_by_package(
        payload,
        root=ROOT,
        package_ids={"showcase:patchKit", "stdlib:objc3.system"},
    )
    return {
        package_id: normalize_package_loader_interop_metadata(entry)
        for package_id, entry in by_package.items()
    }


def test_package_loader_interop_fixture_covers_headers_abi_and_mixed_images() -> None:
    metadata = normalized_interop_metadata()

    patchkit = metadata["showcase:patchKit"]
    assert "ParserKit/ForeignEntry.h" in patchkit["header_imports"]
    assert "ParserKit/ParserKit.h" in patchkit["header_exports"]
    assert patchkit["abi_alignment"][0]["bytes"] == 16
    assert {image["metadata_section"] for image in patchkit["mixed_images"]} == {
        "__objc3c_interop",
        "__objc3c_package",
    }
    assert {surface["bridge_kind"] for surface in patchkit["bridge_surfaces"]} == {
        "objcxx",
        "swift",
    }
    assert any(
        surface["bridge_kind"] == "objcxx"
        and surface["metadata_name"] == "CppBridge"
        and surface["header"] == "Bridge.hpp"
        for surface in patchkit["bridge_surfaces"]
    )
    assert any(
        surface["bridge_kind"] == "swift"
        and surface["metadata_name"] == "SwiftBridge"
        and surface["swift_visibility"] == "private"
        for surface in patchkit["bridge_surfaces"]
    )
    assert patchkit["tamper_rejection"]["diagnostic_code"] == PACKAGE_LOADER_INTEROP_TAMPER_CODE

    system = metadata["stdlib:objc3.system"]
    assert "BridgeProvider.h" in system["header_imports"]
    assert system["negative_diagnostics"][0]["diagnostic_code"] == "O3PKG8052"
    assert {diagnostic["rejection_kind"] for package in metadata.values() for diagnostic in package["negative_diagnostics"]} == {
        "conflicting",
        "malformed",
        "unsafe",
        "unsupported",
    }


def test_package_loader_interop_channel_summary_is_distribution_ready() -> None:
    payload = load_package_loader_interop_metadata(ROOT)
    by_package = package_loader_metadata_by_package(
        payload,
        root=ROOT,
        package_ids={"showcase:patchKit", "stdlib:objc3.system"},
    )

    summary = package_loader_metadata_channel_summary(payload, by_package)

    assert summary["support"] == "local-mixed-image-metadata-digest-checked"
    assert summary["package_ids"] == ["showcase:patchKit", "stdlib:objc3.system"]
    assert summary["header_import_count"] == 5
    assert summary["header_export_count"] == 3
    assert summary["abi_alignment_count"] == 2
    assert summary["foreign_type_count"] == 2
    assert summary["mixed_image_count"] == 3
    assert summary["bridge_surface_count"] == 4
    assert summary["objcxx_bridge_surface_count"] == 2
    assert summary["swift_bridge_surface_count"] == 2
    assert summary["positive_fixture_count"] == 5
    assert summary["negative_fixture_count"] == 4
    assert summary["tamper_rejection_diagnostic"] == PACKAGE_LOADER_INTEROP_TAMPER_CODE
    assert "network-resolved interop metadata" in summary["unsupported_surfaces"]


def test_package_loader_interop_metadata_digest_is_replay_stable() -> None:
    first = normalized_interop_metadata()
    second = normalized_interop_metadata()

    assert {
        package_id: metadata["digest"]
        for package_id, metadata in first.items()
    } == {
        package_id: metadata["digest"]
        for package_id, metadata in second.items()
    }


def test_package_loader_interop_tamper_mismatch_reports_stable_code() -> None:
    metadata = normalized_interop_metadata()
    lock = {
        "packages": [
            {
                "package_id": "showcase:patchKit",
                "interop_loader_metadata": metadata["showcase:patchKit"],
            }
        ]
    }
    mirror_metadata = deepcopy(metadata["showcase:patchKit"])
    mirror_metadata["digest"] = "sha256:" + ("0" * 64)
    mirror = {
        "integrity_policy": {
            "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        },
        "packages": [
            {
                "package_id": "showcase:patchKit",
                "interop_loader_metadata": mirror_metadata,
            }
        ],
    }
    registry = deepcopy(lock)
    publication = {
        "interop_loader_support": "local-mixed-image-metadata-digest-checked",
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    }

    failures = collect_interop_loader_metadata_failures(lock, mirror, registry, publication)

    assert failures == [
        f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror interop metadata digest mismatch for showcase:patchKit"
    ]


def test_package_loader_interop_payload_tamper_with_preserved_digest_reports_stable_code() -> None:
    metadata = normalized_interop_metadata()
    lock = {
        "packages": [
            {
                "package_id": "showcase:patchKit",
                "interop_loader_metadata": metadata["showcase:patchKit"],
            }
        ]
    }
    mirror_metadata = deepcopy(metadata["showcase:patchKit"])
    mirror_metadata["header_exports"] = ["Tampered/PatchKit.h"]
    mirror = {
        "integrity_policy": {
            "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        },
        "packages": [
            {
                "package_id": "showcase:patchKit",
                "interop_loader_metadata": mirror_metadata,
            }
        ],
    }
    registry = deepcopy(lock)
    publication = {
        "interop_loader_support": "local-mixed-image-metadata-digest-checked",
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    }

    failures = collect_interop_loader_metadata_failures(lock, mirror, registry, publication)

    assert failures == [
        f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror interop metadata payload mismatch for showcase:patchKit"
    ]


def test_offline_mirror_cache_payload_tamper_reports_stable_code(tmp_path: Path) -> None:
    source_path = tmp_path / "fixtures" / "Pkg.json"
    source_path.parent.mkdir(parents=True)
    source_path.write_text("{}\n", encoding="utf-8")
    mirror_package = {
        "package_id": "fixture:Pkg",
        "source": "fixtures/Pkg.json",
        "source_digest": file_digest(source_path),
        "cache_path": "tmp/artifacts/package-ecosystem/mirrors/cache/fixture/Pkg.json",
    }
    cache_payload = expected_cache_payload(mirror_package)
    mirror_package["cache_digest"] = stable_digest(cache_payload)
    cache_path = tmp_path / mirror_package["cache_path"]
    cache_path.parent.mkdir(parents=True)
    tampered_payload = dict(cache_payload)
    tampered_payload["source_digest"] = "sha256:" + ("0" * 64)
    cache_path.write_text(
        json.dumps(tampered_payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    failures = collect_offline_mirror_cache_failures(
        {"packages": [mirror_package]},
        root=tmp_path,
        cache_root=tmp_path / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "cache",
    )

    assert failures == [
        f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror cache payload mismatch for fixture:Pkg"
    ]
