from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_objc3c_package_registry_mirror_reproducibility import (  # noqa: E402
    collect_interop_loader_metadata_failures,
)
from package_ecosystem_contracts import (  # noqa: E402
    PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    load_package_loader_interop_metadata,
    normalize_package_loader_interop_metadata,
    package_loader_metadata_by_package,
)


def normalized_interop_metadata() -> dict[str, dict[str, object]]:
    payload = load_package_loader_interop_metadata(ROOT)
    by_package = package_loader_metadata_by_package(
        payload,
        root=ROOT,
        package_ids={"showcase:patchKit", "stdlib:objc3.system"},
    )
    return {
        package_id: normalize_package_loader_interop_metadata(entry)
        for package_id, entry in by_package.items()
    }


def test_package_loader_interop_fixture_covers_headers_abi_and_mixed_images() -> None:
    metadata = normalized_interop_metadata()

    patchkit = metadata["showcase:patchKit"]
    assert "ParserKit/ForeignEntry.h" in patchkit["header_imports"]
    assert "ParserKit/ParserKit.h" in patchkit["header_exports"]
    assert patchkit["abi_alignment"][0]["bytes"] == 16
    assert {image["metadata_section"] for image in patchkit["mixed_images"]} == {
        "__objc3c_interop",
        "__objc3c_package",
    }
    assert patchkit["tamper_rejection"]["diagnostic_code"] == PACKAGE_LOADER_INTEROP_TAMPER_CODE

    system = metadata["stdlib:objc3.system"]
    assert "BridgeProvider.h" in system["header_imports"]
    assert system["negative_diagnostics"][0]["diagnostic_code"] == "O3PKG8052"


def test_package_loader_interop_tamper_mismatch_reports_stable_code() -> None:
    metadata = normalized_interop_metadata()
    lock = {
        "packages": [
            {
                "package_id": "showcase:patchKit",
                "interop_loader_metadata": metadata["showcase:patchKit"],
            }
        ]
    }
    mirror_metadata = deepcopy(metadata["showcase:patchKit"])
    mirror_metadata["digest"] = "sha256:" + ("0" * 64)
    mirror = {
        "integrity_policy": {
            "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        },
        "packages": [
            {
                "package_id": "showcase:patchKit",
                "interop_loader_metadata": mirror_metadata,
            }
        ],
    }
    registry = deepcopy(lock)
    publication = {
        "interop_loader_support": "local-mixed-image-metadata-digest-checked",
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    }

    failures = collect_interop_loader_metadata_failures(lock, mirror, registry, publication)

    assert failures == [
        f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror interop metadata digest mismatch for showcase:patchKit"
    ]
