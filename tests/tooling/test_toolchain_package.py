import json
import os
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.package import MANIFEST, SCHEMA, _digest, inventory, normalize_permissions, verify_sdk

pytestmark = pytest.mark.skipif(os.name != "posix", reason="Linux SDK filesystem seal qualification")


def sealed(tmp_path):
    prefix = tmp_path / "sdk"
    (prefix / "bin").mkdir(parents=True)
    (prefix / "bin/clang").write_bytes(b"compiler")
    (prefix / "bin/clang").chmod(0o755)
    (prefix / "bin/clang++").symlink_to("clang")
    normalize_permissions(prefix)
    data = {"schema": SCHEMA, "files": inventory(prefix)}
    data["seal_sha256"] = _digest(data)
    (prefix / MANIFEST).write_text(json.dumps(data))
    (prefix / MANIFEST).chmod(0o644)
    return prefix, data["seal_sha256"]


def test_relocated_sdk_verifies(tmp_path):
    prefix, seal = sealed(tmp_path)
    destination = tmp_path / "relocated"
    prefix.rename(destination)
    assert verify_sdk(destination, seal)["seal_sha256"] == seal


@pytest.mark.parametrize("damage", ["bytes", "mode", "symlink", "extra", "manifest", "expected",
                                    "directory_mode", "root_mode", "manifest_mode"])
def test_sdk_seal_rejects_changes(tmp_path, damage):
    prefix, seal = sealed(tmp_path)
    compiler = prefix / "bin/clang"
    if damage == "bytes":
        stamp = compiler.stat()
        compiler.write_bytes(b"tampered")
        os.utime(compiler, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    elif damage == "mode":
        compiler.chmod(0o644)
    elif damage == "symlink":
        (prefix / "bin/clang++").unlink()
        (prefix / "bin/clang++").symlink_to("../bin/clang")
    elif damage == "extra":
        (prefix / "bin/unrecorded").write_text("extra")
    elif damage == "manifest":
        data = json.loads((prefix / MANIFEST).read_text())
        data["files"] = {}
        (prefix / MANIFEST).write_text(json.dumps(data))
    elif damage == "directory_mode":
        (prefix / 'bin').chmod(0o700)
    elif damage == "root_mode":
        prefix.chmod(0o700)
    elif damage == "manifest_mode":
        (prefix / MANIFEST).chmod(0o600)
    else:
        seal = "0" * 64
    with pytest.raises(ValueError):
        verify_sdk(prefix, seal)


def test_release_permissions_do_not_inherit_a_private_build_umask(tmp_path):
    prefix = tmp_path / 'sdk'
    directory = prefix / 'lib'
    directory.mkdir(parents=True, mode=0o700)
    library = directory / 'library.a'
    library.write_text('library')
    library.chmod(0o600)
    executable = directory / 'tool'
    executable.write_text('tool')
    executable.chmod(0o700)
    link = directory / 'alias'
    link.symlink_to('tool')
    normalize_permissions(prefix)
    assert prefix.stat().st_mode & 0o777 == 0o755
    assert directory.stat().st_mode & 0o777 == 0o755
    assert library.stat().st_mode & 0o777 == 0o644
    assert executable.stat().st_mode & 0o777 == 0o755
    assert link.is_symlink()


def test_seal_rejects_external_symbolic_link(tmp_path):
    prefix, _ = sealed(tmp_path)
    outside = tmp_path / "external"
    outside.write_text("external dependency")
    (prefix / "bin/external").symlink_to(outside)
    with pytest.raises(ValueError, match="escapes"):
        inventory(prefix)


def test_seal_rejects_absolute_internal_link_that_cannot_relocate(tmp_path):
    prefix, _ = sealed(tmp_path)
    (prefix / "bin/absolute").symlink_to(prefix / "bin/clang")
    with pytest.raises(ValueError, match="escapes"):
        inventory(prefix)


def test_independent_release_gates_execute_concurrently(tmp_path):
    import subprocess
    import sys
    from scripts.objc3c_toolchain.package import _run_qualifications

    script = '''
from pathlib import Path
import sys, time
root, name, peer = Path(sys.argv[1]), sys.argv[2], sys.argv[3]
(root / name).write_text('started')
deadline = time.monotonic() + 5
while not (root / peer).exists():
    if time.monotonic() > deadline:
        raise SystemExit('release gates ran serially')
    time.sleep(.01)
'''
    def gate(name, peer):
        subprocess.run([sys.executable, '-B', '-c', script, str(tmp_path), name, peer],
                       check=True, timeout=10)
        return {'status': 'PASS'}

    result = _run_qualifications({'first': lambda: gate('first', 'second'),
                                  'second': lambda: gate('second', 'first')}, 2)
    assert set(result) == {'first', 'second'}


def test_release_failure_joins_other_gates_and_preserves_their_evidence(tmp_path):
    import subprocess
    import sys
    from scripts.objc3c_toolchain.package import _run_qualifications

    def failed():
        raise ValueError('failed source import; see import.log')

    def completed():
        subprocess.run([sys.executable, '-B', '-c',
                        'import pathlib,sys,time; time.sleep(.1); pathlib.Path(sys.argv[1]).write_text("PASS")',
                        str(tmp_path / 'qualification.json')], check=True, timeout=10)
        return {'status': 'PASS'}

    with pytest.raises(ValueError, match='import: failed source import; see import.log'):
        _run_qualifications({'import': failed, 'calls': completed,
                             'queued': lambda: pytest.fail('started after failure')}, 2)
    assert (tmp_path / 'qualification.json').read_text() == 'PASS'


@pytest.mark.parametrize('damage', ['status', 'seal', 'count', 'command'])
def test_release_gate_rejects_successful_process_with_invalid_receipt(tmp_path, damage):
    from scripts.objc3c_toolchain.package import _qualify_call_suite

    sdk = tmp_path / 'sdk'
    examples = sdk / 'share/objc3c/examples/native'
    examples.mkdir(parents=True)
    evidence = {'status': 'PASS', 'sdk_seal': 'sealed', 'executions': [{}],
                'commands': [{'exit_code': 0, 'expected_exit_code': 0}]}
    if damage == 'status':
        evidence['status'] = 'FAIL'
    elif damage == 'seal':
        evidence['sdk_seal'] = 'other'
    elif damage == 'count':
        evidence['executions'] = []
    else:
        evidence['commands'][0]['exit_code'] = 1
    (examples / 'qualify-probe.py').write_text(
        'import json,pathlib,sys\n'
        'output = pathlib.Path(sys.argv[sys.argv.index("--output-dir") + 1])\n'
        f'(output / "qualification.json").write_text({json.dumps(evidence)!r})\n')
    with pytest.raises(ValueError, match='receipt is incomplete'):
        _qualify_call_suite(sdk, tmp_path / 'evidence', 'sealed', 'probe', {'executions': 1})
