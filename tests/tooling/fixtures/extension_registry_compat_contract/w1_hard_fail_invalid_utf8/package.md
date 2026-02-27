# V013-GOV-02 Package: Extension Registry Compatibility Validation Suite

| `seed_id` | `V013-GOV-02` |
| `acceptance_gate_id` | `AC-V013-GOV-02` |
| `CM-RC-07` | `unknown_major_input` | Unknown major from consumer perspective | Required-field interpretation is undefined | `fail` | `fail` | Hard fail closed and escalate under `E2`. | `VAL-RC-06` |
| `VAL-RC-06` | `rg -n "AC-V013-GOV-02|VAL-RC-|ESC-RC-" tests/governance/registry_compat/README.md` | Exit `0`; all required governance identifiers are present. | Treat test/readme contract as incomplete and block publish. |
3. Deterministic validator nondeterminism (same input producing conflicting outcomes).
| `ESC-RC-04` (`E4`) | Integrity-risk or policy breach with no safe workaround. | Steering owner + security owner | Immediate | Apply emergency hold and require superseding corrective artifact set. |
| `AC-V013-GOV-02-05` | Validation transcript for `python scripts/spec_lint.py` is recorded. | Section `7` includes command, output, and exit status. | This package Section `7`. |

Exit status: `0` (`PASS`)
