#pragma once

#include <string_view>

namespace aim {

class LocalStore;

enum class Language {
  kEnglish = 0,
  kChinese = 1,
};

void SetLanguage(Language language);
Language GetLanguage();

// English text is the lookup key; returns Chinese when language is zh, else English.
const char* Tr(const char* english);

constexpr const char* kLanguageStoreKey = "Language";

void LoadLanguageFromStore(LocalStore& store);
void SaveLanguageToStore(LocalStore& store, Language language);

}  // namespace aim
