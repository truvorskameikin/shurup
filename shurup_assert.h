#ifndef __SHURUP_ASSERT_H_
#define __SHURUP_ASSERT_H_

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
  ASSERT_TYPE_NE
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
  const std::string& message,
  const char* file_name,
  int line
) {
  std::string operatior_string = "==";
  if (assert_type == ASSERT_TYPE_NE)
    operatior_string = "!=";

  std::string expected_value_string = TypeToString(expected_value);
  std::string actual_value_string = TypeToString(actual_value);

  std::string line_string = TypeToString(line);

  return
    std::string("Assertion failed: ") +
    message +
    ". Expression: " +
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
  const std::string& message,
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
    message,
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
  const std::string& message,
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
    message,
    file_name,
    line
  );

  std::cout << result << std::endl;

  abort();

  return false;
}

} // namespace shurup

#define TEST_ASSERT_EQ(expected_value, actual_expression, message)\
shurup::AssertEq(\
  #expected_value,\
  (expected_value),\
  #actual_expression,\
  (actual_expression),\
  message,\
  __FILE__,\
  __LINE__\
)

#define TEST_ASSERT_NE(expected_value, actual_expression, message)\
shurup::AssertNe(\
  #expected_value,\
  (expected_value),\
  #actual_expression,\
  (actual_expression),\
  message,\
  __FILE__,\
  __LINE__\
)

#endif