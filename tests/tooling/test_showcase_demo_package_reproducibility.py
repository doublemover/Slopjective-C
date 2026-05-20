from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts.objc3c_runnable_showcase_e2e.manifest import (
    validate_showcase_demo_packages,
)

ROOT = Path(__file__).resolve().parents[2]


def _demo_payload() -> dict[str, object]:
    return json.loads((ROOT / "showcase" / "demo_packages.json").read_text(encoding="utf-8"))


def _stage_demo_package_root(tmp_path: Path, payload: dict[str, object]) -> Path:
    package_root = tmp_path / "pkg"
    demo_manifest = package_root / "showcase" / "demo_packages.json"
    demo_manifest.parent.mkdir(parents=True, exist_ok=True)
    demo_manifest.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    for package in payload["packages"]:  # type: ignore[index]
        assert isinstance(package, dict)
        for key in ("source", "workspace_manifest"):
            path = package_root / str(package[key])
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("{}\n", encoding="utf-8")
    return package_root


def _showcase_examples(payload: dict[str, object]) -> list[dict[str, object]]:
    return [
        {
            "example_id": package["example_id"],
            "source": package["source"],
            "workspace_manifest": package["workspace_manifest"],
            "expected_exit_code": package["expected_exit_code"],
        }
        for package in payload["packages"]  # type: ignore[index]
        if isinstance(package, dict)
    ]


def test_packaged_showcase_demo_packages_accept_checked_in_replay_inputs(
    tmp_path: Path,
) -> None:
    payload = _demo_payload()
    package_root = _stage_demo_package_root(tmp_path, payload)

    packages = validate_showcase_demo_packages(
        package_root=package_root,
        demo_packages_manifest=package_root / "showcase" / "demo_packages.json",
        manifest_demo_packages=payload["packages"],
        showcase_examples=_showcase_examples(payload),
    )

    assert packages == payload["packages"]


def test_packaged_showcase_demo_packages_reject_tmp_source_of_truth(
    tmp_path: Path,
) -> None:
    payload = deepcopy(_demo_payload())
    packages = payload["packages"]
    assert isinstance(packages, list)
    assert isinstance(packages[0], dict)
    packages[0]["manifest_inputs"] = [*packages[0]["manifest_inputs"], "tmp/drift.json"]
    package_root = _stage_demo_package_root(tmp_path, payload)

    with pytest.raises(RuntimeError, match="checked-in paths"):
        validate_showcase_demo_packages(
            package_root=package_root,
            demo_packages_manifest=package_root / "showcase" / "demo_packages.json",
            manifest_demo_packages=payload["packages"],
            showcase_examples=_showcase_examples(payload),
        )


def test_runnable_toolchain_package_manifest_stages_showcase_demo_packages() -> None:
    docs_showcase = (
        ROOT
        / "scripts"
        / "objc3c_runnable_toolchain_package_helpers"
        / "manifest_provenance"
        / "docs_showcase.psm1"
    ).read_text(encoding="utf-8")
    artifact_io = (
        ROOT / "scripts" / "package_objc3c_runnable_toolchain" / "artifact_report_io.psm1"
    ).read_text(encoding="utf-8")
    artifact_surfaces = (
        ROOT
        / "scripts"
        / "package_objc3c_runnable_toolchain"
        / "artifact_report_surfaces.psm1"
    ).read_text(encoding="utf-8")

    assert '"showcase/demo_packages.json"' in docs_showcase
    assert 'ShowcaseDemoPackagesRelativePath = $showcaseDemoPackagesRelativePath' in artifact_io
    assert 'showcase_demo_packages_manifest = $showcaseDemoPackagesRelativePath' in artifact_surfaces
    assert 'showcase_demo_packages = $showcaseDemoPackagesPayload["packages"]' in artifact_surfaces
