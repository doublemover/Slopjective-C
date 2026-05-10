from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_AUTHENTICITY_SCHEMA_ID = "objc3c.artifact.authenticity.schema.v1"
SYNTHETIC_FIXTURE_FAMILY_ID = "objc3c.fixture.synthetic.librarycliparity.v1"
SYNTHETIC_FIXTURE_LABEL = "fixture parity IR"
SYNTHETIC_FIXTURE_REASON = "fixture parity IR for CLI versus library behavior"
SYNTHETIC_MANIFEST_ARTIFACT_FAMILY_ID = "objc3c.fixture.synthetic.librarycliparity.manifest.v1"
SYNTHETIC_GOLDEN_SUMMARY_ARTIFACT_FAMILY_ID = "objc3c.fixture.synthetic.librarycliparity.golden_summary.v1"
SYNTHETIC_LL_REQUIRED_KEYS = (
    "artifact_family_id",
    "provenance_class",
    "provenance_mode",
    "fixture_family_id",
    "explicit_fixture_label",
)


def read_json(path: Path, *, label: str) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"{label} missing: {display_path(path)}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"{label} parse error at {display_path(path)}: {exc}") from exc
    if not isinstance(payload, dict):
        raise ValueError(f"{label} must be a JSON object: {display_path(path)}")
    return payload


def parse_ll_comment_metadata(path: Path) -> dict[str, str]:
    metadata: dict[str, str] = {}
    for line in path.read_text(encoding="utf-8").splitlines()[:16]:
        stripped = line.strip()
        if not stripped.startswith(";"):
            continue
        body = stripped[1:].strip()
        if ":" not in body:
            continue
        key, value = body.split(":", 1)
        metadata[key.strip()] = value.strip()
    return metadata


def synthetic_fixture_manifest_envelope() -> dict[str, str]:
    return {
        "authenticity_schema_id": ARTIFACT_AUTHENTICITY_SCHEMA_ID,
        "artifact_family_id": SYNTHETIC_MANIFEST_ARTIFACT_FAMILY_ID,
        "provenance_class": "synthetic_fixture",
        "provenance_mode": "fixture_curated",
        "content_role": "fixture_parity_manifest",
        "fixture_family_id": SYNTHETIC_FIXTURE_FAMILY_ID,
        "synthetic_reason": SYNTHETIC_FIXTURE_REASON,
        "explicit_fixture_label": SYNTHETIC_FIXTURE_LABEL,
    }


def synthetic_fixture_summary_envelope() -> dict[str, str]:
    return {
        "authenticity_schema_id": ARTIFACT_AUTHENTICITY_SCHEMA_ID,
        "artifact_family_id": SYNTHETIC_GOLDEN_SUMMARY_ARTIFACT_FAMILY_ID,
        "provenance_class": "synthetic_fixture",
        "provenance_mode": "fixture_curated",
        "content_role": "fixture_parity_golden_summary",
        "fixture_family_id": SYNTHETIC_FIXTURE_FAMILY_ID,
        "synthetic_reason": SYNTHETIC_FIXTURE_REASON,
        "explicit_fixture_label": SYNTHETIC_FIXTURE_LABEL,
    }


def synthetic_fixture_contract_payload() -> dict[str, Any]:
    return {
        "fixture_family_id": SYNTHETIC_FIXTURE_FAMILY_ID,
        "relocation_decision": "filesystem_root_retained_labeling_promoted_to_authenticity_contract",
        "required_manifest_artifact_family_id": SYNTHETIC_MANIFEST_ARTIFACT_FAMILY_ID,
        "required_ll_header_keys": list(SYNTHETIC_LL_REQUIRED_KEYS),
        "required_provenance_class": "synthetic_fixture",
        "required_provenance_mode": "fixture_curated",
        "required_explicit_fixture_label": SYNTHETIC_FIXTURE_LABEL,
    }


