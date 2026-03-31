#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(r"C:/Users/sneak/Development/Slopjective-C")
FIXTURE_ROOT = ROOT / "tests/tooling/fixtures/objc3c"
CLASSIFICATION_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json"
OUT_DIR = ROOT / "tmp/reports/m315/M315-C003"
JSON_OUT = OUT_DIR / "synthetic_fixture_labeling_summary.json"
MD_OUT = OUT_DIR / "synthetic_fixture_labeling_summary.md"

ARTIFACT_AUTHENTICITY_SCHEMA_ID = "objc3c.artifact.authenticity.schema.v1"
REPLAY_FIXTURE_FAMILY_ID = "objc3c.fixture.synthetic.replayll.v1"
REPLAY_MANIFEST_ARTIFACT_FAMILY_ID = "objc3c.artifact.replaymanifest.frontend.v1"
REPLAY_FIXTURE_LABEL = "replay IR fixture"
REPLAY_FIXTURE_REASON = "deterministic replay artifact preserved for validation contracts"

LL_HEADER = [
    f"; authenticity_schema_id: {ARTIFACT_AUTHENTICITY_SCHEMA_ID}",
    f"; artifact_family_id: {REPLAY_FIXTURE_FAMILY_ID}",
    "; provenance_class: synthetic_fixture",
    "; provenance_mode: fixture_curated",
    "; content_role: replay_ir_fixture",
    f"; fixture_family_id: {REPLAY_FIXTURE_FAMILY_ID}",
    f"; explicit_fixture_label: {REPLAY_FIXTURE_LABEL}",
    f"; synthetic_reason: {REPLAY_FIXTURE_REASON}",
]


