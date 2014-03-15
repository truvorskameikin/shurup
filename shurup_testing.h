#ifndef __SHURUP_TESTING_H_
#define __SHURUP_TESTING_H_

#include <string>
#include <set>
#include <sstream>
#include <iostream>
#include "shurup_assert.h"

namespace shurup {
  inline
  void TestHeader(const std::string& group_name) {
    std::cout << group_name << ":" << std::endl;
  }

  inline
  void TestFooter(const std::string& group_name) {
  }

  inline
  void CaseHeader(
    const std::string& group_name,
    const std::string& case_name
  ) {
    std::cout << "  Testing " << case_name << ": ";
  }

  inline
  void CaseFooter(
    const std::string& group_name,
    const std::string& case_name
  ) {
    std::cout << "Ok" << std::endl;
  }

  inline
  void CaseSkip(
    const std::string& group_name,
    const std::string& case_name
  ) {
    std::cout << "   Testing " << case_name << ": SKIPPED" << std::endl;
  }

  class Settings {
   public:
    Settings() : are_all_groups_skipped_(false) {
    }

    bool are_all_groups_skipped() const {
      return are_all_groups_skipped_;
    }

    void set_are_all_groups_skipped(bool are_all_groups_skipped) {
      are_all_groups_skipped_ = are_all_groups_skipped;
    }

    const std::set<std::string>& skipped_groups() const {
      return skipped_groups_;
    }

    bool is_skipped_group(const std::string& group_name) {
      if (are_all_groups_skipped_)
        return true;
      return skipped_groups_.find(group_name) != skipped_groups_.end();
    }

    void ParseCommandLineArguments(int argc, char* argv[]) {
      skipped_groups_.clear();

      for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-shurup_skip") == 0) {
          if (i + 1 < argc)
            skipped_groups_.insert(argv[i + 1]);
        }
      }
    }

   private:
    bool are_all_groups_skipped_;
    std::set<std::string> skipped_groups_;
  };
} // namespace shurup

#define TEST_MAKE(test_name, case_name)\
void test_name##_##case_name##_Test()

#define TEST_DECLARE(test_name, case_name)\
extern void test_name##_##case_name##_Test()

#define TEST_START_GROUP(test_name)\
shurup::TestHeader(#test_name)

#define TEST_END_GROUP(test_name)\
shurup::TestFooter(#test_name)

#define TEST_CALL(test_name, case_name)\
shurup::CaseHeader(#test_name, #case_name);\
test_name##_##case_name##_Test();\
shurup::CaseFooter(#test_name, #case_name)

#define TEST_SKIP(test_name, case_name)\
shurup::CaseSkip(#test_name, #case_name);

#define TEST(test_name, case_name)\
if (!settings.is_skipped_group(#test_name)) {\
shurup::CaseHeader(#test_name, #case_name);\
test_name##_##case_name##_Test();\
shurup::CaseFooter(#test_name, #case_name);\
}\
else {\
shurup::CaseSkip(#test_name, #case_name);\
}

#endif