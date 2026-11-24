struct Quote {
  unsigned cents;
  unsigned business_days;
};

enum class QuoteError { invalid_weight, above_limit };

using QuoteResult = Result(Quote, QuoteError);

QuoteResult quote_shipment(int grams, bool express) {
  if (grams <= 0)
    return Err(Quote, QuoteError, QuoteError::invalid_weight);
  if (grams > 20000)
    return Err(Quote, QuoteError, QuoteError::above_limit);

  const unsigned weight_bands = (static_cast<unsigned>(grams) + 499U) / 500U;
  const unsigned base_cents = express ? 850U : 500U;
  return Ok(Quote, QuoteError, (Quote{base_cents + 100U * weight_bands,
                                     express ? 2U : 5U}));
}
