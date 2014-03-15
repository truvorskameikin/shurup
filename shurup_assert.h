#ifndef __SHURUP_ASSERT_H_
#define __SHURUP_ASSERT_H_

#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>

namespace shurup {

namespace {

template<typename Type>
std::string TypeToString(const Type& value) {
  std::ostringstream stream;
  stream << value;

  return stream.str();
}

} // namespace

enum AssertType {
  ASSERT_TYPE_EQ,
  ASSERT_TYPE_NE,
  ASSERT_TYPE_GE,
  ASSERT_TYPE_LE
};

template<
  typename ExpectedValueType,
  typename ActualValueType
>
std::string FormatAssertMessage(
  AssertType assert_type,
  const std::string& expected_expression,
  const ExpectedValueType& expected_value,
  const std::string& actual_expression,
  const ActualValueType& actual_value,
  const char* file_name,
  int line
) {
  std::string operatior_string = "==";
  if (assert_type == ASSERT_TYPE_NE)
    operatior_string = "!=";
  if (assert_type == ASSERT_TYPE_GE )
	operatior_string = ">=";
  if (assert_type == ASSERT_TYPE_LE )
	operatior_string = "<=";

  std::string expected_value_string = TypeToString(expected_value);
  std::string actual_value_string = TypeToString(actual_value);

  std::string line_string = TypeToString(line);

  return
    std::string("Assertion failed! Expression: ") +
    expected_expression +
    " " +
    operatior_string +
    " " +
    actual_expression +
    ", expected value: " +
    expected_value_string +
    ", actual value: " +
    actual_value_string +
    ". At " +
    file_name +
    " (" +
    line_string +
    ")";
}

template<
  typename ExpectedValueType,
  typename ActualValueType
>
bool AssertEq(
  const std::string& expected_expression,
  const ExpectedValueType& expected_value,
  const std::string& actual_expression,
  const ActualValueType& actual_value,
  const char* file_name,
  int line
) {
  if (expected_value == actual_value)
    return true;

  std::string result = FormatAssertMessage(
    ASSERT_TYPE_EQ,
    expected_expression,
    expected_value,
    actual_expression,
    actual_value,
    file_name,
    line
  );

  std::cout << result << std::endl;

  abort();

  return false;
}

template<
  typename ExpectedValueType,
  typename ActualValueType
>
bool AssertNe(
  const std::string& expected_expression,
  const ExpectedValueType& expected_value,
  const std::string& actual_expression,
  const ActualValueType& actual_value,
  const char* file_name,
  int line
) {
  if (expected_value != actual_value)
    return true;

  std::string result = FormatAssertMessage(
    ASSERT_TYPE_NE,
    expected_expression,
    expected_value,
    actual_expression,
    actual_value,
    file_name,
    line
  );

  std::cout << result << std::endl;

  abort();

  return false;
}

// actual_value is GREATER OR EQUAL than expected_value
template<
  typename ExpectedValueType,
  typename ActualValueType
>
bool AssertGe(
  const std::string& expected_expression,
  const ExpectedValueType& expected_value,
  const std::string& actual_expression,
  const ActualValueType& actual_value,
  const char* file_name,
  int line
) {
  if (expected_value <= actual_value)
    return true;

  std::string result = FormatAssertMessage(
    ASSERT_TYPE_GE,
    expected_expression,
    expected_value,
    actual_expression,
    actual_value,
    file_name,
    line
  );

  std::cout << result << std::endl;

  abort();

  return false;
}

// actual_value is LESS OR EQUAL than expected_value
template<
  typename ExpectedValueType,
  typename ActualValueType
>
bool AssertLe(
  const std::string& expected_expression,
  const ExpectedValueType& expected_value,
  const std::string& actual_expression,
  const ActualValueType& actual_value,
  const char* file_name,
  int line
) {
  if (expected_value >= actual_value)
    return true;

  std::string result = FormatAssertMessage(
    ASSERT_TYPE_GE,
    expected_expression,
    expected_value,
    actual_expression,
    actual_value,
    file_name,
    line
  );

  std::cout << result << std::endl;

  abort();

  return false;
}

} // namespace shurup

#define TEST_ASSERT_EQ(expected_value, actual_expression)\
shurup::AssertEq(\
  #expected_value,\
  (expected_value),\
  #actual_expression,\
  (actual_expression),\
  __FILE__,\
  __LINE__\
)

#define TEST_ASSERT_NE(expected_value, actual_expression)\
shurup::AssertNe(\
  #expected_value,\
  (expected_value),\
  #actual_expression,\
  (actual_expression),\
  __FILE__,\
  __LINE__\
)

// actual_expression >= expected_value
#define TEST_ASSERT_GE(expected_value, actual_expression)\
shurup::AssertGe(\
  #expected_value,\
  (expected_value),\
  #actual_expression,\
  (actual_expression),\
  __FILE__,\
  __LINE__\
)

// actual_expression <= expected_value
#define TEST_ASSERT_LE(expected_value, actual_expression)\
shurup::AssertLe(\
  #expected_value,\
  (expected_value),\
  #actual_expression,\
  (actual_expression),\
  __FILE__,\
  __LINE__\
)

#endif