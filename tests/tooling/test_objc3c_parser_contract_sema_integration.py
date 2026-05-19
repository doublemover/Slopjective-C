from pathlib import Path
import sys

TEST_ROOT = Path(__file__).resolve().parent
if str(TEST_ROOT) not in sys.path:
    sys.path.insert(0, str(TEST_ROOT))

from objc3c_parser_sema_build_artifact_boundaries import *  # noqa: F401,F403
from objc3c_parser_sema_contract_boundaries import *  # noqa: F401,F403
from objc3c_parser_sema_hardening_matrix import *  # noqa: F401,F403
