from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[1]
RUNNER_EXE = REPO_ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE_PATH = REPO_ROOT / "tests" / "tooling" / "fixtures" / "native" / "cleanup_replay_provenance_positive.objc3"
RESULT_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_c003_genuine_replay_artifact_regeneration_and_provenance_capture_core_feature_implementation_result.json"
)
SCHEMA_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_c002_artifact_authenticity_schema_and_evidence_contract_and_architecture_freeze_schema.json"
)
BRIDGE_SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C003"
    / "legacy_replay_bridge_registry.json"
)
LIVE_OUT_DIR = REPO_ROOT / "tmp" / "reports" / "m315" / "M315-C003" / "live_replay_positive"
LIVE_ENVELOPE_PATH = LIVE_OUT_DIR / "artifact_authenticity.json"
SUMMARY_PATH = (
    REPO_ROOT / "tmp" / "reports" / "m315" / "M315-C003" / "genuine_replay_provenance_capture_summary.json"
)


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def tracked_replay_paths() -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", "tests/tooling/fixtures/objc3c/**/replay_run_*/module.ll"],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
      raise RuntimeError(result.stderr)
    return [line.replace("\\", "/") for line in result.stdout.splitlines() if line.strip()]


def has_frontend_header(path: Path) -> bool:
    header = "\n".join(path.read_text(encoding="utf-8").splitlines()[:5]).lower()
    return "objc3c native frontend ir" in header


def validation_test_path_for(replay_path: str) -> str:
    fixture_root = Path(replay_path).parents[1].name
    return f"tests/tooling/test_objc3c_{fixture_root}.py"


def build_bridge_registry(result: dict) -> dict:
    replay_paths = tracked_replay_paths()
    entries = []
    with_header = 0
    without_header = 0
    for replay_path in replay_paths:
        replay_abs = REPO_ROOT / replay_path
        header_present = has_frontend_header(replay_abs)
        with_header += int(header_present)
        without_header += int(not header_present)
        validation_test = validation_test_path_for(replay_path)
        manifest_path = replay_path.replace("/module.ll", "/module.manifest.json")
        entries.append(
            {
                "artifact_path": replay_path,
                "manifest_path": manifest_path,
                "artifact_family_id": result["legacy_replay_bridge_contract"]["artifact_family_id"],
                "provenance_class": result["legacy_replay_bridge_contract"]["provenance_class"],
                "provenance_mode": result["legacy_replay_bridge_contract"]["provenance_mode"],
                "content_role": result["legacy_replay_bridge_contract"]["content_role"],
                "migration_owner_issue": result["legacy_replay_bridge_contract"]["migration_owner_issue"],
                "allowed_root_glob": result["legacy_replay_bridge_contract"]["allowed_root_glob"],
                "bridge_reason": (
                    "legacy replay artifact lacks full authenticity envelope and regeneration recipe"
                    if header_present
                    else "legacy replay artifact lacks authenticity header, full envelope, and regeneration recipe"
                ),
                "header_present": header_present,
                "validation_test_path": validation_test,
                "validation_recipe": f"python -m pytest {validation_test} -q",
            }
        )
    registry = {
        "artifact_family_id": result["legacy_replay_bridge_contract"]["artifact_family_id"],
        "provenance_class": result["legacy_replay_bridge_contract"]["provenance_class"],
        "provenance_mode": result["legacy_replay_bridge_contract"]["provenance_mode"],
        "entries_total": len(entries),
        "entries_with_frontend_header": with_header,
        "entries_without_frontend_header": without_header,
        "entries": entries,
    }
    BRIDGE_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    BRIDGE_SUMMARY_PATH.write_text(json.dumps(registry, indent=2) + "\n", encoding="utf-8")
    return registry


