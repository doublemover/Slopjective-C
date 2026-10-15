"""Identity-bound handoff from the package producer to clean-install consumers."""
from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel

CONTRACT = "objc3c.package_ecosystem.producer_artifact_report.v1"
LOCK = "tmp/artifacts/package-ecosystem/locks/objc3c-package-lock.json"
MIRROR = "tmp/artifacts/package-ecosystem/mirrors/offline-mirror-index.json"
REGISTRY = "tmp/artifacts/package-ecosystem/registry/local-package-index.json"
PUBLICATION = "tmp/artifacts/package-ecosystem/registry/publication-metadata.json"
RESTORE = "tmp/artifacts/package-ecosystem/offline-install/objc3c-offline-mirror-restore-receipt.json"
SUMMARY = "tmp/reports/package-ecosystem/package-mirror-summary.json"
CONSUMER_CONTRACT = "objc3c.package_ecosystem.clean_install_artifact_report.v1"


def _identity_fields(report: dict[str, Any]) -> None:
    if not isinstance(report.get("run_id"), str) or not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]{0,79}", report["run_id"]):
        raise ValueError("package evidence run identity is malformed")
    for key in ("producer_node_identity", "build_identity"):
        if not isinstance(report.get(key), str) or not report[key].strip():
            raise ValueError(f"package evidence {key} is missing")


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _digest(value: Any) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def _path(root: Path, raw: str) -> Path:
    candidate = (root / raw).resolve()
    try:
        candidate.relative_to(root.resolve())
    except ValueError as error:
        raise ValueError(f"producer artifact escapes repository: {raw}") from error
    if not candidate.is_file():
        raise ValueError(f"producer artifact is missing: {raw}")
    return candidate


def _inventory(root: Path) -> list[str]:
    lock = load_json(_path(root, LOCK))
    summary = load_json(_path(root, SUMMARY))
    if summary.get("status") != "PASS":
        raise ValueError("package producer summary is not PASS")
    paths = {LOCK, MIRROR, REGISTRY, PUBLICATION, RESTORE, SUMMARY}
    # Bind producer implementation as well as generated data, including when
    # the handoff is exercised directly without the cross-lane node runner.
    implementation = [root / "scripts/build_objc3c_package_lock.py",
                      root / "scripts/build_objc3c_package_mirror.py",
                      root / "scripts/package_ecosystem_contracts.py"]
    implementation.extend((root / "scripts/objc3c_package_manager").glob("*.py"))
    paths.update(repo_rel(path, root=root) for path in implementation if path.is_file())
    packages = lock.get("packages")
    if not isinstance(packages, list):
        raise ValueError("package producer lock inventory is missing")
    digest_inputs = lock.get("digest_inputs")
    if not isinstance(digest_inputs, list) or not digest_inputs:
        raise ValueError("package producer input inventory is missing")
    for raw in digest_inputs:
        if not isinstance(raw, str) or not raw:
            raise ValueError("package producer input path is malformed")
        paths.add(raw)
    for package in packages:
        if not isinstance(package, dict):
            raise ValueError("package lock contains malformed package")
        manifest = package.get("package_manifest", {})
        if not isinstance(manifest, dict) or not isinstance(manifest.get("path"), str) or not manifest["path"]:
            raise ValueError("package producer manifest path is missing")
        paths.add(manifest["path"])
        source = package.get("source")
        if not isinstance(source, str) or not source:
            raise ValueError("package producer source path is missing")
        paths.add(source)
    cache_paths = summary.get("cache_paths")
    if not isinstance(cache_paths, list) or not cache_paths:
        raise ValueError("package producer cache inventory is missing")
    for raw in cache_paths:
        if not isinstance(raw, str) or not raw:
            raise ValueError("package producer cache path is malformed")
        paths.add(raw)
    return sorted(paths)


def write_producer_report(root: Path, destination: Path, *, run_id: str,
                          node_identity: str, build_identity: str) -> dict[str, Any]:
    inventory = _inventory(root)
    artifacts = [{"path": raw, "sha256": _sha256(_path(root, raw))} for raw in inventory]
    report: dict[str, Any] = {
        "contract_id": CONTRACT, "status": "PASS", "complete": True,
        "run_id": run_id, "producer_node_identity": node_identity,
        "build_identity": build_identity, "artifacts": artifacts,
    }
    _identity_fields(report)
    report["report_digest"] = _digest(report)
    write_json_file(destination, report)
    return report


def validate_producer_report(root: Path, report_path: Path, *,
                             expected_node_identity: str | None = None,
                             expected_build_identity: str | None = None) -> dict[str, Any]:
    report = load_json(report_path)
    if report.get("contract_id") != CONTRACT or report.get("status") != "PASS" or report.get("complete") is not True:
        raise ValueError("producer report is unsupported, failed, or partial")
    _identity_fields(report)
    digest = report.get("report_digest")
    unsigned = dict(report)
    unsigned.pop("report_digest", None)
    if not isinstance(digest, str) or digest != _digest(unsigned):
        raise ValueError("producer report digest is invalid")
    if expected_node_identity is not None and report.get("producer_node_identity") != expected_node_identity:
        raise ValueError("producer report node identity mismatch")
    if expected_build_identity is not None and report.get("build_identity") != expected_build_identity:
        raise ValueError("producer report build identity mismatch")
    artifacts = report.get("artifacts")
    if not isinstance(artifacts, list) or any(not isinstance(row, dict) for row in artifacts):
        raise ValueError("producer report artifact inventory is malformed")
    actual = []
    for row in artifacts:
        raw = row.get("path")
        if not isinstance(raw, str) or not isinstance(row.get("sha256"), str):
            raise ValueError("producer report artifact entry is malformed")
        path = _path(root, raw)
        if _sha256(path) != row["sha256"]:
            raise ValueError(f"producer artifact changed since production: {raw}")
        actual.append(raw)
    if len(actual) != len(set(actual)) or sorted(actual) != _inventory(root):
        raise ValueError("producer report artifact inventory is partial or mismatched")
    return report


