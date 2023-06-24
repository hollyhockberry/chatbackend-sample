// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef SAMPLE_AVATAR_HPP_
#define SAMPLE_AVATAR_HPP_

#include <Avatar.h>
#include <modules/Avatar.hpp>
#include "AudioOutputM5Speaker.h"

namespace sample {

class Avatar : public chat_backend::modules::Avatar {
  static Avatar* _instance;
  m5avatar::Avatar _avatar;
  AudioOutputM5Speaker* _audio = nullptr;
  unsigned long _balloonTime = 0;
  String _balloonText = "";
  String _systemMessage = "";

 public:
  Avatar();
  void assignAudio(AudioOutputM5Speaker* audio);
  void begin() override;

  void expression(const String& content) override;
  void balloon(const String& content) override;
  void systemMessage(const String& content) override;

 private:
  static void main(void* arg);
  void loop();

  void lipSync(void *args);
};

}  // sample

#endif  // SAMPLE_AVATAR_HPP_