def validate_synthetic_fixture_contract(
    *,
    library_dir: Path,
    cli_dir: Path,
) -> tuple[bool, list[str], dict[str, Any] | None]:
    manifest_paths = [
        ("library", library_dir / "module.manifest.json"),
        ("cli", cli_dir / "module.manifest.json"),
    ]
    ll_paths = [
        ("library", library_dir / "module.ll"),
        ("cli", cli_dir / "module.ll"),
    ]

    failures: list[str] = []
    manifest_payloads: dict[str, dict[str, Any]] = {}
    ll_metadata: dict[str, dict[str, str]] = {}

    fixture_root = (ROOT / "tests" / "tooling" / "fixtures" / "native" / "library_cli_parity").resolve()

    def under_committed_fixture_root(path: Path) -> bool:
        try:
            path.resolve().relative_to(fixture_root)
            return True
        except ValueError:
            return False

    candidate = under_committed_fixture_root(library_dir) and under_committed_fixture_root(cli_dir)

    manifest_envelopes: dict[str, dict[str, Any]] = {}
    for role, path in manifest_paths:
        if not path.exists():
            continue
        try:
            payload = read_json(path, label=f"{role} manifest")
        except ValueError:
            continue
        manifest_payloads[role] = payload
        envelope = payload.get("artifact_authenticity")
        if isinstance(envelope, dict):
            manifest_envelopes[role] = envelope
            if (
                envelope.get("provenance_class") == "synthetic_fixture"
                or envelope.get("fixture_family_id") == SYNTHETIC_FIXTURE_FAMILY_ID
                or envelope.get("artifact_family_id") in {SYNTHETIC_MANIFEST_ARTIFACT_FAMILY_ID, SYNTHETIC_GOLDEN_SUMMARY_ARTIFACT_FAMILY_ID}
            ):
                candidate = True

    for role, path in ll_paths:
        if not path.exists():
            continue
        metadata = parse_ll_comment_metadata(path)
        ll_metadata[role] = metadata
        if (
            metadata.get("explicit_fixture_label") == SYNTHETIC_FIXTURE_LABEL
            or metadata.get("provenance_class") == "synthetic_fixture"
            or metadata.get("fixture_family_id") == SYNTHETIC_FIXTURE_FAMILY_ID
            or metadata.get("artifact_family_id") == SYNTHETIC_FIXTURE_FAMILY_ID
        ):
            candidate = True

    if not candidate:
        return False, [], None

    for role, path in manifest_paths:
        if not path.exists():
            failures.append(
                f"synthetic fixture authenticity mismatch: missing {role} manifest {display_path(path)}"
            )
            continue
        payload = manifest_payloads.get(role)
        if payload is None:
            try:
                payload = read_json(path, label=f"{role} manifest")
            except ValueError as exc:
                failures.append(str(exc))
                continue
            manifest_payloads[role] = payload
        envelope = payload.get("artifact_authenticity")
        if not isinstance(envelope, dict):
            failures.append(
                f"synthetic fixture authenticity mismatch: {display_path(path)} must define artifact_authenticity"
            )
            continue
        expected = synthetic_fixture_manifest_envelope()
        for key, value in expected.items():
            if envelope.get(key) != value:
                failures.append(
                    f"synthetic fixture authenticity mismatch: {display_path(path)} field {key!r} expected {value!r}, observed {envelope.get(key)!r}"
                )

    if len(manifest_payloads) == 2:
        library_envelope = manifest_payloads["library"].get("artifact_authenticity")
        cli_envelope = manifest_payloads["cli"].get("artifact_authenticity")
        if library_envelope != cli_envelope:
            failures.append(
                "synthetic fixture authenticity mismatch: library and cli manifest envelopes must match exactly"
            )

    for role, path in ll_paths:
        if not path.exists():
            failures.append(
                f"synthetic fixture authenticity mismatch: missing {role} ll artifact {display_path(path)}"
            )
            continue
        metadata = ll_metadata.get(role)
        if metadata is None:
            metadata = parse_ll_comment_metadata(path)
        ll_metadata[role] = metadata
        expected_values = {
            "artifact_family_id": SYNTHETIC_FIXTURE_FAMILY_ID,
            "provenance_class": "synthetic_fixture",
            "provenance_mode": "fixture_curated",
            "fixture_family_id": SYNTHETIC_FIXTURE_FAMILY_ID,
            "explicit_fixture_label": SYNTHETIC_FIXTURE_LABEL,
        }
        for key in SYNTHETIC_LL_REQUIRED_KEYS:
            if key not in metadata:
                failures.append(
                    f"synthetic fixture authenticity mismatch: {display_path(path)} missing ll authenticity key {key!r}"
                )
                continue
            if metadata.get(key) != expected_values[key]:
                failures.append(
                    f"synthetic fixture authenticity mismatch: {display_path(path)} ll authenticity key {key!r} expected {expected_values[key]!r}, observed {metadata.get(key)!r}"
                )

    if len(ll_metadata) == 2 and ll_metadata["library"] != ll_metadata["cli"]:
        failures.append(
            "synthetic fixture authenticity mismatch: library and cli ll authenticity headers must match exactly"
        )

    details = {
        "applied": True,
        "fixture_family_id": SYNTHETIC_FIXTURE_FAMILY_ID,
        "manifest_paths": [display_path(path) for _, path in manifest_paths],
        "ll_paths": [display_path(path) for _, path in ll_paths],
        "validated_ll_header_keys": list(SYNTHETIC_LL_REQUIRED_KEYS),
        "manifest_artifact_family_id": SYNTHETIC_MANIFEST_ARTIFACT_FAMILY_ID,
        "summary_artifact_family_id": SYNTHETIC_GOLDEN_SUMMARY_ARTIFACT_FAMILY_ID,
    }
    return True, failures, details
