// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include "VoiceVoxTTS.hpp"
#include "modules/TTS.hpp"

namespace chat_backend::impl {

VoiceVoxTTS::VoiceVoxTTS(AudioOutput& audioOutput, const char* rootCACertificate) {
  _client = new VoiceVoxClient(audioOutput, rootCACertificate);
}

bool VoiceVoxTTS::busy() const {
  return _client->busy();
}

void VoiceVoxTTS::apiKey(const char* key) {
  _client->apiKey(key);
}

void VoiceVoxTTS::begin() {
  _client->begin();
}

bool VoiceVoxTTS::say(const char* content) {
  M5_LOGV("begin");
  _client->queue(content);
  M5_LOGV("end");  
  return true;
}

}  // chat_backend::impl
