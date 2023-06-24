// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_MODULES_STT_HPP_
#define CHATBACKEND_MODULES_STT_HPP_

#include <WString.h>

namespace chat_backend::modules {

class STT {
 public:
  virtual bool busy() const { return false; }
  virtual void apiKey(const char* key) {}
  virtual void begin() {}
  virtual bool record() { return true; }
  virtual bool transcribe(String& out) { return true; }
};

}  // chat_backend::modules

#endif  // CHATBACKEND_MODULES_STT_HPP_
