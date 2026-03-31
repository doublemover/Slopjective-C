; ModuleID = 'm189_validation_task_runtime_interop_cancellation'
; authenticity_schema_id: objc3c.artifact.authenticity.schema.v1
; artifact_family_id: objc3c.fixture.synthetic.replayll.v1
; provenance_class: synthetic_fixture
; provenance_mode: fixture_curated
; content_role: replay_ir_fixture
; fixture_family_id: objc3c.fixture.synthetic.replayll.v1
; explicit_fixture_label: replay IR fixture
; synthetic_reason: deterministic replay artifact preserved for validation contracts
source_filename = "m189_validation_task_runtime_interop_cancellation.objc3"

; task_runtime_interop_cancellation_lowering = task_runtime_sites=9;task_runtime_interop_sites=5;cancellation_probe_sites=4;cancellation_handler_sites=3;runtime_resume_sites=4;runtime_cancel_sites=2;normalized_sites=7;guard_blocked_sites=2;contract_violation_sites=0;deterministic=true;lane_contract=m189-task-runtime-interop-cancellation-lowering-v1
; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites=9, task_runtime_interop_sites=5, cancellation_probe_sites=4, cancellation_handler_sites=3, runtime_resume_sites=4, runtime_cancel_sites=2, normalized_sites=7, guard_blocked_sites=2, contract_violation_sites=0, deterministic_task_runtime_interop_cancellation_lowering_handoff=true
!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}
!40 = !{i64 9, i64 5, i64 4, i64 3, i64 4, i64 2, i64 7, i64 2, i64 0, i1 1}
