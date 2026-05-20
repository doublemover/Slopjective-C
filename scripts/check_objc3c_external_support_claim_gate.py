#!/usr/bin/env python3
"""Fail-closed gate for external-validation-backed support and adoption claims."""

from __future__ import annotations

import argparse
import hashlib
import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "external_validation"
REPRO_CORPUS = FIXTURE_ROOT / "repro_corpus.json"
CLAIM_GATE = FIXTURE_ROOT / "support_claim_gate.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "claim-gate-summary.json"

CORPUS_CONTRACT_ID = "objc3c.external_validation.repro_corpus.v1"
GATE_CONTRACT_ID = "objc3c.external_validation.support_claim_gate.v1"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.support_claim_gate.summary.v1"
TRUST_POLICY_CONTRACT_ID = "objc3c.external_validation.trust.policy.v1"
INTAKE_MANIFEST_CONTRACT_ID = "objc3c.external_validation.intake.manifest.v1"
ADOPTION_POLICY_CONTRACT_ID = "objc3c.adoption_legibility.public_claim_policy.v1"
SCHEMA_VERSION = 1

DIAG_SCHEMA = "OBJC3-EXTERNAL-CLAIM-GATE-SCHEMA"
DIAG_MISSING = "OBJC3-EXTERNAL-CLAIM-GATE-MISSING-EVIDENCE"
DIAG_STALE = "OBJC3-EXTERNAL-CLAIM-GATE-STALE-EVIDENCE"
DIAG_NON_REPRODUCIBLE = "OBJC3-EXTERNAL-CLAIM-GATE-NON-REPRODUCIBLE"
DIAG_LOCAL_ONLY = "OBJC3-EXTERNAL-CLAIM-GATE-LOCAL-ONLY"
DIAG_CLAIM_BINDING = "OBJC3-EXTERNAL-CLAIM-GATE-CLAIM-BINDING"

CLAIM_KINDS = {"support", "adoption"}
FORBIDDEN_ROOTS = ("tmp", "temp")


class ClaimGateFailure(RuntimeError):
    def __init__(self, diagnostic_code: str, message: str) -> None:
        super().__init__(message)
        self.diagnostic_code = diagnostic_code


def fail(diagnostic_code: str, message: str) -> None:
    raise ClaimGateFailure(diagnostic_code, message)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def require_object(payload: Any, label: str) -> dict[str, Any]:
    if not isinstance(payload, dict):
        fail(DIAG_SCHEMA, f"{label} must be an object")
    return payload


def require_contract(payload: dict[str, Any], *, contract_id: str, label: str) -> None:
    if payload.get("contract_id") != contract_id:
        fail(DIAG_SCHEMA, f"{label} contract_id drifted")
    if payload.get("schema_version") != SCHEMA_VERSION:
        fail(DIAG_SCHEMA, f"{label} schema_version drifted")


def require_sorted_unique(values: list[str], *, label: str) -> None:
    if values != sorted(values):
        fail(DIAG_SCHEMA, f"{label} must be sorted")
    if len(values) != len(set(values)):
        fail(DIAG_SCHEMA, f"{label} must be unique")


def require_repo_relative(raw_path: str, *, field_name: str) -> Path:
    if not isinstance(raw_path, str) or not raw_path:
        fail(DIAG_MISSING, f"{field_name} is missing")
    normalized = raw_path.replace("\\", "/")
    candidate = Path(normalized)
    if candidate.is_absolute() or ".." in candidate.parts:
        fail(DIAG_LOCAL_ONLY, f"{field_name} must be a repo-relative path: {raw_path}")
    first_part = normalized.split("/", 1)[0]
    if first_part in FORBIDDEN_ROOTS:
        fail(DIAG_LOCAL_ONLY, f"{field_name} cannot use local-only root {first_part}: {raw_path}")
    return ROOT / normalized


def require_existing_repo_file(raw_path: str, *, field_name: str) -> Path:
    path = require_repo_relative(raw_path, field_name=field_name)
    if not path.is_file():
        fail(DIAG_MISSING, f"{field_name} does not exist: {raw_path}")
    return path


