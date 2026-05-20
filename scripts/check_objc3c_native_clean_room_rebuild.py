#!/usr/bin/env python3
"""Validate deterministic native bootstrap rebuilds from isolated output roots."""

from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.paths import display_path

ROOT = Path(__file__).resolve().parents[1]
TEMP_ROOT = ROOT / "tmp" / "clean-room" / "objc3c-native-rebuild"
REPORT_DEFAULT = ROOT / "tmp" / "reports" / "native-clean-room-rebuild" / "summary.json"
ENSURE_BUILD = ROOT / "scripts" / "ensure_objc3c_native_build.py"
PATHFUL_JSON_ARTIFACTS = {
    "compile_commands",
    "build_fingerprint",
    "repo_superclean_surface",
}
RAW_DIGEST_ARTIFACTS = {
    "native_executable",
    "capi_runner",
    "runtime_library",
}


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mode", choices=("fast", "contracts", "full"), default="fast")
    parser.add_argument("--work-root", type=Path, default=TEMP_ROOT)
    parser.add_argument("--summary-out", type=Path, default=REPORT_DEFAULT)
    parser.add_argument("--parallelism", type=int, default=4)
    parser.add_argument("--keep-work-root", action="store_true")
    return parser.parse_args(argv)


def path_is_relative_to(path: Path, root: Path) -> bool:
    try:
        path.resolve().relative_to(root.resolve())
        return True
    except ValueError:
        return False


def reset_work_root(work_root: Path) -> None:
    clean_room_parent = (ROOT / "tmp" / "clean-room").resolve()
    resolved = work_root.resolve()
    if not path_is_relative_to(resolved, clean_room_parent):
        raise RuntimeError(f"refusing to reset non-clean-room path: {work_root}")
    if work_root.exists():
        shutil.rmtree(work_root)
    work_root.mkdir(parents=True, exist_ok=True)


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def normalized_value(value: Any, clean_root: Path) -> Any:
    clean_root_display = display_path(clean_root)
    clean_root_native = str(clean_root.resolve())
    if isinstance(value, str):
        return (
            value.replace(clean_root_native, "<clean-room>")
            .replace(clean_root_native.replace("\\", "/"), "<clean-room>")
            .replace(clean_root_display, "<clean-room>")
        )
    if isinstance(value, list):
        return [normalized_value(item, clean_root) for item in value]
    if isinstance(value, dict):
        return {key: normalized_value(item, clean_root) for key, item in value.items()}
    return value


def artifact_path(summary: dict[str, Any], artifact_name: str, clean_root: Path) -> Path:
    artifact = summary["artifacts"][artifact_name]
    if not artifact.get("exists"):
        raise RuntimeError(f"{artifact_name} was not produced in {display_path(clean_root)}")
    return ROOT / artifact["path"]


def normalized_json_digest(path: Path, clean_root: Path) -> str:
    payload = json.loads(path.read_text(encoding="utf-8"))
    normalized = normalized_value(payload, clean_root)
    return sha256_text(canonical_json(normalized))


def frontend_packet_digests(summary: dict[str, Any], clean_root: Path) -> dict[str, str]:
    result: dict[str, str] = {}
    for packet in summary["artifacts"]["frontend_packets"]:
        artifact = packet["artifact"]
        if not artifact.get("exists"):
            raise RuntimeError(f"frontend packet missing: {packet['name']}")
        path = ROOT / artifact["path"]
        result[packet["name"]] = normalized_json_digest(path, clean_root)
    return result


def run_rebuild(
    label: str, mode: str, work_root: Path, parallelism: int
) -> dict[str, Any]:
    clean_root = work_root / label
    summary_dir = clean_root / "reports"
    ensure_summary = summary_dir / "ensure_summary.json"
    build_summary = summary_dir / "native_build_summary.json"
    command = [
        sys.executable,
        str(ENSURE_BUILD),
        "--mode",
        mode,
        "--reason",
        f"native-clean-room-{label}",
        "--summary-out",
        str(ensure_summary),
        "--build-summary-out",
        str(build_summary),
        "--clean-room-root",
        str(clean_root),
        "--force-reconfigure",
        "--parallelism",
        str(parallelism),
    ]
    completed = subprocess.run(command, cwd=ROOT, check=False, text=True)
    if completed.returncode != 0:
        raise RuntimeError(f"clean-room rebuild {label} failed with exit code {completed.returncode}")
    ensure_payload = load_json(ensure_summary)
    build_payload = load_json(build_summary)
    return {
        "label": label,
        "clean_root": clean_root,
        "ensure_summary_path": ensure_summary,
        "build_summary_path": build_summary,
        "ensure": ensure_payload,
        "build": build_payload,
    }


