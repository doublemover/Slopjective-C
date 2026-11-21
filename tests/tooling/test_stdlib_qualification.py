"""The installed library gate must preserve evidence and reject stale success."""
import json
from pathlib import Path
import re
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_stdlib as qualification


def test_partial_modules_track_release_evidence_not_undefined_extensions():
    inventory_path = (Path(__file__).resolve().parents[2] /
                      'stdlib/module_inventory.json')
    inventory = json.loads(inventory_path.read_text())
    partial = [row for row in inventory['canonical_modules']
               if row['status'] == 'partial']
    assert {row['module'] for row in partial} == {
        'objc3.concurrency', 'objc3.keypath', 'objc3.system'
    }
    for module in partial:
        remaining = module['remaining']
        assert remaining and all('evidence' in item for item in remaining)
        joined = ' '.join(remaining)
        assert 'effectful and isolated paths' not in joined
        assert 'implicit union active-arm ownership' not in joined
        assert 'complete strict concurrency' not in joined


@pytest.mark.parametrize('module,prefix', [
    ('objc3.core', 'OBJC3_STDLIB_VERSION'),
    ('objc3.keypath', 'OBJC3_KEYPATH_MODULE_SEMVER'),
])
def test_public_module_version_macros_match_inventory(module, prefix):
    stdlib = Path(__file__).resolve().parents[2] / 'stdlib'
    inventory = json.loads((stdlib / 'module_inventory.json').read_text())
    owner = next(row for row in inventory['canonical_modules'] if row['module'] == module)
    header = (stdlib / owner['header']).read_text()
    for component in ('major', 'minor', 'patch'):
        value = re.search(r'^#define ' + prefix + '_' + component.upper() +
                          r' (\d+)u$', header, re.MULTILINE)
        assert value and int(value[1]) == owner['module_semver'][component], (module, component)


@pytest.mark.parametrize('failed_command', (1, 2, 3, 4))
def test_failed_library_gate_replaces_success_and_reclaims_temporary_files(
        tmp_path, monkeypatch, failed_command):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/stdlib'
    fixture.mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    for name in ('provider.h', 'provider.m', 'client.m', 'factory-contract.m', 'system-contract.m'):
        (fixture / name).write_text('fixture')
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    (sdk / 'bin').mkdir()
    (sdk / 'bin/clang').write_text('compiler')
    (sdk / 'lib').mkdir()
    (sdk / 'lib/libobjc3_runtime_services.a').write_text('runtime')
    calls, roots = [], []

    def run(argv, **kwargs):
        calls.append(argv)
        roots.append(kwargs['cwd'])
        if len(calls) > 1:
            assert not (kwargs['cwd'] / 'provider.m').exists()
        kwargs['stdout'].write('forced failure\n')
        return SimpleNamespace(returncode=int(len(calls) == failed_command))

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='standard library failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert len(report['commands']) == failed_command
    assert report['commands'][-1]['exit_code'] == 1
    assert roots and all(not root.exists() for root in roots)
    record = report['commands'][-1]
    assert record['log_sha256'] == qualification.packages.digest(
        output / (record['name'] + '.log'))