def _consumer_inventory(root: Path, output_root: Path, summary_path: Path) -> list[str]:
    from objc3c_package_manager.install_distribution import INSTALL_VERIFICATION_REL

    output_root = output_root.resolve()
    if output_root.name != "package-consumer" or not output_root.is_relative_to(root.resolve()):
        raise ValueError("clean-install evidence output root is not isolated")
    if not summary_path.resolve().is_relative_to(output_root):
        raise ValueError("clean-install summary escapes its output root")
    summary = load_json(summary_path)
    if summary.get("status") != "PASS" or summary.get("from_nothing_probe", {}).get("requested") is not True:
        raise ValueError("clean-install consumer summary is failed or incomplete")
    verification_path = output_root / INSTALL_VERIFICATION_REL
    verification = load_json(verification_path)
    paths = {repo_rel(summary_path, root=root), repo_rel(verification_path, root=root)}
    generated_paths = verification.get("generated_paths")
    if not isinstance(generated_paths, list) or not generated_paths:
        raise ValueError("clean-install generated artifact inventory is missing")
    for raw in generated_paths:
        if not isinstance(raw, str) or not raw:
            raise ValueError("clean-install artifact path is malformed")
        path = (root / raw).resolve()
        if not path.is_relative_to(output_root) or not path.exists():
            raise ValueError(f"clean-install artifact is missing or escapes output root: {raw}")
        if path.is_file():
            paths.add(repo_rel(path, root=root))
    return sorted(paths)


def write_consumer_report(root: Path, destination: Path, *, run_id: str,
                          node_identity: str, build_identity: str,
                          producer_report_path: Path, output_root: Path,
                          summary_path: Path) -> dict[str, Any]:
    producer = validate_producer_report(root, producer_report_path, expected_build_identity=build_identity)
    inventory = _consumer_inventory(root, output_root, summary_path)
    summary = load_json(summary_path)
    if summary.get("from_nothing_probe", {}).get("producer_report_digest") != producer["report_digest"]:
        raise ValueError("clean-install summary producer identity mismatch")
    report: dict[str, Any] = {
        "contract_id": CONSUMER_CONTRACT, "status": "PASS", "complete": True,
        "run_id": run_id, "producer_node_identity": producer["producer_node_identity"],
        "consumer_node_identity": node_identity, "build_identity": build_identity,
        "producer_report": repo_rel(producer_report_path, root=root),
        "producer_report_digest": producer["report_digest"],
        "output_root": repo_rel(output_root, root=root),
        "summary": repo_rel(summary_path, root=root),
        "artifacts": [{"path": raw, "sha256": _sha256(_path(root, raw))} for raw in inventory],
    }
    _identity_fields(report)
    report["report_digest"] = _digest(report)
    write_json_file(destination, report)
    return report


def validate_consumer_report(root: Path, report_path: Path, *,
                             expected_node_identity: str,
                             expected_producer_identity: str,
                             expected_build_identity: str) -> dict[str, Any]:
    report = load_json(report_path)
    if report.get("contract_id") != CONSUMER_CONTRACT or report.get("status") != "PASS" or report.get("complete") is not True:
        raise ValueError("clean-install report is unsupported, failed, or partial")
    _identity_fields(report)
    unsigned = dict(report)
    unsigned.pop("report_digest", None)
    if report.get("report_digest") != _digest(unsigned):
        raise ValueError("clean-install report digest is invalid")
    if report.get("consumer_node_identity") != expected_node_identity or report.get("build_identity") != expected_build_identity:
        raise ValueError("clean-install node or build identity mismatch")
    producer = validate_producer_report(root, _path(root, report["producer_report"]),
        expected_node_identity=expected_producer_identity, expected_build_identity=expected_build_identity)
    if (report.get("producer_report_digest") != producer["report_digest"]
            or report.get("producer_node_identity") != producer["producer_node_identity"]):
        raise ValueError("clean-install producer report digest mismatch")
    inventory = _consumer_inventory(root, root / report["output_root"], root / report["summary"])
    summary = load_json(root / report["summary"])
    if summary.get("from_nothing_probe", {}).get("producer_report_digest") != producer["report_digest"]:
        raise ValueError("clean-install summary producer identity mismatch")
    artifacts = report.get("artifacts")
    if not isinstance(artifacts, list) or any(not isinstance(row, dict) for row in artifacts):
        raise ValueError("clean-install artifact inventory is malformed")
    actual = []
    for row in artifacts:
        raw = row.get("path")
        if not isinstance(raw, str) or _sha256(_path(root, raw)) != row.get("sha256"):
            raise ValueError("clean-install artifact changed since installation")
        actual.append(raw)
    if sorted(actual) != inventory or len(actual) != len(set(actual)):
        raise ValueError("clean-install artifact inventory is partial or mismatched")
    return report


__all__ = ["CONTRACT", "validate_producer_report", "write_producer_report",
           "CONSUMER_CONTRACT", "validate_consumer_report", "write_consumer_report"]
