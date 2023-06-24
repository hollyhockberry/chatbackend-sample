// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_HPP_
#define CHATBACKEND_HPP_

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "Actions.hpp"
#include "tasks/Chat.hpp"
#include "modules/WakeTrigger.hpp"
#include "modules/STT.hpp"
#include "modules/TTS.hpp"
#include "modules/Avatar.hpp"

namespace chat_backend {

class ChatBackend {
  Actions* _actions;
  tasks::Chat* _chat;
  modules::STT* _stt;
  modules::TTS* _tts;
  modules::WakeTrigger* _wake;
  modules::Avatar* _avatar;
  Actions::Resources _resources;
  unsigned long _lastAccess;

 public:
  ChatBackend(Actions* actions,
              tasks::Chat* chat,
              modules::STT* stt, modules::TTS* tts,
              modules::WakeTrigger* wake,
              modules::Avatar* avatar);
  void begin();
  void loop();

  tasks::Chat& Chat() { return *_chat; }
  modules::STT& STT() { return *_stt; }
  modules::TTS& TTS() { return *_tts; }

 private:
  bool update();
  bool busy() const;

  void handleMessage(const String& type, const String& content);
  void sendMessage(const char* type, const char* message);
  void sendMessage(const char* type);
  void takeMutex();
  void giveMutex();
};

}  // chat_backend

#endif  // CHATBACKEND_HPP_
