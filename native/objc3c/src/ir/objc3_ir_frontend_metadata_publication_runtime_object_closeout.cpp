#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_closeout.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeObjectCloseoutMetadataNodes(std::ostringstream &out) {
  out << "!64 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateFailureModel)
      << "\"}\n";
  out << "!65 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataObjectEmissionCloseoutContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel)
      << "\"}\n";
}
