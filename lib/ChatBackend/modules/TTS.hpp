// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_MODULES_TTS_HPP_
#define CHATBACKEND_MODULES_TTS_HPP_

namespace chat_backend::modules {

class TTS {
 public:
  virtual bool busy() const { return false; }
  virtual void apiKey(const char* key) {}
  virtual void begin() {}
  virtual bool say(const char* content) { return true; }
};

}  // chat_backend::modules

#endif  // CHATBACKEND_MODULES_TTS_HPP_
