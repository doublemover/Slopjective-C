#include "parse/objc3_parser_core_block_literal_signature_parameters_diagnostics_recovery.inc"
#include "parse/objc3_parser_core_block_literal_signature_parameters_signature_boundaries.inc"
#include "parse/objc3_parser_core_block_literal_signature_parameters_parameter_item_parsing.inc"
#include "parse/objc3_parser_core_block_literal_signature_parameters_metadata_publication.inc"
#include "parse/objc3_parser_core_block_literal_signature_parameters_finalization.inc"

  bool ParseBlockLiteralParameterList(
      std::vector<Objc3BlockParameterSourceModel> &parameters) {
    if (!MatchBlockLiteralParameterListOpening()) {
      return true;
    }

    if (!AtBlockLiteralParameterListClosing()) {
      if (!ParseBlockLiteralParameterItems(parameters)) {
        return false;
      }
    }
    return ConsumeBlockLiteralParameterListClosing();
  }

  void ApplyBlockLiteralSignatureModel(
      Expr &block,
      const std::vector<Objc3BlockParameterSourceModel> &parameters,
      std::vector<std::string> &parameter_names) {
    PublishBlockLiteralParameterNameModel(block, parameters, parameter_names);
    PublishBlockLiteralSignatureMetadata(block, parameters);
    PublishBlockLiteralParameterSourceOrder(block, parameters);
    FinalizeBlockLiteralSignatureProfile(block, parameters);
  }
