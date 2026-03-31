# M314-D001 Workflow Integration Report

- package_script_count: `143`
- public_script_count: `143`
- operator_script_count: `135`
- maintainer_script_count: `8`
- runner_mode: `public_runner-parameterized-task-runner-v2`

## Operator examples
- `build:docs:commands` -> `build-public-command-surface` (`operator`, `build`)
- `check:docs:commands` -> `check-public-command-surface` (`operator`, `check`)
- `check:repo:surface` -> `check-repo-superclean-surface` (`operator`, `check`)

## Maintainer public examples
- `check:objc3c:boundaries` -> `check-dependency-boundaries` (`maintainer`, `check`)
- `check:task-hygiene` -> `check-task-hygiene` (`maintainer`, `check`)
- `lint` -> `lint-default` (`maintainer`, `lint`)

## Internal maintainer actions
- `build-public-command-contract` -> `python:scripts/build_objc3c_public_command_contract.py` (`internal`, `internal`)
- `check-public-command-contract` -> `python:scripts/build_objc3c_public_command_contract.py --check` (`internal`, `internal`)
- `check-public-command-budget` -> `python:scripts/check_objc3c_public_command_budget.py` (`internal`, `internal`)

## Documentation assertions
- `maintainer_runbook_mentions_build_public_command_contract`: `True`
- `maintainer_runbook_mentions_check_public_command_contract`: `True`
- `maintainer_runbook_mentions_check_public_command_budget`: `True`
- `maintainer_runbook_uses_wrapper_for_dependency_boundaries`: `True`
- `maintainer_runbook_uses_wrapper_for_task_hygiene`: `True`
- `maintainer_runbook_uses_wrapper_for_external_validation_surface`: `True`
- `readme_mentions_internal_maintainer_actions`: `True`

Next issue: `M314-D002`
