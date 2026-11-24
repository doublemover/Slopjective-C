@import Parcel.Quotes;

int main() {
  auto result = @Parcel.Quotes.quote_shipment(500, false);
  match (result) {
    case .Ok(let quote): { return quote.cents == 600 && quote.business_days == 5 ? 0 : 1; }
    case .Err(let error): { return 1; }
  }
}
