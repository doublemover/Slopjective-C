from __future__ import annotations

import sys

from .path_bootstrap import install_workflow_import_roots

install_workflow_import_roots()

from scripts.objc3c_workflow.entrypoint_module import main


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
