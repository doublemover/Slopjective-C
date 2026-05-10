#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

std::string Objc3ExecutableMethodBodyBindingSummary() {
  std::ostringstream out;
  // executable method-body binding implementation anchor: lane-C
  // upgrades the frozen C001 surface into a fail-closed runtime capability by
  // requiring every implementation-owned executable method entry to bind to
  // exactly one concrete LLVM definition symbol before the object artifact is
  // accepted.
  out << "contract=" << kObjc3ExecutableMethodBodyBindingContractId
      << ";source_model=" << kObjc3ExecutableMethodBodyBindingSourceModel
      << ";runtime_model=" << kObjc3ExecutableMethodBodyBindingRuntimeModel
      << ";fail_closed_model="
      << kObjc3ExecutableMethodBodyBindingFailClosedModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel;
  return out.str();
}

std::string Objc3ExecutableRealizationRecordsSummary() {
  std::ostringstream out;
  // executable realization-record expansion anchor: emitted
  // class/protocol/category records now preserve the owner and graph edges that
  // D-lane runtime realization will consume directly. Parser/sema still own the
  // legality and canonical identities; lowering only serializes that closure
  // into stable artifact layouts.
  out << "contract=" << kObjc3ExecutableRealizationRecordsContractId
      << ";class_record_model="
      << kObjc3ExecutableRealizationClassRecordModel
      << ";protocol_record_model="
      << kObjc3ExecutableRealizationProtocolRecordModel
      << ";category_record_model="
      << kObjc3ExecutableRealizationCategoryRecordModel
      << ";fail_closed_model="
      << kObjc3ExecutableRealizationFailClosedModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel;
  return out.str();
}

std::string Objc3RuntimeClassRealizationSummary() {
  std::ostringstream out;
  // class-realization-runtime freeze anchor: the current runtime
  // consumes emitted realization records directly, walks the class/metaclass
  // graph deterministically, attaches preferred category implementation
  // records after bundle selection, and uses protocol records only as
  // declaration-aware negative lookup evidence. Property/ivar storage and
  // executable protocol bodies remain outside this runtime boundary.
  out << "contract=" << kObjc3RuntimeClassRealizationContractId
      << ";class_realization_model=" << kObjc3RuntimeClassRealizationModel
      << ";metaclass_graph_model=" << kObjc3RuntimeMetaclassGraphModel
      << ";category_attachment_model="
      << kObjc3RuntimeClassRealizationCategoryAttachmentModel
      << ";protocol_check_model=" << kObjc3RuntimeProtocolCheckModel
      << ";fail_closed_model="
      << kObjc3RuntimeClassRealizationFailClosedModel
      << ";non_goals=no-property-storage-no-ivar-layout-no-protocol-body-dispatch";
  return out.str();
}

std::string Objc3RuntimeMetaclassGraphRootClassSummary() {
  std::ostringstream out;
  // metaclass-graph-root-class anchor: runtime now publishes a
  // realized class graph keyed by stable receiver base identities, preserves
  // root classes as explicit nodes with null superclass links, and keeps
  // known-class/class-self dispatch on the same metaclass graph without
  // widening the public runtime ABI. Allocation, instance storage, and
  // protocol-body execution remain outside this boundary.
  out << "contract=" << kObjc3RuntimeMetaclassGraphRootClassContractId
      << ";realized_class_graph_model="
      << kObjc3RuntimeRealizedClassGraphModel
      << ";root_class_baseline_model="
      << kObjc3RuntimeRootClassBaselineModel
      << ";fail_closed_model="
      << kObjc3RuntimeRealizedClassGraphFailClosedModel
      << ";non_goals=no-allocation-no-instance-storage-no-protocol-body-dispatch";
  return out.str();
}

std::string Objc3RuntimeCategoryAttachmentProtocolConformanceSummary() {
  std::ostringstream out;
  // category-attachment-protocol-conformance anchor: runtime-owned
  // realized class nodes now retain preferred category attachments and answer
  // protocol conformance queries from emitted class/category protocol refs
  // without rediscovering source legality or widening the public ABI.
  out << "contract="
      << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
      << ";category_attachment_model="
      << kObjc3RuntimeCategoryAttachmentRealizedGraphModel
      << ";protocol_conformance_query_model="
      << kObjc3RuntimeProtocolConformanceQueryModel
      << ";fail_closed_model="
      << kObjc3RuntimeAttachmentConformanceFailClosedModel
      << ";non_goals=no-allocation-no-property-storage-no-cross-image-attachment";
  return out.str();
}

