"""Exercise locked package authoring and expansion through the installed CLI."""
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile


def main():
    compiler = Path(sys.argv[1]).resolve(strict=True)
    with tempfile.TemporaryDirectory(prefix='objc3-meta-cli-') as directory:
        root = Path(directory)
        manifest = root / 'objc3-meta.json'
        package = {
            'contract_id': 'objc3c.meta.package.v1', 'id': 'fixture.cli', 'version': '1.0.0',
            'requires': {'meta_revision': 1, 'extension_abi': 24}, 'capabilities': [],
            'entrypoints': [{
                'name': 'Answer', 'kind': 'macro', 'requires': [],
                'declarations': [{'text': '- (int)answer;'}],
                'implementations': [{'text': '- (int)answer {return 42;}'}]}]}
        manifest.write_text(json.dumps(package))
        fingerprint = subprocess.check_output(
            [compiler, '--meta-package-fingerprint', manifest], text=True).strip()
        assert re.fullmatch('[a-f0-9]{64}', fingerprint)
        package['fingerprint'] = fingerprint
        manifest.write_text(json.dumps(package))
        lock = root / 'meta.lock'
        lock.write_text(json.dumps({'contract_id': 'objc3c.meta.lock.v1', 'packages': [{
            'id': package['id'], 'version': package['version'], 'fingerprint': fingerprint}]}))
        source = root / 'module.m'
        source.write_text('''
__attribute__((objc_root_class,objc_expand("fixture.cli::Answer")))
@interface CLIAnswer { Class isa; } @end
@implementation CLIAnswer @end
''')
        interface = root / 'fixture.CLI.objc3i'
        output = root / 'module.o'
        record = root / 'dependencies.json'
        subprocess.run([compiler, source, '--objc3', '--runtime', 'gnustep-2.2',
                        '--meta-package', manifest, '--meta-lock', lock,
                        '--emit-object', output, '--emit-interface', interface,
                        '--module', 'fixture.CLI', '--package', 'cli-v1',
                        '--export-declaration', 'CLIAnswer',
                        '--emit-dependency-record', record], check=True)
        assert output.stat().st_size and 'answer' in interface.read_text()
        dependencies = json.loads(record.read_text())['metaprogramming']
        assert {row['path'] for row in dependencies} == {str(manifest), str(lock)}
        identity = json.loads(Path(str(interface) + '.json').read_text())['module']
        manifest.unlink()
        lock.unlink()
        consumer = root / 'consumer.m'
        consumer.write_text('int use(CLIAnswer *value) {return [value answer];}')
        subprocess.run([compiler, consumer, '--objc3', '--runtime', 'gnustep-2.2',
                        '--module-search-path', root,
                        '--import-module', 'fixture.CLI', identity['semantic_fingerprint'], 'cli-v1'], check=True)


if __name__ == '__main__':
    main()
