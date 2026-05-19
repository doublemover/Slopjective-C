from __future__ import annotations

import json
from typing import Any

from objc3c_c_api_runner_extraction_sources import ROOT, read_text

C_API_RUNNER_CONTRACT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "frontend_c_api_runner_contract.json"
)


def load_c_api_runner_contract() -> dict[str, Any]:
    return json.loads(read_text(C_API_RUNNER_CONTRACT))
