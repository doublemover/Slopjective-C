; ModuleID = 'm181_validation_throws_propagation'
; authenticity_schema_id: objc3c.artifact.authenticity.schema.v1
; artifact_family_id: objc3c.fixture.synthetic.replayll.v1
; provenance_class: synthetic_fixture
; provenance_mode: fixture_curated
; content_role: replay_ir_fixture
; fixture_family_id: objc3c.fixture.synthetic.replayll.v1
; explicit_fixture_label: replay IR fixture
; synthetic_reason: deterministic replay artifact preserved for validation contracts
source_filename = "m181_validation_throws_propagation.objc3"

; throws_propagation_lowering = throws_propagation_sites=5;throws_clause_sites=2;throws_annotation_sites=2;throws_path_sites=1;propagating_call_sites=3;normalized_sites=4;propagation_boundary_sites=1;contract_violation_sites=0;deterministic=true;lane_contract=m181-throws-propagation-lowering-v1
; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites=5, throws_clause_sites=2, throws_annotation_sites=2, throws_path_sites=1, propagating_call_sites=3, normalized_sites=4, propagation_boundary_sites=1, contract_violation_sites=0, deterministic_throws_propagation_lowering_handoff=true
!objc3.objc_throws_propagation_lowering = !{!34}
!34 = !{i64 5, i64 2, i64 2, i64 1, i64 3, i64 4, i64 1, i64 0, i1 1}