def run_live_positive(result: dict) -> dict:
    LIVE_OUT_DIR.mkdir(parents=True, exist_ok=True)
    command = [
        str(RUNNER_EXE.resolve()),
        str(FIXTURE_PATH.resolve()),
        "--out-dir",
        str(LIVE_OUT_DIR.resolve()),
        "--emit-prefix",
        "module",
        "--no-emit-object",
    ]
    completed = subprocess.run(
        command,
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout)

    ir_path = LIVE_OUT_DIR / "module.ll"
    manifest_path = LIVE_OUT_DIR / "module.manifest.json"
    summary_path = LIVE_OUT_DIR / "module.c_api_summary.json"
    ir_text = ir_path.read_text(encoding="utf-8")
    header_present = has_frontend_header(ir_path)

    envelope = {
        "artifact_authenticity": {
            "authenticity_schema_id": "objc3c.artifact.authenticity.schema.v1",
            "artifact_family_id": result["live_generated_replay_contract"]["artifact_family_id"],
            "provenance_class": result["live_generated_replay_contract"]["provenance_class"],
            "provenance_mode": result["live_generated_replay_contract"]["provenance_mode"],
            "content_role": result["live_generated_replay_contract"]["content_role"],
            "generator_surface_id": result["live_generated_replay_contract"]["generator_surface_id"],
            "regeneration_recipe": "artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/cleanup_replay_provenance_positive.objc3 --out-dir tmp/reports/m315/M315-C003/live_replay_positive --emit-prefix module --no-emit-object",
            "input_artifact_paths": [
                result["live_generated_replay_contract"]["fixture_path"]
            ],
            "output_path": "tmp/reports/m315/M315-C003/live_replay_positive/module.ll",
            "manifest_path": "tmp/reports/m315/M315-C003/live_replay_positive/module.manifest.json",
            "runner_summary_path": "tmp/reports/m315/M315-C003/live_replay_positive/module.c_api_summary.json"
        }
    }
    LIVE_ENVELOPE_PATH.write_text(json.dumps(envelope, indent=2) + "\n", encoding="utf-8")
    return {
        "command": command,
        "ir_path": str(ir_path.relative_to(REPO_ROOT)).replace("\\", "/"),
        "manifest_path": str(manifest_path.relative_to(REPO_ROOT)).replace("\\", "/"),
        "runner_summary_path": str(summary_path.relative_to(REPO_ROOT)).replace("\\", "/"),
        "header_present": header_present,
        "ir_contains_main": "define i32 @main()" in ir_text,
    }


def main() -> int:
    result = read_json(RESULT_PATH)
    schema = read_json(SCHEMA_PATH)
    errors: list[str] = []

    if not RUNNER_EXE.exists():
        print(f"ERROR: missing runner executable: {RUNNER_EXE}", file=sys.stderr)
        return 1
    if not FIXTURE_PATH.exists():
        print(f"ERROR: missing positive fixture: {FIXTURE_PATH}", file=sys.stderr)
        return 1

    bridge_registry = build_bridge_registry(result)
    live_capture = run_live_positive(result)
    live_envelope = read_json(LIVE_ENVELOPE_PATH)["artifact_authenticity"]

    for entry in bridge_registry["entries"]:
        validation_test = REPO_ROOT / entry["validation_test_path"]
        if not validation_test.exists():
            errors.append(f"missing validation test for bridge entry: {entry['validation_test_path']}")
        if entry["provenance_class"] != "legacy_generated_replay_bridge":
            errors.append(f"bridge entry drifted provenance class: {entry['artifact_path']}")

    expected_counts = result["legacy_replay_bridge_counts"]
    if bridge_registry["entries_total"] != expected_counts["total"]:
        errors.append("legacy replay bridge total drifted")
    if bridge_registry["entries_with_frontend_header"] != expected_counts["with_frontend_header"]:
        errors.append("legacy replay bridge header-present count drifted")
    if bridge_registry["entries_without_frontend_header"] != expected_counts["without_frontend_header"]:
        errors.append("legacy replay bridge header-missing count drifted")

    if live_envelope["provenance_class"] != "generated_replay":
        errors.append("live replay envelope must be generated_replay")
    if live_envelope["provenance_mode"] != "tool_generated_regenerable":
        errors.append("live replay envelope must be tool_generated_regenerable")
    if not live_envelope["generator_surface_id"]:
        errors.append("live replay envelope missing generator_surface_id")
    if not live_envelope["regeneration_recipe"]:
        errors.append("live replay envelope missing regeneration_recipe")
    if not live_capture["header_present"]:
        errors.append("live replay IR is missing frontend header")
    if not live_capture["ir_contains_main"]:
        errors.append("live replay IR missing main definition")

    if "generated_replay" not in schema["proof_rules"]["proof_eligible_classes"]:
        errors.append("C002 proof-eligible class drifted")

    summary = {
        "legacy_bridge_total": bridge_registry["entries_total"],
        "legacy_bridge_with_frontend_header": bridge_registry["entries_with_frontend_header"],
        "legacy_bridge_without_frontend_header": bridge_registry["entries_without_frontend_header"],
        "live_generated_replay": live_capture,
        "live_envelope_path": str(LIVE_ENVELOPE_PATH.relative_to(REPO_ROOT)).replace("\\", "/"),
        "bridge_registry_path": str(BRIDGE_SUMMARY_PATH.relative_to(REPO_ROOT)).replace("\\", "/"),
        "ok": not errors,
        "errors": errors,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        print(f"Summary written to {SUMMARY_PATH}", file=sys.stderr)
        return 1

    print("M315-C003 replay provenance capture summary:")
    print(json.dumps(summary, indent=2))
    print(f"Summary written to {SUMMARY_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
