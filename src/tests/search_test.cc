#include "aim/common/search.h"

#include "gtest/gtest.h"

using namespace aim;

TEST(SearchTest, StringMatchesSearch) {
  std::string name = "AF Test ScenarioFoo";
  EXPECT_TRUE(StringMatchesSearch(name, GetSearchWords("A")));
  EXPECT_TRUE(StringMatchesSearch(name, GetSearchWords("tes A fo")));
  EXPECT_TRUE(StringMatchesSearch(name, GetSearchWords(" te ")));
  EXPECT_FALSE(StringMatchesSearch(name, GetSearchWords("A bjk")));
  EXPECT_FALSE(StringMatchesSearch(name, GetSearchWords("afa")));

  EXPECT_TRUE(StringMatchesSearch(name, GetSearchWords("A foo")));
  EXPECT_TRUE(StringMatchesSearch(name, GetSearchWords("A foo -")));
  EXPECT_TRUE(StringMatchesSearch(name, GetSearchWords("A -nomatch")));
  EXPECT_FALSE(StringMatchesSearch(name, GetSearchWords("A -foo")));
}