def require_path_hash(raw_path: str, expected_sha256: str, *, field_name: str) -> Path:
    path = require_existing_repo_file(raw_path, field_name=field_name)
    if not isinstance(expected_sha256, str) or not expected_sha256:
        fail(DIAG_STALE, f"{field_name} is missing sha256 for {raw_path}")
    actual_sha256 = sha256_file(path)
    if actual_sha256 != expected_sha256.lower():
        fail(DIAG_STALE, f"{field_name} sha256 mismatch for {raw_path}")
    return path


def accepted_intake_entries(intake_manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    entries = intake_manifest.get("entries")
    if not isinstance(entries, list):
        fail(DIAG_SCHEMA, "intake manifest entries must be a list")
    accepted: dict[str, dict[str, Any]] = {}
    for entry in entries:
        entry = require_object(entry, "intake manifest entry")
        fixture_id = entry.get("fixture_id")
        if not isinstance(fixture_id, str) or not fixture_id:
            fail(DIAG_SCHEMA, "intake manifest entry missing fixture_id")
        if entry.get("trust_state") == "accepted":
            accepted[fixture_id] = entry
    return accepted


def expected_evidence_path(intake_entry: dict[str, Any]) -> str:
    surface = intake_entry.get("normalized_surface")
    if surface == "conformance-case":
        path = intake_entry.get("normalized_case_path")
    elif surface == "replay-contract":
        path = intake_entry.get("normalized_contract_path")
    else:
        fail(DIAG_NON_REPRODUCIBLE, f"{intake_entry.get('fixture_id')} uses unsupported normalized_surface")
    if not isinstance(path, str) or not path:
        fail(DIAG_MISSING, f"{intake_entry.get('fixture_id')} is missing normalized evidence path")
    return path


def require_reproducibility(entry: dict[str, Any], fixture_id: str) -> None:
    reproducibility = entry.get("reproducibility")
    if not isinstance(reproducibility, dict):
        fail(DIAG_NON_REPRODUCIBLE, f"{fixture_id} is missing reproducibility metadata")
    if reproducibility.get("mode") != "checked-in-replay":
        fail(DIAG_NON_REPRODUCIBLE, f"{fixture_id} is not backed by checked-in replay")
    if reproducibility.get("requires_live_replay") is not True:
        fail(DIAG_NON_REPRODUCIBLE, f"{fixture_id} does not require live replay")
    if reproducibility.get("local_only") is True:
        fail(DIAG_LOCAL_ONLY, f"{fixture_id} is marked local-only")
    if reproducibility.get("tmp_source_allowed") is not False:
        fail(DIAG_LOCAL_ONLY, f"{fixture_id} allows tmp as source evidence")


def validate_corpus_entries(
    corpus: dict[str, Any],
    *,
    intake_entries: dict[str, dict[str, Any]],
) -> tuple[dict[str, dict[str, Any]], list[str]]:
    entries = corpus.get("entries")
    if not isinstance(entries, list) or not entries:
        fail(DIAG_SCHEMA, "repro corpus entries must be a non-empty list")

    fixture_ids = [str(entry.get("fixture_id")) for entry in entries if isinstance(entry, dict)]
    require_sorted_unique(fixture_ids, label="repro corpus fixture_id list")

    checked_paths: list[str] = []
    by_fixture_id: dict[str, dict[str, Any]] = {}
    for raw_entry in entries:
        entry = require_object(raw_entry, "repro corpus entry")
        fixture_id = entry.get("fixture_id")
        if not isinstance(fixture_id, str) or not fixture_id:
            fail(DIAG_SCHEMA, "repro corpus entry missing fixture_id")
        intake_entry = intake_entries.get(fixture_id)
        if intake_entry is None:
            fail(DIAG_CLAIM_BINDING, f"{fixture_id} is not an accepted intake entry")
        if entry.get("trust_state") != "accepted":
            fail(DIAG_CLAIM_BINDING, f"{fixture_id} is not accepted")
        if entry.get("normalized_surface") != intake_entry.get("normalized_surface"):
            fail(DIAG_CLAIM_BINDING, f"{fixture_id} normalized_surface drifted from intake manifest")
        if entry.get("family") != intake_entry.get("family"):
            fail(DIAG_CLAIM_BINDING, f"{fixture_id} family drifted from intake manifest")

        evidence_path = entry.get("evidence_path")
        expected_path = expected_evidence_path(intake_entry)
        checked_paths.append(repo_rel(require_path_hash(str(evidence_path), str(entry.get("evidence_sha256", "")), field_name=f"{fixture_id} evidence_path")))
        if evidence_path != expected_path:
            fail(DIAG_CLAIM_BINDING, f"{fixture_id} evidence path drifted from intake manifest")

        replay_script = entry.get("replay_script")
        checked_paths.append(repo_rel(require_path_hash(str(replay_script), str(entry.get("replay_script_sha256", "")), field_name=f"{fixture_id} replay_script")))
        if replay_script != intake_entry.get("replay_script"):
            fail(DIAG_CLAIM_BINDING, f"{fixture_id} replay_script drifted from intake manifest")

        require_reproducibility(entry, fixture_id)
        for claim_field in ("support_claims", "adoption_claims"):
            claims = entry.get(claim_field)
            if not isinstance(claims, list) or not claims:
                fail(DIAG_CLAIM_BINDING, f"{fixture_id} is missing {claim_field}")
            claim_ids = [str(claim_id) for claim_id in claims]
            require_sorted_unique(claim_ids, label=f"{fixture_id} {claim_field}")
        by_fixture_id[fixture_id] = entry
    return by_fixture_id, sorted(set(checked_paths))


def validate_claim_bindings(
    gate: dict[str, Any],
    *,
    corpus_entries: dict[str, dict[str, Any]],
) -> list[dict[str, Any]]:
    bindings = gate.get("claim_bindings")
    if not isinstance(bindings, list) or not bindings:
        fail(DIAG_SCHEMA, "claim gate bindings must be a non-empty list")
    claim_ids = [str(binding.get("claim_id")) for binding in bindings if isinstance(binding, dict)]
    require_sorted_unique(claim_ids, label="claim binding claim_id list")

    summaries: list[dict[str, Any]] = []
    for raw_binding in bindings:
        binding = require_object(raw_binding, "claim binding")
        claim_id = binding.get("claim_id")
        claim_kind = binding.get("claim_kind")
        fixture_ids = binding.get("required_fixture_ids")
        if not isinstance(claim_id, str) or not claim_id:
            fail(DIAG_CLAIM_BINDING, "claim binding missing claim_id")
        if claim_kind not in CLAIM_KINDS:
            fail(DIAG_CLAIM_BINDING, f"{claim_id} uses unsupported claim_kind")
        if not isinstance(fixture_ids, list) or not fixture_ids:
            fail(DIAG_CLAIM_BINDING, f"{claim_id} must name required_fixture_ids")
        required_fixture_ids = [str(fixture_id) for fixture_id in fixture_ids]
        require_sorted_unique(required_fixture_ids, label=f"{claim_id} required_fixture_ids")

        for fixture_id in required_fixture_ids:
            entry = corpus_entries.get(fixture_id)
            if entry is None:
                fail(DIAG_MISSING, f"{claim_id} references missing fixture {fixture_id}")
            claim_field = "support_claims" if claim_kind == "support" else "adoption_claims"
            if claim_id not in set(str(value) for value in entry.get(claim_field, [])):
                fail(DIAG_CLAIM_BINDING, f"{claim_id} is not bound on {fixture_id}")
        summaries.append(
            {
                "claim_id": claim_id,
                "claim_kind": claim_kind,
                "required_fixture_count": len(required_fixture_ids),
            }
        )
    return summaries


def validate_external_claim_gate(
    *,
    corpus_path: Path,
    gate_path: Path,
) -> dict[str, Any]:
    corpus = load_json(corpus_path)
    gate = load_json(gate_path)
    require_contract(corpus, contract_id=CORPUS_CONTRACT_ID, label="repro corpus")
    require_contract(gate, contract_id=GATE_CONTRACT_ID, label="support claim gate")

    corpus_from_gate = gate.get("repro_corpus")
    if corpus_from_gate != repo_rel(corpus_path):
        fail(DIAG_SCHEMA, "support claim gate repro_corpus path drifted")

    trust_policy_path = require_existing_repo_file(str(gate.get("trust_policy")), field_name="trust_policy")
    intake_manifest_path = require_existing_repo_file(str(gate.get("intake_manifest")), field_name="intake_manifest")
    adoption_policy_path = require_existing_repo_file(str(gate.get("adoption_claim_policy")), field_name="adoption_claim_policy")

    trust_policy = load_json(trust_policy_path)
    intake_manifest = load_json(intake_manifest_path)
    adoption_policy = load_json(adoption_policy_path)
    require_contract(trust_policy, contract_id=TRUST_POLICY_CONTRACT_ID, label="trust policy")
    require_contract(intake_manifest, contract_id=INTAKE_MANIFEST_CONTRACT_ID, label="intake manifest")
    require_contract(adoption_policy, contract_id=ADOPTION_POLICY_CONTRACT_ID, label="adoption claim policy")

    if trust_policy.get("publishable_trust_states") != ["accepted"]:
        fail(DIAG_CLAIM_BINDING, "external validation publishable trust states must fail closed to accepted only")
    if "fixtures with nondeterministic replay stay quarantined until normalized replay passes" not in trust_policy.get("fail_closed_rules", []):
        fail(DIAG_NON_REPRODUCIBLE, "trust policy no longer fails closed on nondeterministic replay")

    corpus_entries, checked_paths = validate_corpus_entries(
        corpus,
        intake_entries=accepted_intake_entries(intake_manifest),
    )
    claim_summaries = validate_claim_bindings(gate, corpus_entries=corpus_entries)

    negative_fixture_root = gate.get("negative_fixture_root")
    if isinstance(negative_fixture_root, str):
        root = require_repo_relative(negative_fixture_root, field_name="negative_fixture_root")
        if not root.is_dir():
            fail(DIAG_MISSING, f"negative fixture root does not exist: {negative_fixture_root}")
        checked_paths.append(repo_rel(root))

    for case in gate.get("negative_fixtures", []):
        case = require_object(case, "negative fixture")
        fixture_path = require_existing_repo_file(str(case.get("path")), field_name=f"{case.get('case_id')} path")
        checked_paths.append(repo_rel(fixture_path))
        expected_code = case.get("expected_diagnostic_code")
        if expected_code not in {
            DIAG_MISSING,
            DIAG_STALE,
            DIAG_NON_REPRODUCIBLE,
            DIAG_LOCAL_ONLY,
        }:
            fail(DIAG_SCHEMA, f"{case.get('case_id')} uses an unknown expected_diagnostic_code")

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_external_support_claim_gate.py",
        "repro_corpus": repo_rel(corpus_path),
        "claim_gate": repo_rel(gate_path),
        "corpus_revision": corpus.get("corpus_revision"),
        "fixture_count": len(corpus_entries),
        "claim_binding_count": len(claim_summaries),
        "claim_bindings": claim_summaries,
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--corpus", type=Path, default=REPRO_CORPUS)
    parser.add_argument("--claim-gate", type=Path, default=CLAIM_GATE)
    parser.add_argument("--summary", type=Path, default=SUMMARY_PATH)
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        summary = validate_external_claim_gate(
            corpus_path=args.corpus,
            gate_path=args.claim_gate,
        )
    except ClaimGateFailure as exc:
        print(
            f"external-support-claim-gate: FAIL {exc.diagnostic_code}\n- {exc}",
            file=sys.stderr,
        )
        return 1

    write_report_json(args.summary, summary)
    print(f"summary_path: {repo_rel(args.summary)}")
    print("external-support-claim-gate: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
