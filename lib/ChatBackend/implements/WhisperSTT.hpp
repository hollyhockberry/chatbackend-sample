// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_IMPLE_WHISPERSTT_HPP_
#define CHATBACKEND_IMPLE_WHISPERSTT_HPP_

#include <Arduino.h>
#include "modules/STT.hpp"

namespace chat_backend::impl {

class WhisperSTT : public modules::STT {
  const char* _rootCACertificate;
  String _apiKey = "";
  byte* _buffer = nullptr;
  size_t _size = 0;

 public:
  explicit WhisperSTT(const char* rootCACertificate);
  void apiKey(const char* key) override;
  void begin() override;

  bool record() override;
  bool transcribe(String& out) override;
};

}  // chat_backend::impl

#endif  // CHATBACKEND_IMPLE_WHISPERSTT_HPP_