def normalize(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def replay_manifest_envelope() -> dict[str, str]:
    return {
        "authenticity_schema_id": ARTIFACT_AUTHENTICITY_SCHEMA_ID,
        "artifact_family_id": REPLAY_MANIFEST_ARTIFACT_FAMILY_ID,
        "provenance_class": "synthetic_fixture",
        "provenance_mode": "fixture_curated",
        "content_role": "replay_frontend_manifest",
        "fixture_family_id": REPLAY_FIXTURE_FAMILY_ID,
        "synthetic_reason": REPLAY_FIXTURE_REASON,
        "explicit_fixture_label": REPLAY_FIXTURE_LABEL,
    }


def apply_ll_headers(path: Path) -> bool:
    lines = path.read_text(encoding="utf-8").splitlines()
    filtered: list[str] = []
    for line in lines:
        stripped = line.strip()
        if any(
            stripped.startswith(prefix)
            for prefix in (
                "; authenticity_schema_id:",
                "; artifact_family_id:",
                "; provenance_class:",
                "; provenance_mode:",
                "; content_role:",
                "; fixture_family_id:",
                "; explicit_fixture_label:",
                "; synthetic_reason:",
            )
        ):
            continue
        filtered.append(line)
    insert_at = 1 if filtered and filtered[0].startswith("; ModuleID") else 0
    updated = filtered[:insert_at] + LL_HEADER + filtered[insert_at:]
    if updated != lines:
        path.write_text("\n".join(updated) + "\n", encoding="utf-8")
        return True
    return False


def apply_manifest_envelope(path: Path) -> bool:
    payload = read_json(path)
    envelope = replay_manifest_envelope()
    if payload.get("artifact_authenticity") == envelope:
        return False
    payload["artifact_authenticity"] = envelope
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return True


def parse_ll_metadata(path: Path) -> dict[str, str]:
    metadata: dict[str, str] = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if not stripped.startswith(";"):
            break
        body = stripped[1:].strip()
        if ":" not in body:
            continue
        key, value = body.split(":", 1)
        metadata[key.strip()] = value.strip()
    return metadata


def main() -> int:
    ll_paths = sorted(FIXTURE_ROOT.glob("**/replay_run_*/module.ll"))
    manifest_paths = sorted(FIXTURE_ROOT.glob("**/replay_run_*/module.manifest.json"))

    ll_changed = sum(1 for path in ll_paths if apply_ll_headers(path))
    manifest_changed = sum(1 for path in manifest_paths if apply_manifest_envelope(path))

    classification = read_json(CLASSIFICATION_PATH)
    replay_rules = {
        rule["rule_id"]: rule
        for rule in classification["classes"]["synthetic_fixture"]["rules"]
        if rule["rule_id"] in {
            "objc3c.fixture.synthetic.replayll.v1",
            "objc3c.fixture.synthetic.replaymanifest.v1",
            "objc3c.fixture.synthetic.testjson.v1",
        }
    }

    manifest_envelope = replay_manifest_envelope()
    ll_checks = []
    for path in ll_paths:
        metadata = parse_ll_metadata(path)
        ll_checks.append(
            all(
                metadata.get(key) == value
                for key, value in {
                    "authenticity_schema_id": ARTIFACT_AUTHENTICITY_SCHEMA_ID,
                    "artifact_family_id": REPLAY_FIXTURE_FAMILY_ID,
                    "provenance_class": "synthetic_fixture",
                    "provenance_mode": "fixture_curated",
                    "content_role": "replay_ir_fixture",
                    "fixture_family_id": REPLAY_FIXTURE_FAMILY_ID,
                    "explicit_fixture_label": REPLAY_FIXTURE_LABEL,
                    "synthetic_reason": REPLAY_FIXTURE_REASON,
                }.items()
            )
        )
    manifest_checks = []
    for path in manifest_paths:
        payload = read_json(path)
        manifest_checks.append(payload.get("artifact_authenticity") == manifest_envelope)

    tests_json_count = sum(1 for _ in ROOT.glob("tests/**/*.json"))
    summary = {
        "issue": "M315-C003",
        "replay_ll_count": len(ll_paths),
        "replay_manifest_count": len(manifest_paths),
        "tests_json_count": tests_json_count,
        "ll_changed_count": ll_changed,
        "manifest_changed_count": manifest_changed,
        "reference_paths": {
            "first_replay_ll": normalize(ll_paths[0]) if ll_paths else None,
            "first_replay_manifest": normalize(manifest_paths[0]) if manifest_paths else None,
        },
        "classification_status": {
            "replayll": replay_rules["objc3c.fixture.synthetic.replayll.v1"]["label_rollout_status"],
            "replaymanifest": replay_rules["objc3c.fixture.synthetic.replaymanifest.v1"]["label_rollout_status"],
            "testsjson": replay_rules["objc3c.fixture.synthetic.testjson.v1"]["label_rollout_status"],
        },
        "checks": {
            "replay_ll_rule_implemented": replay_rules["objc3c.fixture.synthetic.replayll.v1"]["label_rollout_status"] == "implemented-in-c003",
            "replay_manifest_rule_implemented": replay_rules["objc3c.fixture.synthetic.replaymanifest.v1"]["label_rollout_status"] == "implemented-in-c003",
            "tests_json_path_fenced": replay_rules["objc3c.fixture.synthetic.testjson.v1"]["label_rollout_status"] == "path-fenced-by-tests-root",
            "all_replay_ll_labeled": all(ll_checks),
            "all_replay_manifests_labeled": all(manifest_checks),
            "ll_and_manifest_counts_match": len(ll_paths) == len(manifest_paths),
        },
    }
    summary["ok"] = all(summary["checks"].values())

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M315-C003 Synthetic Fixture Labeling Summary\n\n"
        f"- Replay `.ll` fixtures: `{summary['replay_ll_count']}`\n"
        f"- Replay manifest fixtures: `{summary['replay_manifest_count']}`\n"
        f"- Tracked `tests/**/*.json`: `{summary['tests_json_count']}`\n"
        f"- Replay `.ll` files updated this run: `{summary['ll_changed_count']}`\n"
        f"- Replay manifests updated this run: `{summary['manifest_changed_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
