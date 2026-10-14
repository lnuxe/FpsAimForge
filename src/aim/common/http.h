#pragma once

#include <format>
#include <optional>
#include <string>
#include <vector>

namespace aim {

struct FileDownload {
  std::string content;
  std::string etag;
  bool content_unchanged = false;
};

struct HttpHeader {
  std::string key;
  std::string value;

  std::string GetHeader() const {
    return std::format("{}: {}", key, value);
  }
};

inline const HttpHeader kGithubApiVersionHeader = {"X-GitHub-Api-Version", "2026-03-10"};

bool DownloadFile(const std::string& url,
                  const std::vector<HttpHeader>& header,
                  FileDownload* download);

struct FileDownloadResult {
  bool success = false;
  FileDownload download;
};

void GlobalInitializeHttp();
void GlobalCleanupHttp();

std::optional<std::string> ParseEtagFromHeader(const std::string& header);
std::string MakeEtagHeader(const std::string& etag);

}  // namespace aim
