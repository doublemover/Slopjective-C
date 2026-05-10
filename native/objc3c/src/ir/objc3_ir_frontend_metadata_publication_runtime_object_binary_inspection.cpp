#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_binary_inspection.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeObjectBinaryInspectionMetadataNode(
    std::ostringstream &out) {
  out << "!60 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionHarnessContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionPositiveCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionNegativeCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSectionCommand)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSymbolCommand)
      << "\", i64 4, i64 1}\n";
}
