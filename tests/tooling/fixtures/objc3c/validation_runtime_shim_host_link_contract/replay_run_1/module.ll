; authenticity_schema_id: objc3c.artifact.authenticity.schema.v1
; artifact_family_id: objc3c.fixture.synthetic.replayll.v1
; provenance_class: synthetic_fixture
; provenance_mode: fixture_curated
; content_role: replay_ir_fixture
; fixture_family_id: objc3c.fixture.synthetic.replayll.v1
; explicit_fixture_label: replay IR fixture
; synthetic_reason: deterministic replay artifact preserved for validation contracts
; objc3c native frontend IR
; runtime_shim_host_link_lowering = message_send_sites=8;runtime_shim_required_sites=5;runtime_shim_elided_sites=3;runtime_dispatch_arg_slots=3;runtime_dispatch_declaration_parameter_count=5;runtime_dispatch_symbol=objc3_msgsend_i32;default_runtime_dispatch_symbol_binding=true;contract_violation_sites=0;deterministic=true;lane_contract=m160-runtime-shim-host-link-v1
; frontend_objc_runtime_shim_host_link_profile = message_send_sites=8, runtime_shim_required_sites=5, runtime_shim_elided_sites=3, runtime_dispatch_arg_slots=3, runtime_dispatch_declaration_parameter_count=5, runtime_dispatch_symbol=objc3_msgsend_i32, default_runtime_dispatch_symbol_binding=true, contract_violation_sites=0, deterministic_runtime_shim_host_link_handoff=true
!objc3.objc_runtime_shim_host_link = !{!13}
!13 = !{i64 8, i64 5, i64 3, i64 3, i64 5, !"objc3_msgsend_i32", i1 1, i64 0, i1 1}
