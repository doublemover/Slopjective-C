#!/usr/bin/env python3
"""Deterministically probe LLVM and sema/type-system parity capability surfaces."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "objc3c-llvm-capabilities-v2"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
SHOWCASE_PORTFOLIO_PATH = ROOT / "showcase" / "portfolio.json"


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def load_json(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


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


def run_command(command: list[str]) -> tuple[subprocess.CompletedProcess[str], float]:
    started = time.perf_counter()
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=False,
        )
    except FileNotFoundError:
        result = subprocess.CompletedProcess(
            command,
            127,
            stdout="",
            stderr=f"executable not found: {command[0]}",
        )
    except OSError as exc:
        result = subprocess.CompletedProcess(
            command,
            126,
            stdout="",
            stderr=f"command launch error ({exc.__class__.__name__}): {command[0]}",
        )
    duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
    return result, duration_ms


def first_non_empty_line(text: str) -> str:
    for line in text.splitlines():
        stripped = line.strip()
        if stripped:
            return stripped
    return ""


def probe_executable(path: Path, *, role: str) -> dict[str, object]:
    version_cmd = [str(path), "--version"]
    version_result, version_duration_ms = run_command(version_cmd)
    version_text = (version_result.stdout or "") + (version_result.stderr or "")

    found = version_result.returncode != 127
    head = first_non_empty_line(version_text)
    payload: dict[str, object] = {
        "role": role,
        "path": str(path),
        "found": found,
        "version_exit_code": version_result.returncode,
        "version_duration_ms": version_duration_ms,
        "version_headline": head,
    }
    if not found:
        payload["diagnostic"] = f"{role} executable not found: {path}"
    return payload


def probe_llc_filetype_obj(path: Path) -> dict[str, object]:
    help_cmd = [str(path), "--help"]
    help_result, help_duration_ms = run_command(help_cmd)
    help_text = (help_result.stdout or "") + (help_result.stderr or "")

    mentions_filetype = "--filetype" in help_text or "-filetype" in help_text
    mentions_obj = re.search(r"\bobj\b", help_text) is not None
    supports_from_help = mentions_filetype and mentions_obj

    version_with_filetype_cmd = [str(path), "--filetype=obj", "--version"]
    filetype_version_result, version_with_filetype_duration_ms = run_command(version_with_filetype_cmd)
    supports_from_command = filetype_version_result.returncode != 127

    supports_filetype_obj = supports_from_help or supports_from_command

    return {
        "help_exit_code": help_result.returncode,
        "help_duration_ms": help_duration_ms,
        "help_mentions_filetype": mentions_filetype,
        "help_mentions_obj": mentions_obj,
        "version_with_filetype_exit_code": filetype_version_result.returncode,
        "version_with_filetype_duration_ms": version_with_filetype_duration_ms,
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


def build_capability_demo_compatibility_surface(
    *,
    program_surface: dict[str, object],
    showcase_portfolio: dict[str, object],
    parity_ready: bool,
) -> dict[str, object]:
    failures: list[str] = []
    program_examples = program_surface.get("capability_demo_examples")
    showcase_examples = showcase_portfolio.get("examples")
    onboarding_policy = program_surface.get("onboarding_policy")

    if not isinstance(program_examples, list):
        failures.append("program surface did not publish capability_demo_examples")
        program_examples = []
    if not isinstance(showcase_examples, list):
        failures.append("showcase portfolio did not publish examples")
        showcase_examples = []
    if not isinstance(onboarding_policy, dict):
        failures.append("program surface did not publish onboarding_policy")
        onboarding_policy = {}

    program_examples_by_id = {
        str(entry.get("id")): entry
        for entry in program_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    showcase_examples_by_id = {
        str(entry.get("id")): entry
        for entry in showcase_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }

    program_ids = list(program_examples_by_id)
    showcase_ids = list(showcase_examples_by_id)
    examples: list[dict[str, object]] = []

    program_examples_match_showcase_examples = program_ids == showcase_ids
    story_capabilities_match = True
    stdlib_followup_modules_match = True
    actor_claims_are_qualified = True

    if not program_examples_match_showcase_examples:
        failures.append("program surface example ids drifted from showcase portfolio examples")

    for demo_id in program_ids:
        program_entry = program_examples_by_id[demo_id]
        showcase_entry = showcase_examples_by_id.get(demo_id)
        if showcase_entry is None:
            failures.append(f"showcase portfolio missing capability demo entry {demo_id}")
            continue
        story_capabilities = program_entry.get("story_capabilities")
        stdlib_followup_modules = program_entry.get("stdlib_followup_modules")
        if not isinstance(story_capabilities, list) or not all(
            isinstance(value, str) and value for value in story_capabilities
        ):
            failures.append(f"program surface story_capabilities malformed for {demo_id}")
            story_capabilities = []
        if not isinstance(stdlib_followup_modules, list) or not all(
            isinstance(value, str) and value for value in stdlib_followup_modules
        ):
            failures.append(f"program surface stdlib_followup_modules malformed for {demo_id}")
            stdlib_followup_modules = []
        if showcase_entry.get("story_capabilities") != story_capabilities:
            story_capabilities_match = False
            failures.append(f"story capability drift detected for {demo_id}")
        if showcase_entry.get("stdlib_followup_modules") != stdlib_followup_modules:
            stdlib_followup_modules_match = False
            failures.append(f"stdlib follow-up module drift detected for {demo_id}")
        if "actors" in story_capabilities:
            actor_claims_are_qualified = False
            failures.append(f"capability demo {demo_id} published unsupported runnable actor claim")

        claim_class = "comparison-facing" if "actor-shaped-messaging" in story_capabilities else "runnable-now"
        examples.append(
            {
                "id": demo_id,
                "claim_class": claim_class,
                "story_capabilities": story_capabilities,
                "stdlib_followup_modules": stdlib_followup_modules,
                "compatible": showcase_entry.get("story_capabilities") == story_capabilities
                and showcase_entry.get("stdlib_followup_modules") == stdlib_followup_modules
                and "actors" not in story_capabilities,
            }
        )

    drift_checks = {
        "program_examples_match_showcase_examples": program_examples_match_showcase_examples,
        "story_capabilities_match": story_capabilities_match,
        "stdlib_followup_modules_match": stdlib_followup_modules_match,
        "actor_claims_are_qualified": actor_claims_are_qualified,
        "probe_ready_for_demo_validation": parity_ready,
    }
    if not parity_ready:
        failures.append("capability demo compatibility requires sema/type-system parity to stay ready")

    return {
        "program_surface": display_path(PROGRAM_SURFACE_PATH),
        "showcase_portfolio": display_path(SHOWCASE_PORTFOLIO_PATH),
        "onboarding_entry_order": onboarding_policy.get("entry_order", []),
        "drift_checks": drift_checks,
        "examples": examples,
        "failures": failures,
        "ok": not failures,
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
    capability_demo_compatibility: dict[str, object]
    if not PROGRAM_SURFACE_PATH.is_file():
        capability_demo_compatibility = {
            "program_surface": display_path(PROGRAM_SURFACE_PATH),
            "showcase_portfolio": display_path(SHOWCASE_PORTFOLIO_PATH),
            "drift_checks": {},
            "examples": [],
            "failures": [f"missing program surface contract: {display_path(PROGRAM_SURFACE_PATH)}"],
            "ok": False,
        }
    elif not SHOWCASE_PORTFOLIO_PATH.is_file():
        capability_demo_compatibility = {
            "program_surface": display_path(PROGRAM_SURFACE_PATH),
            "showcase_portfolio": display_path(SHOWCASE_PORTFOLIO_PATH),
            "drift_checks": {},
            "examples": [],
            "failures": [f"missing showcase portfolio contract: {display_path(SHOWCASE_PORTFOLIO_PATH)}"],
            "ok": False,
        }
    else:
        capability_demo_compatibility = build_capability_demo_compatibility_surface(
            program_surface=load_json(PROGRAM_SURFACE_PATH),
            showcase_portfolio=load_json(SHOWCASE_PORTFOLIO_PATH),
            parity_ready=bool(sema_type_system_parity["parity_ready"]),
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
    for failure in capability_demo_compatibility.get("failures", []):
        failures.append(f"capability demo compatibility: {failure}")

    summary = {
        "mode": MODE,
        "clang": clang_probe,
        "llc": llc_probe,
        "llc_features": llc_features,
        "sema_type_system_parity": sema_type_system_parity,
        "capability_demo_compatibility": capability_demo_compatibility,
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
