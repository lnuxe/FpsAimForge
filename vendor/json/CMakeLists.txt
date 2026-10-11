add_library(json nlohmann/json.cc nlohmann/json.h)

target_include_directories(json PUBLIC .)
