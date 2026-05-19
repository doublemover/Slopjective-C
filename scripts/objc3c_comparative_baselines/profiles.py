"""Host profile capture for comparative baseline telemetry."""

from __future__ import annotations

import os
import platform
import socket
from typing import Any


def machine_profile() -> dict[str, Any]:
    cpu_model = (
        os.environ.get("PROCESSOR_IDENTIFIER")
        or platform.processor()
        or os.environ.get("PROCESSOR_ARCHITECTURE", "")
    )
    return {
        "hostname": socket.gethostname(),
        "os": platform.platform(),
        "arch": platform.machine() or os.environ.get("PROCESSOR_ARCHITECTURE", ""),
        "cpu_model": cpu_model,
        "cpu_count": os.cpu_count() or 1,
        "python_version": platform.python_version(),
    }
