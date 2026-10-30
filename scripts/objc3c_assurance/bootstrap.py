"""Preserve clean-host setup failures before native execution can start."""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess

from scripts.objc3c_toolchain.binary_packages import write_json
from .evidence import artifact


def setup(repo: Path, root: Path, output: Path) -> int:
    output = output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    report = {'schema': 'objc3c.native-assurance.bootstrap.v1', 'status': 'running',
              'exit_code': None, 'artifacts': []}
    receipt = output / 'result.json'
    log = output / 'setup.log'
    write_json(receipt, report)
    try:
        with log.open('w') as stream:
            result = subprocess.run(['bash', str(repo / 'scripts/setup_cloud_environment.sh')], cwd=repo,
                env={**os.environ, 'OBJC3C_CLOUD_SETUP_ROOT': str(root)}, stdout=stream, stderr=subprocess.STDOUT)
        report.update(status='PASS' if result.returncode == 0 else 'FAIL', exit_code=result.returncode)
        if result.returncode:
            print('\n'.join(log.read_text(errors='replace').splitlines()[-40:]), flush=True)
        return result.returncode if result.returncode >= 0 else 128-result.returncode
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        for path in sorted((root / 'logs').glob('*.log')):
            shutil.copyfile(path, output / path.name)
        report['artifacts'] = [artifact(output, path) for path in sorted(output.glob('*.log'))]
        write_json(receipt, report)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    return setup(args.repo.resolve(), args.root.resolve(), args.output)


if __name__ == '__main__':
    raise SystemExit(main())
