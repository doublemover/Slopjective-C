function Get-Objc3cNativeFrontendModules {
  return @(
    [ordered]@{
      name = "driver"
      sources = @(
        "native/objc3c/src/driver/objc3_cli_options.cpp"
        "native/objc3c/src/driver/objc3_driver_main.cpp"
        "native/objc3c/src/driver/objc3_driver_shell.cpp"
        "native/objc3c/src/driver/objc3_frontend_options.cpp"
        "native/objc3c/src/driver/objc3_llvm_capability_routing.cpp"
        "native/objc3c/src/driver/objc3_objc3_path.cpp"
        "native/objc3c/src/driver/objc3_objectivec_path.cpp"
        "native/objc3c/src/driver/objc3_compilation_driver.cpp"
      )
    }
    [ordered]@{
      name = "diagnostics-io"
      sources = @(
        "native/objc3c/src/diag/objc3_diag_utils.cpp"
        "native/objc3c/src/io/objc3_diagnostics_artifacts.cpp"
        "native/objc3c/src/io/objc3_file_io.cpp"
        "native/objc3c/src/io/objc3_json.cpp"
        "native/objc3c/src/io/objc3_manifest_artifacts.cpp"
        "native/objc3c/src/io/objc3_process.cpp"
      )
    }
    [ordered]@{
      name = "ir"
      sources = @(
        "native/objc3c/src/ir/objc3_ir_emitter.cpp"
      )
    }
    [ordered]@{
      name = "lex-parse"
      sources = @(
        "native/objc3c/src/lex/objc3_lexer.cpp"
        "native/objc3c/src/parse/objc3_ast_builder.cpp"
        "native/objc3c/src/parse/objc3_ast_builder_contract.cpp"
        "native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp"
        "native/objc3c/src/parse/objc3_diagnostic_source_precision_scaffold.cpp"
        "native/objc3c/src/parse/objc3_parse_support.cpp"
        "native/objc3c/src/parse/objc3_parser.cpp"
      )
    }
    [ordered]@{
      name = "frontend-api"
      sources = @(
        "native/objc3c/src/libobjc3c_frontend/c_api.cpp"
        "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp"
        "native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp"
      )
    }
    [ordered]@{
      name = "lowering"
      sources = @(
        "native/objc3c/src/lower/objc3_lowering_contract.cpp"
      )
    }
    [ordered]@{
      name = "pipeline"
      sources = @(
        "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp"
        "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"
        "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
        "native/objc3c/src/pipeline/objc3_ir_emission_completeness_scaffold.cpp"
        "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp"
        "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp"
      )
    }
    [ordered]@{
      name = "sema"
      sources = @(
        "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
        "native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp"
        "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
        "native/objc3c/src/sema/objc3_semantic_passes.cpp"
        "native/objc3c/src/sema/objc3_type_form_scaffold.cpp"
        "native/objc3c/src/sema/objc3_static_analysis.cpp"
        "native/objc3c/src/sema/objc3_pure_contract.cpp"
      )
    }
  )
}
