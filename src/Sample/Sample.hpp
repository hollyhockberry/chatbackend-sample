// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef SAMPLE_SAMPLE_HPP_
#define SAMPLE_SAMPLE_HPP_

#include <ChatBackend.hpp>
#include <tasks/Chat.hpp>
#include <implements/WhisperSTT.hpp>
#include <implements/VoiceVoxTTS.hpp>
#include "Actions.hpp"
#include "Avatar.hpp"
#include "WakeTrigger.hpp"
#include "AudioOutputM5Speaker.h"

namespace sample {

class Sample : public chat_backend::ChatBackend {
  AudioOutputM5Speaker _audio;
  Actions _actions;
  chat_backend::tasks::Chat _chat;
  chat_backend::impl::WhisperSTT _stt;
  chat_backend::impl::VoiceVoxTTS _tts;
  WakeTrigger _wake;
  Avatar _avatar;

 public:
  Sample();
  void begin(const char* keyOpenAI, const char* keyVoiceVox);
};

}  // sample

#endif  // SAMPLE_SAMPLE_HPP_
