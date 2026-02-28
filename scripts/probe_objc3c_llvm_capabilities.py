#!/usr/bin/env python3
"""Deterministically probe LLVM and sema/type-system parity capability surfaces."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "objc3c-llvm-capabilities-v2"


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--clang", type=Path, default=Path("clang"))
    parser.add_argument("--llc", type=Path, default=Path("llc"))
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/objc3c_llvm_capabilities_summary.json"),
    )
    return parser.parse_args(argv)


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    try:
        return subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=False,
        )
    except FileNotFoundError:
        return subprocess.CompletedProcess(
            command,
            127,
            stdout="",
            stderr=f"executable not found: {command[0]}",
        )
    except OSError as exc:
        return subprocess.CompletedProcess(
            command,
            126,
            stdout="",
            stderr=f"command launch error ({exc.__class__.__name__}): {command[0]}",
        )


def first_non_empty_line(text: str) -> str:
    for line in text.splitlines():
        stripped = line.strip()
        if stripped:
            return stripped
    return ""


def probe_executable(path: Path, *, role: str) -> dict[str, object]:
    version_cmd = [str(path), "--version"]
    version_result = run_command(version_cmd)
    version_text = (version_result.stdout or "") + (version_result.stderr or "")

    found = version_result.returncode != 127
    head = first_non_empty_line(version_text)
    payload: dict[str, object] = {
        "role": role,
        "path": str(path),
        "found": found,
        "version_exit_code": version_result.returncode,
        "version_headline": head,
    }
    if not found:
        payload["diagnostic"] = f"{role} executable not found: {path}"
    return payload


def probe_llc_filetype_obj(path: Path) -> dict[str, object]:
    help_cmd = [str(path), "--help"]
    help_result = run_command(help_cmd)
    help_text = (help_result.stdout or "") + (help_result.stderr or "")

    mentions_filetype = "--filetype" in help_text or "-filetype" in help_text
    mentions_obj = re.search(r"\bobj\b", help_text) is not None
    supports_from_help = mentions_filetype and mentions_obj

    version_with_filetype_cmd = [str(path), "--filetype=obj", "--version"]
    filetype_version_result = run_command(version_with_filetype_cmd)
    supports_from_command = filetype_version_result.returncode != 127

    supports_filetype_obj = supports_from_help or supports_from_command

    return {
        "help_exit_code": help_result.returncode,
        "help_mentions_filetype": mentions_filetype,
        "help_mentions_obj": mentions_obj,
        "version_with_filetype_exit_code": filetype_version_result.returncode,
        "supports_filetype_obj": supports_filetype_obj,
    }


def build_sema_type_system_parity_surface(
    *,
    clang_probe: dict[str, object],
    llc_probe: dict[str, object],
    llc_features: dict[str, object],
) -> dict[str, object]:
    clang_found = bool(clang_probe.get("found"))
    llc_found = bool(llc_probe.get("found"))
    llc_supports_obj = bool(llc_features.get("supports_filetype_obj", False))

    deterministic_semantic_diagnostics = clang_found
    deterministic_type_metadata_handoff = clang_found and llc_found and llc_supports_obj

    blockers: list[str] = []
    if not clang_found:
        blockers.append("clang executable missing")
    if not llc_found:
        blockers.append("llc executable missing")
    elif not llc_supports_obj:
        blockers.append("llc missing --filetype=obj support")

    parity_ready = deterministic_semantic_diagnostics and deterministic_type_metadata_handoff
    return {
        "deterministic_semantic_diagnostics": deterministic_semantic_diagnostics,
        "deterministic_type_metadata_handoff": deterministic_type_metadata_handoff,
        "parity_ready": parity_ready,
        "blockers": blockers,
    }


def write_json(path: Path, payload: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(canonical_json(payload), encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    clang_probe = probe_executable(args.clang, role="clang")
    llc_probe = probe_executable(args.llc, role="llc")

    llc_features: dict[str, object] = {
        "supports_filetype_obj": False,
    }
    if bool(llc_probe["found"]):
        llc_features = probe_llc_filetype_obj(args.llc)

    sema_type_system_parity = build_sema_type_system_parity_surface(
        clang_probe=clang_probe,
        llc_probe=llc_probe,
        llc_features=llc_features,
    )

    failures: list[str] = []
    if not bool(clang_probe["found"]):
        failures.append(str(clang_probe.get("diagnostic", "clang executable missing")))
    if not bool(llc_probe["found"]):
        failures.append(str(llc_probe.get("diagnostic", "llc executable missing")))
    if bool(llc_probe["found"]) and not bool(llc_features["supports_filetype_obj"]):
        failures.append("llc capability probe failed: --filetype=obj support not detected")
    if not bool(sema_type_system_parity["parity_ready"]):
        failures.append(
            "sema/type-system parity capability unavailable: "
            + ", ".join(str(blocker) for blocker in sema_type_system_parity["blockers"])
        )

    summary = {
        "mode": MODE,
        "clang": clang_probe,
        "llc": llc_probe,
        "llc_features": llc_features,
        "sema_type_system_parity": sema_type_system_parity,
        "failures": failures,
        "ok": not failures,
    }
    write_json(args.summary_out, summary)

    if failures:
        for failure in failures:
            print(f"LLVM-CAP-FAIL: {failure}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("LLVM-CAP-PASS: clang+llc capabilities discovered; sema/type-system parity ready")
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
