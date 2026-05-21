from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts.check_objc3c_platform_support_matrix import (
    SOURCE_TRUTH_PATH,
    validate_platform_support_source_truth,
)

ROOT = Path(__file__).resolve().parents[2]


def load_source_truth() -> dict:
    return json.loads(SOURCE_TRUTH_PATH.read_text(encoding="utf-8"))


def write_source_truth(tmp_path: Path, payload: dict) -> Path:
    path = tmp_path / "source_truth_matrix.json"
    path.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")
    return path


def test_platform_support_source_truth_validates_checked_matrix() -> None:
    summary = validate_platform_support_source_truth()

    assert summary["status"] == "PASS"
    assert summary["issue"] == "OBJ3-NEXT-024"
    assert summary["supported_platform_ids"] == ["windows-x64"]
    assert summary["unsupported_platform_ids"] == ["darwin-arm64", "linux-x64"]
    assert summary["required_supported_evidence_classes"] == [
        "build",
        "package",
        "install",
        "execution",
    ]
    assert summary["required_auxiliary_evidence_classes"] == [
        "toolchain",
        "hosted_ci",
        "clean_room",
    ]
    assert summary["required_toolchain_components"] == [
        "llvm",
        "clang",
        "cmake",
        "ninja",
        "python",
        "node",
        "pwsh",
    ]


def test_platform_support_source_truth_rejects_source_only_support_row(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["supported_rows"][0]["required_evidence"].pop("package")

    with pytest.raises(Exception, match="required property|schema validation"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_unregistered_public_command(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["supported_rows"][0]["public_replay_commands"].append(
        "npm run objc3c -- invented-platform-support"
    )

    with pytest.raises(RuntimeError, match="not in ACTION_SPECS"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_unsupported_host_widening(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["unsupported_rows"][0]["platform_id"] = "windows-x64"

    with pytest.raises(RuntimeError, match="both supported and unsupported"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_generated_source_truth(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    upstream_path = tmp_path / "platform_toolchain_support_evidence.json"
    upstream = json.loads(
        (ROOT / payload["upstream_sources"]["platform_toolchain_support_evidence"]).read_text(
            encoding="utf-8"
        )
    )
    upstream["evidence_records"][0]["source_paths"] = [
        "tmp/reports/platform-support/generated-source.json"
    ]
    upstream_path.write_text(json.dumps(upstream, indent=2, sort_keys=True), encoding="utf-8")
    payload["upstream_sources"]["platform_toolchain_support_evidence"] = str(upstream_path)

    with pytest.raises(RuntimeError, match="used generated output as source truth"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))
