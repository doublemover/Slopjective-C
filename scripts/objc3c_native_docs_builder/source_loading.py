from __future__ import annotations

from pathlib import Path

from .constants import (
    FRAGMENT_ORDER,
    LINT_DISABLE_LINE,
    ORDER_LINE_RE,
    README_PATH,
    REQUIRED_HEADINGS,
    SRC_DIR,
)
from .models import ContractCheckResult


def parse_order_from_readme(text: str) -> list[str]:
    observed: list[str] = []
    for raw_line in text.splitlines():
        match = ORDER_LINE_RE.match(raw_line.strip())
        if match:
            observed.append(match.group(1))
    return observed


def validate_readme_contract(text: str) -> list[str]:
    errors: list[str] = []

    for heading in REQUIRED_HEADINGS:
        if heading not in text:
            errors.append(f"missing required README heading: {heading}")

    for fragment in FRAGMENT_ORDER:
        token = f"`{fragment}`"
        if token not in text:
            errors.append(f"README missing fragment reference: {token}")

    observed_order = parse_order_from_readme(text)
    if observed_order != list(FRAGMENT_ORDER):
        errors.append(
            "README stitch order drift: "
            f"expected {list(FRAGMENT_ORDER)} observed {observed_order}"
        )

    if len(observed_order) != len(set(observed_order)):
        errors.append("README stitch order contains duplicate fragment entries")

    return errors


def find_unknown_fragments() -> list[str]:
    known = set(FRAGMENT_ORDER) | {"README.md", "OWNERSHIP.md"}
    unknown = [
        path.name
        for path in sorted(SRC_DIR.glob("*.md"))
        if path.name not in known
    ]
    return unknown


def required_fragment_paths() -> tuple[list[Path], list[str]]:
    paths: list[Path] = []
    missing: list[str] = []
    for name in FRAGMENT_ORDER:
        path = SRC_DIR / name
        if path.is_file():
            paths.append(path)
        else:
            missing.append(name)
    return paths, missing


def validate_source_contract(*, allow_missing_fragments: bool) -> ContractCheckResult:
    errors: list[str] = []
    warnings: list[str] = []

    if not README_PATH.is_file():
        errors.append(f"missing contract README: {README_PATH}")
    else:
        readme_text = README_PATH.read_text(encoding="utf-8")
        errors.extend(validate_readme_contract(readme_text))

    required_paths, missing = required_fragment_paths()
    if missing and not allow_missing_fragments:
        errors.append(
            "missing required fragment files: "
            + ", ".join(missing)
        )
    elif missing:
        warnings.append(
            "missing fragments tolerated in --check-contract mode: "
            + ", ".join(missing)
        )

    unknown_fragments = find_unknown_fragments()
    if unknown_fragments:
        errors.append(
            "unexpected fragments in docs source dir: "
            + ", ".join(unknown_fragments)
        )

    return ContractCheckResult(
        errors=errors,
        warnings=warnings,
        required_paths=required_paths,
        missing_fragments=missing,
    )


def read_fragment_bytes(path: Path) -> bytes:
    data = path.read_bytes()
    if data.startswith(LINT_DISABLE_LINE):
        first_newline = data.find(b"\n")
        if first_newline != -1:
            data = data[first_newline + 1 :]
            if data.startswith(b"\r\n"):
                data = data[2:]
            elif data.startswith(b"\n"):
                data = data[1:]
    if not data.endswith((b"\n", b"\r")):
        data += b"\n"
    return data


def stitch_fragments(paths: list[Path]) -> bytes:
    return b"".join(read_fragment_bytes(path) for path in paths)
