#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_runtime_gate.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IROwnershipRuntimeGateMetadataNode(std::ostringstream &out) {
  out << "!72 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateSupportedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\"}\n";
}