std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary() {
  std::ostringstream out;
  // canonical-runnable-object-sample anchor: runtime-owned builtin
  // alloc/new/init resolution now closes the smallest truthful executable
  // object sample while metadata-rich object-model behavior stays proven
  // through paired library/probe evidence instead of pretending the runtime
  // export gate is already open for every executable sample shape.
  out << "contract="
      << kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId
      << ";execution_model="
      << kObjc3RuntimeCanonicalRunnableObjectExecutionModel
      << ";probe_split_model="
      << kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel
      << ";fail_closed_model="
      << kObjc3RuntimeCanonicalRunnableObjectFailClosedModel
      << ";non_goals=no-property-storage-no-ivar-layout-no-metadata-heavy-executable-export-bypass";
  return out.str();
}

std::string Objc3RuntimeMetadataBinaryInspectionHarnessSummary() {
  std::ostringstream out;
  // binary inspection harness expansion anchor: lane-C now proves
  // emitted metadata sections structurally through one shared llvm-readobj and
  // llvm-objdump corpus. The positive corpus covers scaffold-only, class-heavy,
  // category-heavy, and selector-pool-heavy objects, while the negative corpus
  // remains fail-closed when semantic validation prevents object emission.
  out << "contract=" << kObjc3RuntimeBinaryInspectionHarnessContractId
      << ";positive_corpus_model="
      << kObjc3RuntimeBinaryInspectionPositiveCorpusModel
      << ";negative_corpus_model="
      << kObjc3RuntimeBinaryInspectionNegativeCorpusModel
      << ";section_inventory_command="
      << kObjc3RuntimeBinaryInspectionSectionCommand
      << ";symbol_inventory_command="
      << kObjc3RuntimeBinaryInspectionSymbolCommand
      << ";non_goals=no-new-metadata-families-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataEmissionGateSummary() {
  std::ostringstream out;
  // metadata-emission gate anchor: lane-E freezes one fail-closed
  // evidence chain over the implemented A002/B003/C006/D003 summaries before
  // cross-lane closeout begins, so later work must preserve the same source
  // matrix, object-format policy, binary inspection corpus, and archive/static
  // link discovery proofs instead of redefining the gate ad hoc.
  out << "contract=" << kObjc3RuntimeMetadataEmissionGateContractId
      << ";evidence_model=" << kObjc3RuntimeMetadataEmissionGateEvidenceModel
      << ";failure_model=" << kObjc3RuntimeMetadataEmissionGateFailureModel
      << ";non_goals=no-new-emission-families-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataObjectEmissionCloseoutSummary() {
  std::ostringstream out;
  // cross-lane object-emission closeout anchor: lane-E extends the
  // E001 summary chain with fresh integrated native object probes so class,
  // category, and message-send outputs all prove the same source graph,
  // object-format policy, binary inspection, and linker/discovery continuity
  // before later startup registration work begins.
  out << "contract=" << kObjc3RuntimeMetadataObjectEmissionCloseoutContractId
      << ";evidence_model="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel
      << ";failure_model="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel
      << ";non_goals=no-startup-registration-or-runtime-bootstrap";
  return out.str();
}

std::string Objc3ManifestObjectIrTruthGateSummary() {
  std::ostringstream out;
  // manifest/object/IR truth gate anchor: issue #8018 freezes one regenerated
  // artifact set above the existing metadata-emission and object-emission gates
  // so release claims cannot be widened from only one sidecar or object probe.
  out << "contract=" << kObjc3ManifestObjectIrTruthGateContractId
      << ";evidence_model=" << kObjc3ManifestObjectIrTruthGateEvidenceModel
      << ";manifest_model=" << kObjc3ManifestObjectIrTruthGateManifestModel
      << ";ir_model=" << kObjc3ManifestObjectIrTruthGateIrModel
      << ";object_model=" << kObjc3ManifestObjectIrTruthGateObjectModel
      << ";claim_model=" << kObjc3ManifestObjectIrTruthGateClaimModel
      << ";metadata_gate_contract="
      << kObjc3RuntimeMetadataEmissionGateContractId
      << ";object_closeout_contract="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutContractId
      << ";versioned_conformance_contract="
      << kObjc3VersionedConformanceReportLoweringContractId
      << ";runtime_capability_contract="
      << kObjc3RuntimeCapabilityReportingContractId
      << ";required_artifacts=module.manifest.json,module.ll,module.obj,module.runtime-registration-descriptor.json,module.runtime-registration-manifest.json,module.runtime-metadata.bin,module.runtime-metadata-discovery.json,module.runtime-metadata-linker-options.rsp,module.objc3-conformance-report.json,module.objc3-conformance-publication.json,module.objc3-advanced-feature-gate.json,module.objc3-release-candidate-matrix.json"
      << ";failure_model=" << kObjc3ManifestObjectIrTruthGateFailureModel
      << ";follow_on_surface=objc3c.manifest.object.ir.truthgate.closeout.v1";
  return out.str();
}
