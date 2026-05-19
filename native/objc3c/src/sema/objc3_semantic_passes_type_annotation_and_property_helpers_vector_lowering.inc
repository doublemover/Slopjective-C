static void RecordVectorTypeLoweringAnnotation(ValueType base_type, unsigned lane_count, bool is_return,
                                               Objc3VectorTypeLoweringSummary &summary) {
  if (is_return) {
    ++summary.return_annotations;
  } else {
    ++summary.param_annotations;
  }

  if (base_type == ValueType::Bool) {
    ++summary.bool_annotations;
  } else if (base_type == ValueType::I32) {
    ++summary.i32_annotations;
  } else {
    ++summary.unsupported_annotations;
    summary.deterministic = false;
  }

  switch (lane_count) {
    case 2u:
      ++summary.lane2_annotations;
      break;
    case 4u:
      ++summary.lane4_annotations;
      break;
    case 8u:
      ++summary.lane8_annotations;
      break;
    case 16u:
      ++summary.lane16_annotations;
      break;
    default:
      ++summary.unsupported_annotations;
      summary.deterministic = false;
      break;
  }
}
