#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend::runtime_import_preservation {

struct RuntimeAwareImportModuleSurfaceArtifactBuilder {
  [[nodiscard]] static std::string BuildReplayKey(
      const Objc3Program &program,
      const Objc3ParserContractSnapshot &parser_contract_snapshot,
      const Objc3ModuleImportGraphLoweringContract
          &module_import_graph_lowering_contract);

  [[nodiscard]] static std::string BuildSummaryJson(
      const Objc3Program &program,
      const Objc3ParserContractSnapshot &parser_contract_snapshot,
      const Objc3ModuleImportGraphLoweringContract
          &module_import_graph_lowering_contract);
};

struct RuntimeAwareImportModuleFrontendClosureArtifactBuilder {
  [[nodiscard]] static std::string BuildReplayKey(
      const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary);

  [[nodiscard]] static Objc3RuntimeAwareImportModuleFrontendClosureSummary
  BuildSummary(
      const Objc3Program &program,
      const Objc3ParserContractSnapshot &parser_contract_snapshot,
      const Objc3ModuleImportGraphLoweringContract
          &module_import_graph_lowering_contract,
      const Objc3RuntimeMetadataSourceRecordSet
          &runtime_metadata_source_records);
};

}  // namespace objc3::artifacts::frontend::runtime_import_preservation
