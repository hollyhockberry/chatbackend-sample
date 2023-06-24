// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_IMPLE_VOICEVOXTTS_HPP_
#define CHATBACKEND_IMPLE_VOICEVOXTTS_HPP_

#include <VoiceVoxClient.h>
#include "modules/TTS.hpp"

namespace chat_backend::impl {

class VoiceVoxTTS : public modules::TTS {
  VoiceVoxClient* _client;
 public:
  VoiceVoxTTS(AudioOutput& audioOutput, const char* rootCACertificate);
  bool busy() const override;
  void apiKey(const char* key) override;
  void begin() override;
  bool say(const char* content) override;
};

}  // chat_backend::impl

#endif  // CHATBACKEND_IMPLE_VOICEVOXTTS_HPP_
