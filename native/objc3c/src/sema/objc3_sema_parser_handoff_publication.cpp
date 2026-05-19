#include "sema/objc3_sema_parser_handoff_publication.h"

#include <sstream>

namespace {

#include "sema/objc3_sema_parser_handoff_publication_evidence.inc"
#include "sema/objc3_sema_parser_handoff_publication_transfer.inc"
#include "sema/objc3_sema_parser_handoff_publication_result.inc"

}  // namespace

Objc3SemaParserHandoffPublication PublishObjc3ParserSemaHandoff(
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result) {
  Objc3SemaParserHandoffPublication publication;

  publication.owner_record = handoff.owner_record;
  publication.evidence_record =
      BuildObjc3ParserSemaHandoffPublicationEvidenceRecord(handoff);
  publication.transfer_record =
      BuildObjc3ParserSemaHandoffPublicationTransferRecord(
          handoff, publication.evidence_record);
  publication.owner_record_deterministic =
      publication.transfer_record.owner_record_ready;
  publication.evidence_record_deterministic =
      publication.transfer_record.evidence_record_ready;
  publication.transfer_record_deterministic =
      IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(
          publication.transfer_record);

  PopulateObjc3ParserSemaHandoffPublicationResult(
      handoff, publication, result);

  publication.ready = publication.transfer_record_deterministic;
  return publication;
}
