from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "scripts"
if str(SCRIPTS) not in sys.path:
    sys.path.insert(0, str(SCRIPTS))

from conformance_corpus_surface_model import (
    ConformanceCorpusPaths,
    ConformanceCorpusSurfaceModel,
    SurfaceValidationError,
)


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def _build_model(
    root: Path,
    *,
    retained_suite: dict[str, Any],
    manifest: dict[str, Any],
) -> ConformanceCorpusSurfaceModel:
    _write_json(
        root / "tests" / "conformance" / "longitudinal_suites.json",
        {
            "contract_id": "objc3c.conformance.longitudinal_suites.v1",
            "schema_version": 1,
            "retained_suites": [retained_suite],
        },
    )
    _write_json(
        root / "tests" / "conformance" / "parser" / "manifest.json",
        manifest,
    )
    return ConformanceCorpusSurfaceModel(ConformanceCorpusPaths(root))


def _surface() -> dict[str, Any]:
    return {
        "longitudinal_policy": {
            "suite_manifest": "tests/conformance/longitudinal_suites.json",
            "retained_suite_classes": ["profile-minimum-basis"],
        }
    }


def _manifest_inventory() -> dict[str, str]:
    return {"parser": "tests/conformance/parser/manifest.json"}


def _retained_suite(traceability_targets: list[Any]) -> dict[str, Any]:
    return {
        "suite_id": "core-profile-basis",
        "suite_class": "profile-minimum-basis",
        "bucket": "parser",
        "manifest": "tests/conformance/parser/manifest.json",
        "traceability_targets": traceability_targets,
    }


def _manifest() -> dict[str, Any]:
    return {
        "groups": [
            {
                "name": "issue_48_cli_mode_selection",
                "issue": 48,
                "issues": [49],
                "files": [
                    "TUV-01.json",
                    "nested/M16-A001.json",
                ],
            }
        ]
    }


def test_longitudinal_traceability_targets_resolve_to_manifest_evidence(
    tmp_path: Path,
) -> None:
    model = _build_model(
        tmp_path,
        retained_suite=_retained_suite(
            ["issue_48_cli_mode_selection", "TUV-01", "M16-A001", "#49"]
        ),
        manifest=_manifest(),
    )

    retained = model.validate_longitudinal_suites(
        _surface(),
        _manifest_inventory(),
    )

    assert retained[0].traceability_targets == (
        "issue_48_cli_mode_selection",
        "TUV-01",
        "M16-A001",
        "#49",
    )


def test_longitudinal_traceability_rejects_unknown_manifest_target(
    tmp_path: Path,
) -> None:
    model = _build_model(
        tmp_path,
        retained_suite=_retained_suite(["TUV-01", "MISSING-CLAIM"]),
        manifest=_manifest(),
    )

    with pytest.raises(SurfaceValidationError, match="MISSING-CLAIM"):
        model.validate_longitudinal_suites(_surface(), _manifest_inventory())


def test_longitudinal_traceability_rejects_manifest_bucket_drift(
    tmp_path: Path,
) -> None:
    retained_suite = _retained_suite(["TUV-01"])
    retained_suite["manifest"] = "tests/conformance/alternate/manifest.json"
    model = _build_model(
        tmp_path,
        retained_suite=retained_suite,
        manifest=_manifest(),
    )

    with pytest.raises(SurfaceValidationError, match="manifest drifted"):
        model.validate_longitudinal_suites(_surface(), _manifest_inventory())


def test_longitudinal_traceability_rejects_duplicate_targets(
    tmp_path: Path,
) -> None:
    model = _build_model(
        tmp_path,
        retained_suite=_retained_suite(["TUV-01", "TUV-01"]),
        manifest=_manifest(),
    )

    with pytest.raises(SurfaceValidationError, match="duplicate traceability targets"):
        model.validate_longitudinal_suites(_surface(), _manifest_inventory())
