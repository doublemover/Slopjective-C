from __future__ import annotations

from .config import load_contract_config
from .constants import (
    ALLOWED_SRC_FILES,
    POLICY_README_PATH,
    REQUIRED_POLICY_TOKENS,
    SRC_DIR,
)
from .models import ContractConfig


def find_unknown_src_files() -> list[str]:
    unknown: list[str] = []
    if not SRC_DIR.is_dir():
        return [str(SRC_DIR)]
    for path in sorted(SRC_DIR.glob("*")):
        if not path.is_file():
            continue
        if path.name not in ALLOWED_SRC_FILES:
            unknown.append(path.name)
    return unknown


def validate_policy_readme() -> list[str]:
    errors: list[str] = []
    if not POLICY_README_PATH.is_file():
        return [f"missing policy README: {POLICY_README_PATH}"]

    text = POLICY_README_PATH.read_text(encoding="utf-8")
    for token in REQUIRED_POLICY_TOKENS:
        if token not in text:
            errors.append(f"policy README missing token: {token}")
    return errors


def validate_contract_inputs() -> tuple[ContractConfig | None, list[str]]:
    errors = validate_policy_readme()
    unknown_src_files = find_unknown_src_files()
    if unknown_src_files:
        errors.append(
            "unexpected files under site/src: " + ", ".join(unknown_src_files)
        )

    config, config_errors = load_contract_config()
    errors.extend(config_errors)
    return config, errors
