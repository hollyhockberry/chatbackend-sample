// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_MODULES_AVATAR_HPP_
#define CHATBACKEND_MODULES_AVATAR_HPP_

#include <WString.h>

namespace chat_backend::modules {

class Avatar {
 public:
  virtual void begin() {}
  virtual void expression(const String& content) {}
  virtual void balloon(const String& content) {}
  virtual void systemMessage(const String& content) {}
  virtual void coolDown() {
    expression("Neutral");
  }
};

}  // chat_backend::modules

#endif  // CHATBACKEND_MODULES_AVATAR_HPP_
