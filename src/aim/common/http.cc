#include "http.h"

#include <format>

#include "absl/strings/ascii.h"

namespace aim {

namespace {

constexpr const std::string_view kEtagPrefix = "etag:";

std::string EscapeEtag(std::string etag) {
  etag = absl::StripAsciiWhitespace(etag);
  if (etag.contains('\"')) {
    return etag;
  }
  return std::format("\"{}\"", etag);
}

}  // namespace

std::optional<std::string> ParseEtagFromHeader(const std::string& header) {
  std::string lower_header = absl::AsciiStrToLower(header);
  if (!lower_header.starts_with(kEtagPrefix)) {
    return {};
  }
  std::string etag = header.substr(kEtagPrefix.length(), header.length() - kEtagPrefix.length());
  std::string result(absl::StripAsciiWhitespace(etag));
  if (result.empty()) {
    return {};
  }
  return result;
}

std::string MakeEtagHeader(const std::string& etag) {
  return std::format("If-None-Match: {}", EscapeEtag(etag));
}

}  // namespace aim
