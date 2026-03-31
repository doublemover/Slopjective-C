; ModuleID = 'm184_validation_unwind_cleanup'
; authenticity_schema_id: objc3c.artifact.authenticity.schema.v1
; artifact_family_id: objc3c.fixture.synthetic.replayll.v1
; provenance_class: synthetic_fixture
; provenance_mode: fixture_curated
; content_role: replay_ir_fixture
; fixture_family_id: objc3c.fixture.synthetic.replayll.v1
; explicit_fixture_label: replay IR fixture
; synthetic_reason: deterministic replay artifact preserved for validation contracts
source_filename = "m184_validation_unwind_cleanup.objc3"

; unwind_cleanup_lowering = unwind_cleanup_sites=7;exceptional_exit_sites=5;cleanup_action_sites=4;cleanup_scope_sites=3;cleanup_resume_sites=2;normalized_sites=5;fail_closed_sites=2;contract_violation_sites=0;deterministic=true;lane_contract=m184-unwind-safety-cleanup-emission-lowering-v1
; frontend_objc_unwind_cleanup_lowering_profile = unwind_cleanup_sites=7, exceptional_exit_sites=5, cleanup_action_sites=4, cleanup_scope_sites=3, cleanup_resume_sites=2, normalized_sites=5, fail_closed_sites=2, contract_violation_sites=0, deterministic_unwind_cleanup_lowering_handoff=true
!objc3.objc_unwind_cleanup_lowering = !{!44}
!44 = !{i64 7, i64 5, i64 4, i64 3, i64 2, i64 5, i64 2, i64 0, i1 1}
