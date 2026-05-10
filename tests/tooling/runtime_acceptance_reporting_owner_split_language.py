from runtime_acceptance_reporting_owner_split_support import (
    FORBIDDEN_REPORTING_WORDS,
    reporting_files,
)


def runtime_acceptance_reporting_lane_avoids_retired_language() -> None:
    for path in reporting_files():
        text = path.read_text(encoding="utf-8").lower()
        for forbidden in FORBIDDEN_REPORTING_WORDS:
            assert forbidden not in text, f"{forbidden} leaked through {path}"
