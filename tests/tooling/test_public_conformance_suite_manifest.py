from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from types import ModuleType
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file

SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_public_conformance_suite_manifest.py"


def load_checker() -> ModuleType:
    spec = importlib.util.spec_from_file_location(
        "check_objc3c_public_conformance_suite_manifest",
        SCRIPT_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load public suite checker")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def load_manifest() -> dict[str, Any]:
    return load_json_object(ROOT / "tests" / "conformance" / "public_suite_manifest.json")


def write_manifest(path: Path, manifest: dict[str, Any]) -> None:
    write_json_file(path, manifest, sort_keys=False)


def test_public_conformance_suite_manifest_passes_and_reports_public_taxonomy() -> None:
    checker = load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "public-suite-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0
        summary = load_json_object(checker.SUMMARY_PATH)
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)

    assert summary["contract_id"] == "objc3c.public_conformance_suite.summary.v1"
    assert summary["status"] == "PASS"
    assert summary["case_count"] == 8
    assert summary["phase_count"] == 8
    assert summary["profile_count"] == 3
    assert summary["packageable"] is True
    assert summary["phase_case_counts"]["release_candidate"] == 1
    assert "npm run objc3c -- validate-release-candidate-conformance" in summary["public_commands"]
    assert "objc3c.behavior.parser.canonical-syntax" in summary["support_claims"]


def test_public_conformance_suite_manifest_rejects_compatibility_mode(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["strict_rejection_policy"]["compatibility_mode_allowed"] = True

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_missing_capability_pair(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["support_claim"] = "objc3c.behavior.parser.unowned-public-claim"

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_nonpublic_command(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["runnable_command"] = "python scripts/private_runner.py"

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
