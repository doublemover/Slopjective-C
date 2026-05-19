namespace {

void AppendRuntimeIngestBinaryEnvelopeU32(std::string &payload,
                                          std::uint32_t value) {
  for (std::uint32_t shift = 0; shift < 32u; shift += 8u) {
    payload.push_back(
        static_cast<char>((value >> shift) & static_cast<std::uint32_t>(0xFFu)));
  }
}

void AppendRuntimeIngestBinaryEnvelopeChunk(std::string &payload,
                                            const std::string &chunk_name,
                                            const std::string &chunk_payload) {
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_name.size()));
  payload.append(chunk_name);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_payload.size()));
  payload.append(chunk_payload);
}

}  // namespace

std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelopePayload(
    const std::string &packaging_json,
    const std::string &typed_handoff_json,
    const std::string &debug_projection_json) {
  std::string payload;
  payload.reserve(
      std::char_traits<char>::length(
          kObjc3ExecutableMetadataRuntimeIngestBinaryMagic) +
      sizeof(std::uint32_t) * 8u + packaging_json.size() +
      typed_handoff_json.size() + debug_projection_json.size() + 256u);
  payload.append(kObjc3ExecutableMetadataRuntimeIngestBinaryMagic);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload,
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      packaging_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      typed_handoff_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload,
      kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName,
      debug_projection_json);
  return payload;
}
