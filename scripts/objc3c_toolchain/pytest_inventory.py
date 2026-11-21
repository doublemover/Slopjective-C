"""Export actual tooling collection, phase outcomes and explicit skip reasons."""
from __future__ import annotations

import json
from pathlib import Path


def pytest_addoption(parser):
    parser.addoption('--execution-inventory', help='write the actual selected tooling test inventory')


def pytest_configure(config):
    if config.getoption('--execution-inventory'):
        config._objc3_inventory = {'schema': 'objc3c.pytest-inventory.v1', 'execution': 'unit-contract',
                                   'selected': [], 'deselected': [], 'reports': [], 'collection_errors': []}


def pytest_collection_finish(session):
    inventory = getattr(session.config, '_objc3_inventory', None)
    if inventory is not None:
        inventory['selected'] = [item.nodeid for item in session.items]


def pytest_deselected(items):
    if items:
        inventory = getattr(items[0].config, '_objc3_inventory', None)
        if inventory is not None:
            inventory['deselected'].extend(item.nodeid for item in items)



class Reports:
    def __init__(self, inventory):
        self.inventory = inventory

    def pytest_runtest_logreport(self, report):
        row = {'nodeid': report.nodeid, 'phase': report.when, 'outcome': report.outcome,
               'duration_seconds': report.duration}
        if report.skipped or report.failed:
            row['reason'] = str(report.longrepr)
        if hasattr(report, 'wasxfail'):
            row['xfail'] = report.wasxfail
        self.inventory['reports'].append(row)

    def pytest_collectreport(self, report):
        if report.failed:
            self.inventory['collection_errors'].append({'nodeid': report.nodeid, 'reason': str(report.longrepr)})


def pytest_sessionstart(session):
    inventory = getattr(session.config, '_objc3_inventory', None)
    if inventory is not None:
        session.config.pluginmanager.register(Reports(inventory))


def pytest_sessionfinish(session, exitstatus):
    inventory = getattr(session.config, '_objc3_inventory', None)
    if inventory is not None:
        inventory['exit_code'] = int(exitstatus)
        inventory['complete'] = bool(inventory['selected']) and exitstatus == 0 and not inventory['collection_errors']
        target = Path(session.config.getoption('--execution-inventory'))
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(json.dumps(inventory, indent=2) + '\n')
