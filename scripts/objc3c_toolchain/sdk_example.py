"""Rebuild the installed imports/managed-values/errors/defer example."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--installed-layout', action='store_true')
    args = parser.parse_args()
    example = Path(__file__).resolve().parent
    sdk = example.parents[3]
    output = args.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    manifest = json.loads((example / 'package-manifest-arc.json').read_text())
    identity = manifest['module_identity']
    environment = dict(os.environ)
    environment['LD_LIBRARY_PATH'] = str(sdk / 'lib')
    obj = output / 'objc3-managed-sample.o'
    executable = output / 'objc3-managed-sample'
    command = [str(sdk / 'bin/objc3c-clang'), str(example / 'main.m'), '--objc3', '--arc',
               '--target', identity['target_triple'], '--runtime', 'gnustep-2.0',
               '--source-imports', '--module-search-path', str(example), '--import-module',
               identity['canonical_name'], identity['semantic_fingerprint'], identity['package_identity'],
               '--emit-object', str(obj), '--', '-std=c17', '-fblocks', '-O2', '-g']
    subprocess.run(command, cwd=example, env=environment, check=True)
    rpath = '$ORIGIN/../lib' if args.installed_layout else str(sdk / 'lib')
    subprocess.run([str(sdk / 'bin/clang++'), '-fuse-ld=lld', str(obj), str(example / 'provider.obj'),
                    '-L' + str(sdk / 'lib'), '-lobjc', '-Wl,-rpath,' + rpath, '-o', str(executable)],
                   env=environment, check=True)
    environment.pop('LD_LIBRARY_PATH', None)
    subprocess.run([str(executable)], env=environment, check=True)
    print(json.dumps({'status': 'PASS', 'executable': str(executable)}))


if __name__ == '__main__':
    main()
