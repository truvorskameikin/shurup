#ifndef __SHURUP_ANDROID_LOG_H_
#define __SHURUP_ANDROID_LOG_H_

#include <string>
#include <streambuf>
#include <jni.h>
#include <android/log.h>

namespace shurup {
  class AndroidLogBuf : public std::streambuf {
   public:
    AndroidLogBuf(const std::string& tag) : tag_(tag) {
    }

    ~AndroidLogBuf() {
      if (!buffer_.empty())
        __android_log_print(ANDROID_LOG_INFO, tag_.c_str(), buffer_.c_str());
    }

   private:
    int overflow(int c) {
      if (std::char_traits<char>::eq_int_type(c, std::char_traits<char>::eof())) {
        return std::char_traits<char>::not_eof(c);
      } else {
        if (std::char_traits<char>::eq_int_type(c, '\n')) {
          if (!buffer_.empty()) {
            __android_log_print(ANDROID_LOG_INFO, tag_.c_str(), buffer_.c_str());
            buffer_.clear();
          }
        } else if (!std::char_traits<char>::eq_int_type(c, '\r')) {
          buffer_ += (char) c;
        }
      }
    }

   private:
    std::string tag_;
    std::string buffer_;
  };
}

#endif