from __future__ import annotations

import argparse
import json
import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Callable

from scripts.objc3c_tooling.paths import repo_rel
from scripts.objc3c_tooling.subprocesses import CommandExecution, run_timed

from .constants import (
    CONTRACT_PATH,
    MANIFEST_PATH,
    SUMMARY_CONTRACT_ID,
    SUMMARY_PATH,
    ROOT,
)
from .models import FrameworkSample
from .validation import (
    build_compile_command,
    load_json,
    validate_compiled_replay_contract,
    validate_manifest,
)

RunCommand = Callable[[list[str]], CommandExecution]


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate application framework samples.")
    parser.add_argument(
        "--sample",
        action="append",
        default=[],
        help="Compile only a named sample id. Repeatable.",
    )
    parser.add_argument(
        "--manifest",
        default=str(MANIFEST_PATH),
        help="Application framework sample manifest path.",
    )
    parser.add_argument(
        "--contract",
        default=str(CONTRACT_PATH),
        help="Application framework sample contract fixture path.",
    )
    parser.add_argument(
        "--summary-out",
        default=str(SUMMARY_PATH),
        help="Summary report output path.",
    )
    parser.add_argument(
        "--validate-only",
        action="store_true",
        help="Validate manifests without compiling sample sources.",
    )
    return parser.parse_args(argv)


def _default_run_command(command: list[str]) -> CommandExecution:
    return run_timed(command, cwd=ROOT, echo=True)


def _required_artifact_paths(sample: FrameworkSample, root: Path, contract: dict[str, object]) -> dict[str, str]:
    required_artifacts = contract.get("required_artifacts", [])
    if not isinstance(required_artifacts, list):
        required_artifacts = []
    return {
        str(relative): repo_rel(sample.artifact_path(root) / str(relative), root=root)
        for relative in required_artifacts
    }


def _reset_sample_artifact_root(sample: FrameworkSample, root: Path) -> bool:
    artifact_path = sample.artifact_path(root)
    allowed_root = (root / "tmp" / "artifacts" / "application-framework-samples").resolve()
    resolved = artifact_path.resolve()
    try:
        resolved.relative_to(allowed_root)
    except ValueError as exc:
        raise RuntimeError(f"{sample.sample_id}: artifact root escaped sample output root") from exc

    existed = artifact_path.exists()
    if existed:
        if artifact_path.is_dir():
            shutil.rmtree(artifact_path)
        else:
            artifact_path.unlink()
    artifact_path.mkdir(parents=True, exist_ok=True)
    return existed


def _compile_sample(
    *,
    root: Path,
    sample: FrameworkSample,
    contract: dict[str, object],
    run_command: RunCommand,
) -> dict[str, object]:
    command = build_compile_command(sample)
    preexisting_artifact_root_removed = _reset_sample_artifact_root(sample, root)
    result = run_command(command)
    artifacts = _required_artifact_paths(sample, root, contract)
    missing_artifacts = [
        path for path in artifacts.values() if not (root / path).is_file()
    ]
    replay_contract = load_json(sample.replay_contract_path(root))
    replay_failures = (
        validate_compiled_replay_contract(
            root=root,
            sample=sample,
            replay_contract=replay_contract,
        )
        if result.returncode == 0
        else [f"{sample.sample_id}: compile command failed before replay validation"]
    )
    return {
        "sample_id": sample.sample_id,
        "package_id": sample.package_id,
        "kind": sample.kind,
        "source": sample.source,
        "workspace_manifest": sample.workspace_manifest,
        "replay_contract": sample.replay_contract,
        "tutorial": sample.tutorial,
        "module_name": sample.module_name,
        "capabilities": list(sample.capabilities),
        "support_claims": list(sample.support_claims),
        "package_dependencies": list(sample.package_dependencies),
        "command": list(result.command),
        "exit_code": result.returncode,
        "duration_seconds": result.duration_seconds,
        "artifact_root": sample.artifact_root,
        "preexisting_artifact_root_removed": preexisting_artifact_root_removed,
        "stale_artifacts_allowed": False,
        "artifacts": artifacts,
        "missing_artifacts": missing_artifacts,
        "replay_failures": replay_failures,
        "status": "PASS"
        if result.returncode == 0 and not missing_artifacts and not replay_failures
        else "FAIL",
    }


def _write_summary(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def run_framework_sample_validation(
    *,
    root: Path = ROOT,
    manifest_path: Path = MANIFEST_PATH,
    contract_path: Path = CONTRACT_PATH,
    summary_path: Path = SUMMARY_PATH,
    selected_sample_ids: set[str] | None = None,
    compile_samples: bool = True,
    run_command: RunCommand = _default_run_command,
) -> tuple[int, dict[str, object]]:
    manifest = load_json(manifest_path)
    contract = load_json(contract_path)
    samples, failures = validate_manifest(root=root, manifest=manifest, contract=contract)

    requested_ids = selected_sample_ids or set()
    known_ids = {sample.sample_id for sample in samples}
    unknown_ids = sorted(requested_ids - known_ids)
    for sample_id in unknown_ids:
        failures.append(f"unknown sample id {sample_id}")

    selected_samples = [
        sample
        for sample in samples
        if not requested_ids or sample.sample_id in requested_ids
    ]
    compile_results: list[dict[str, object]] = []
    if compile_samples and not failures:
        for sample in selected_samples:
            result = _compile_sample(
                root=root,
                sample=sample,
                contract=contract,
                run_command=run_command,
            )
            compile_results.append(result)
            if result["status"] != "PASS":
                failures.append(f"{sample.sample_id}: compile failed or artifacts missing")

    payload: dict[str, object] = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "manifest": repo_rel(manifest_path, root=root),
        "contract": repo_rel(contract_path, root=root),
        "selected_sample_ids": [sample.sample_id for sample in selected_samples],
        "sample_count": len(samples),
        "compiled_sample_count": len(compile_results),
        "capability_domains": manifest.get("capability_domains", []),
        "package_edges": manifest.get("package_edges", []),
        "samples": [
            {
                "sample_id": sample.sample_id,
                "kind": sample.kind,
                "package_id": sample.package_id,
                "source": sample.source,
                "workspace_manifest": sample.workspace_manifest,
                "replay_contract": sample.replay_contract,
                "tutorial": sample.tutorial,
                "capabilities": list(sample.capabilities),
                "support_claims": list(sample.support_claims),
                "package_dependencies": list(sample.package_dependencies),
            }
            for sample in samples
        ],
        "compile_results": compile_results,
        "failures": failures,
    }
    _write_summary(summary_path, payload)
    return (0 if not failures else 1), payload


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    exit_code, payload = run_framework_sample_validation(
        manifest_path=Path(args.manifest),
        contract_path=Path(args.contract),
        summary_path=Path(args.summary_out),
        selected_sample_ids=set(args.sample),
        compile_samples=not args.validate_only,
    )
    print(f"summary_path: {repo_rel(Path(args.summary_out), root=ROOT)}")
    print(f"application-framework-samples: {payload['status']}")
    if payload["failures"]:
        for failure in payload["failures"]:
            print(f"- {failure}", file=sys.stderr)
    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())
