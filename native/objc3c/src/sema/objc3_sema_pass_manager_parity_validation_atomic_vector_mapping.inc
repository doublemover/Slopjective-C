  result.atomic_vector_mapping_publication_record =
      BuildObjc3SemaAtomicVectorMappingPublicationRecord(
          input,
          result.atomic_memory_order_mapping,
          result.deterministic_atomic_memory_order_mapping,
          result.vector_type_lowering,
          result.deterministic_vector_type_lowering);
  result.deterministic_atomic_vector_mapping_publication_record =
      IsReadyObjc3SemaAtomicVectorMappingPublicationRecord(
          result.atomic_vector_mapping_publication_record);
  result.parity_surface.atomic_vector_mapping_publication_record =
      result.atomic_vector_mapping_publication_record;
  result.parity_surface.deterministic_atomic_vector_mapping_publication_record =
      result.deterministic_atomic_vector_mapping_publication_record;
  result.parity_surface.atomic_memory_order_mapping =
      result.parity_surface.atomic_vector_mapping_publication_record
          .atomic_memory_order_mapping;
  result.parity_surface.deterministic_atomic_memory_order_mapping =
      result.parity_surface.atomic_vector_mapping_publication_record
          .deterministic_atomic_memory_order_mapping;
  result.parity_surface.vector_type_lowering =
      result.parity_surface.atomic_vector_mapping_publication_record
          .vector_type_lowering;
  result.parity_surface.deterministic_vector_type_lowering =
      result.parity_surface.atomic_vector_mapping_publication_record
          .deterministic_vector_type_lowering;
