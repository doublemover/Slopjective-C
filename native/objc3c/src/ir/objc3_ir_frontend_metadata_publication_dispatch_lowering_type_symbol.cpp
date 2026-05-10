#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_type_symbol.h"

#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_abi_marshalling.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_class_protocol_symbol.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_selector.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_symbol_graph.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_type_spelling.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_typecheck.h"

void EmitObjc3IRTypeSymbolDispatchLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchLoweringTypeSpellingCounterNode(metadata, out);
  EmitObjc3IRDispatchLoweringSymbolGraphCounterNode(metadata, out);
  EmitObjc3IRDispatchLoweringClassProtocolSymbolCounterNode(metadata, out);
  EmitObjc3IRDispatchLoweringTypecheckCounterNode(metadata, out);
  EmitObjc3IRDispatchLoweringSelectorCounterNode(metadata, out);
  EmitObjc3IRDispatchLoweringAbiMarshallingCounterNode(metadata, out);
}
