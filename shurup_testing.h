#ifndef __SHURUP_TESTING_H_
#define __SHURUP_TESTING_H_

#include <string>
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

#endif