def compare_runs(left: dict[str, Any], right: dict[str, Any]) -> list[dict[str, Any]]:
    checks: list[dict[str, Any]] = []
    left_build = left["build"]
    right_build = right["build"]

    def add_check(name: str, ok: bool, detail: str) -> None:
        checks.append({"name": name, "ok": ok, "detail": detail})

    for run in (left, right):
        ensure = run["ensure"]
        build = run["build"]
        clean_root = run["clean_root"]
        add_check(
            f"{run['label']}:ensure-ok",
            bool(ensure.get("ok")),
            display_path(run["ensure_summary_path"]),
        )
        add_check(
            f"{run['label']}:clean-root-routed",
            bool(build.get("clean_room")) and bool(ensure.get("saw_clean_room_root")),
            display_path(clean_root),
        )
        add_check(
            f"{run['label']}:native-build-ran",
            bool(build.get("native_build_ran")) and bool(ensure.get("saw_cmake_build_start")),
            display_path(run["build_summary_path"]),
        )
        for root_path in build["output_roots"].values():
            add_check(
                f"{run['label']}:output-root-isolated:{root_path}",
                str(root_path).startswith(display_path(clean_root)),
                root_path,
            )

    add_check(
        "execution-mode-parity",
        left_build["execution_mode"] == right_build["execution_mode"],
        f"{left_build['execution_mode']} vs {right_build['execution_mode']}",
    )

    for artifact_name in sorted(RAW_DIGEST_ARTIFACTS):
        left_artifact = left_build["artifacts"][artifact_name]
        right_artifact = right_build["artifacts"][artifact_name]
        add_check(
            f"raw-digest-parity:{artifact_name}",
            left_artifact.get("sha256") == right_artifact.get("sha256"),
            f"{left_artifact.get('sha256')} vs {right_artifact.get('sha256')}",
        )

    for artifact_name in sorted(PATHFUL_JSON_ARTIFACTS):
        left_digest = normalized_json_digest(
            artifact_path(left_build, artifact_name, left["clean_root"]),
            left["clean_root"],
        )
        right_digest = normalized_json_digest(
            artifact_path(right_build, artifact_name, right["clean_root"]),
            right["clean_root"],
        )
        add_check(
            f"normalized-json-parity:{artifact_name}",
            left_digest == right_digest,
            f"{left_digest} vs {right_digest}",
        )

    left_packets = frontend_packet_digests(left_build, left["clean_root"])
    right_packets = frontend_packet_digests(right_build, right["clean_root"])
    add_check(
        "frontend-packet-set-parity",
        sorted(left_packets) == sorted(right_packets),
        f"{sorted(left_packets)} vs {sorted(right_packets)}",
    )
    for packet_name in sorted(set(left_packets) & set(right_packets)):
        add_check(
            f"frontend-packet-parity:{packet_name}",
            left_packets[packet_name] == right_packets[packet_name],
            f"{left_packets[packet_name]} vs {right_packets[packet_name]}",
        )

    return checks


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    if args.parallelism < 1:
        print("error: clean-room rebuild parallelism must be at least 1", file=sys.stderr)
        return 2
    if not args.keep_work_root:
        reset_work_root(args.work_root)
    else:
        args.work_root.mkdir(parents=True, exist_ok=True)

    left = run_rebuild("run-a", args.mode, args.work_root, args.parallelism)
    right = run_rebuild("run-b", args.mode, args.work_root, args.parallelism)
    checks = compare_runs(left, right)
    ok = all(check["ok"] for check in checks)
    summary = {
        "contract_id": "objc3c-native-clean-room-rebuild-gate-v1",
        "ok": ok,
        "mode": args.mode,
        "parallelism": args.parallelism,
        "work_root": display_path(args.work_root),
        "runs": [
            {
                "label": left["label"],
                "clean_root": display_path(left["clean_root"]),
                "ensure_summary_path": display_path(left["ensure_summary_path"]),
                "build_summary_path": display_path(left["build_summary_path"]),
            },
            {
                "label": right["label"],
                "clean_root": display_path(right["clean_root"]),
                "ensure_summary_path": display_path(right["ensure_summary_path"]),
                "build_summary_path": display_path(right["build_summary_path"]),
            },
        ],
        "checks": checks,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if ok:
        print(f"[ok] native clean-room rebuild gate passed: {display_path(args.summary_out)}")
        return 0
    print(f"[fail] native clean-room rebuild gate failed: {display_path(args.summary_out)}", file=sys.stderr)
    for check in checks:
        if not check["ok"]:
            print(f"[fail] {check['name']}: {check['detail']}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
