#!/usr/bin/env python3
"""Validate objc3c native dependency direction contracts."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
ARCHITECTURE_DOC = SRC_ROOT / "ARCHITECTURE.md"
RUNTIME_ARCHITECTURE_DOC = SRC_ROOT / "runtime" / "ARCHITECTURE.md"
ADR_ROOT = SRC_ROOT / "adr"

REQUIRED_ARCH_SECTIONS = (
    "## Layer Model",
    "Allowed dependencies:",
    "Forbidden dependencies:",
)
REQUIRED_RUNTIME_ARCH_SECTIONS = (
    "## Runtime Subsystem Model",
    "Owned code paths:",
    "Allowed subsystem dependencies:",
    "Forbidden subsystem shortcuts:",
)
REQUIRED_ADRS = (
    "ADR-0001-layered-frontend-boundaries.md",
    "ADR-0002-cli-vs-library-separation.md",
    "ADR-0003-diagnostics-determinism-contract.md",
)
REQUIRED_RUNTIME_ARCH_PATH_TOKENS = (
    "`native/objc3c/src/driver/objc3_compilation_driver.cpp`",
    "`native/objc3c/src/io/objc3_process.cpp`",
    "`native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`",
    "`native/objc3c/src/ir/objc3_ir_emitter.cpp`",
    "`native/objc3c/src/runtime/objc3_runtime.h`",
    "`native/objc3c/src/runtime/objc3_runtime.cpp`",
    "`native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`",
    "`scripts/check_objc3c_runtime_acceptance.py`",
    "`scripts/check_objc3c_execution_replay_proof.ps1`",
    "`scripts/objc3c_public_workflow_runner.py`",
)

SOURCE_EXTENSIONS = (".h", ".hpp", ".hh", ".c", ".cc", ".cpp")
INCLUDE_PATTERN = re.compile(r'^\s*#\s*include\s+"([^"]+)"')

ALLOWED_DEPENDENCIES: dict[str, set[str]] = {
    "driver": {"libobjc3c_frontend", "io", "lower", "pipeline"},
    "libobjc3c_frontend": {"pipeline", "io"},
    "pipeline": {"lex", "parse", "sema", "lower", "ir", "io"},
    "lower": {"sema"},
    "ir": {"lower", "parse"},
    "io": {"lower", "pipeline"},
    "lex": set(),
    "parse": {"lex", "pipeline"},
    "sema": {"parse", "pipeline"},
}

EXEMPT_TOP_LEVEL_FILES = {"main.cpp", "ARCHITECTURE.md"}


def display_path(path: Path) -> str:
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def fail(message: str) -> None:
    raise ValueError(message)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Check include/link dependency contracts for native/objc3c frontend "
            "decomposition."
        )
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="alias for --mode strict for CI/task packets that require this flag.",
    )
    parser.add_argument(
        "--mode",
        choices=("contract", "strict"),
        default="strict",
        help="contract checks docs/ADR presence; strict also checks include directions.",
    )
    args = parser.parse_args()
    if args.strict:
        args.mode = "strict"
    return args


def ensure_contract_assets() -> None:
    if not ARCHITECTURE_DOC.exists():
        fail(f"missing architecture contract: {display_path(ARCHITECTURE_DOC)}")
    text = ARCHITECTURE_DOC.read_text(encoding="utf-8")
    for marker in REQUIRED_ARCH_SECTIONS:
        if marker not in text:
            fail(
                "architecture contract missing required section "
                f"'{marker}' in {display_path(ARCHITECTURE_DOC)}"
            )

    if not ADR_ROOT.exists():
        fail(f"missing ADR directory: {display_path(ADR_ROOT)}")
    for adr_name in REQUIRED_ADRS:
        adr_path = ADR_ROOT / adr_name
        if not adr_path.exists():
            fail(f"missing required ADR: {display_path(adr_path)}")

    if not RUNTIME_ARCHITECTURE_DOC.exists():
        fail(
            "missing runtime architecture contract: "
            f"{display_path(RUNTIME_ARCHITECTURE_DOC)}"
        )
    runtime_text = RUNTIME_ARCHITECTURE_DOC.read_text(encoding="utf-8")
    for marker in REQUIRED_RUNTIME_ARCH_SECTIONS:
        if marker not in runtime_text:
            fail(
                "runtime architecture contract missing required section "
                f"'{marker}' in {display_path(RUNTIME_ARCHITECTURE_DOC)}"
            )
    for token in REQUIRED_RUNTIME_ARCH_PATH_TOKENS:
        if token not in runtime_text:
            fail(
                "runtime architecture contract missing required path reference "
                f"{token} in {display_path(RUNTIME_ARCHITECTURE_DOC)}"
            )


def detect_module_for_source(path: Path) -> str | None:
    relative = path.relative_to(SRC_ROOT)
    first = relative.parts[0]
    if first in ALLOWED_DEPENDENCIES:
        return first
    if len(relative.parts) == 1 and relative.name in EXEMPT_TOP_LEVEL_FILES:
        return None
    return None


def detect_module_for_include(include_value: str, owner_path: Path) -> str | None:
    normalized = include_value.replace("\\", "/").lstrip("./")
    if normalized.startswith("../"):
        return None

    include_parts = Path(normalized).parts
    if not include_parts:
        return None

    candidate = include_parts[0]
    if candidate in ALLOWED_DEPENDENCIES:
        return candidate

    owner_dir_name = owner_path.parent.name
    if owner_dir_name in ALLOWED_DEPENDENCIES and candidate == owner_path.name:
        return owner_dir_name

    return None


def scan_sources() -> list[Path]:
    if not SRC_ROOT.exists():
        fail(f"missing source root: {display_path(SRC_ROOT)}")
    return sorted(
        path
        for path in SRC_ROOT.rglob("*")
        if path.is_file() and path.suffix.lower() in SOURCE_EXTENSIONS
    )


def check_include_dependencies() -> list[str]:
    violations: list[str] = []
    source_paths = scan_sources()

    for source_path in source_paths:
        owner_module = detect_module_for_source(source_path)
        if owner_module is None:
            continue
        allowed = ALLOWED_DEPENDENCIES[owner_module]

        for line_number, line in enumerate(source_path.read_text(encoding="utf-8").splitlines(), start=1):
            match = INCLUDE_PATTERN.match(line)
            if not match:
                continue
            include_path = match.group(1)
            dependency_module = detect_module_for_include(include_path, source_path)
            if dependency_module is None or dependency_module == owner_module:
                continue
            if dependency_module not in allowed:
                violations.append(
                    f"{display_path(source_path)}:{line_number} "
                    f"forbidden include '{include_path}' ({owner_module} -> {dependency_module})"
                )

    return violations


def run() -> int:
    args = parse_args()
    ensure_contract_assets()

    if args.mode == "contract":
        print("objc3c dependency boundary contract check passed.")
        return 0

    violations = check_include_dependencies()
    if violations:
        print("objc3c dependency boundary violations:")
        for violation in violations:
            print(f"- {violation}")
        return 1

    print("objc3c dependency boundaries check passed.")
    return 0


def main() -> None:
    try:
        raise SystemExit(run())
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2) from exc


if __name__ == "__main__":
    main()
