"""Check the task header ABI cutover and an Objective-C-free C++ task link."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess
import tempfile

from scripts.objc3c_toolchain.binary_packages import digest, disjoint, write_json
from scripts.objc3c_toolchain.sdk_example import environment


CPP_SOURCE = r'''#include <objc3c/runtime/objc3_task.hpp>
objc3::task<int> answer() { co_return 42; }
int main() {
  objc3_executor_t *executor = nullptr;
  if (objc3_executor_create(1, &executor) != OBJC3_SERVICE_OK) return 1;
  auto task = answer().start(executor);
  uint32_t count = 0;
  if (objc3_executor_pump(executor, 10, &count) != OBJC3_SERVICE_OK ||
      task.result() != 42) return 2;
  task = {};
  objc3_executor_release(executor);
}
'''


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    commands = []
    report = {'schema': 'objc3c.async-checked-abi.v1', 'status': 'FAIL',
              'sdk_seal': json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256'],
              'compiler_sha256': digest((sdk / 'bin/clang').resolve()),
              'runtime_library_sha256': digest(sdk / 'lib/libobjc3_runtime_services.a'),
              'header_sha256': digest(sdk / 'include/objc3c/runtime/objc3_task.hpp'),
              'harness_sha256': digest(Path(__file__)), 'commands': commands,
              'cpp_source': CPP_SOURCE}

    def run(label, argv, *, reject=False):
        result = subprocess.run(list(map(str, argv)), env=environment(sdk),
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                text=True, timeout=90)
        log = output / (label + '.log')
        log.write_text(result.stdout)
        commands.append({'name': label, 'argv': list(map(str, argv)),
                         'exit_code': result.returncode, 'expected_rejection': reject,
                         'log_sha256': digest(log)})
        if (result.returncode == 0) == reject:
            raise ValueError('checked async ABI check failed: ' + label)
        return result.stdout

    try:
        with tempfile.TemporaryDirectory(prefix='objc3 checked ABI ') as temporary:
            root = Path(temporary)
            source = root / 'task.cpp'
            source.write_text(CPP_SOURCE)
            executable = root / 'task'
            run('cpp-link', [sdk / 'bin/clang++', '--no-default-config', '-std=c++20',
                '-O2', '-fcoro-aligned-allocation', '-I' + str(sdk / 'include'),
                source, sdk / 'lib/libobjc3_runtime_services.a', '-pthread',
                '-fuse-ld=lld',
                '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff',
                '-o', executable])
            run('cpp-execute', [executable])
            needed = run('cpp-needed-libraries', [sdk / 'bin/llvm-readobj',
                                                 '--needed-libs', executable])
            if 'libobjc' in needed:
                raise ValueError('ordinary C++ task unexpectedly links Objective-C')
            probe = root / 'stale.mm'
            probe.write_text('#include <objc3c/runtime/objc3_task.hpp>\n')
            rejected = run('stale-header-abi', [sdk / 'bin/clang++', '--no-default-config',
                '-std=c++20', '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2', '-fblocks',
                '-fobjc-arc', '-I' + str(sdk / 'include'), '-fsyntax-only',
                '-U__OBJC3_EXTENSION_ABI_REVISION__',
                '-D__OBJC3_EXTENSION_ABI_REVISION__=19', probe], reject=True)
            if 'task bridge requires Objective-C 3 extension ABI 20' not in rejected:
                raise ValueError('stale ABI rejected without the task header diagnostic')
        report.update(status='PASS', stale_abi_rejected=True,
                      pure_cpp_requires_objc=False)
    except Exception as error:
        report['error'] = str(error)
        raise
    finally:
        write_json(receipt, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.output_dir)
    print(json.dumps({'status': result['status'], 'commands': len(result['commands'])}))


if __name__ == '__main__':
    main()
