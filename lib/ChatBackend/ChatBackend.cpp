// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include <WiFi.h>
#include <gob_unifiedButton.hpp>
#include "ChatBackend.hpp"
#include "tasks/MessagePacket.hpp"

namespace chat_backend {

gob::UnifiedButton unfiedButton;

ChatBackend::ChatBackend(Actions* actions,
                         tasks::Chat* chat,
                         modules::STT* stt,
                         modules::TTS* tts,
                         modules::WakeTrigger* wake,
                         modules::Avatar* avatar)
: _resources() {
  _actions = actions ? actions : new Actions();
  _chat = chat ? chat : new chat_backend::tasks::Chat(nullptr);
  _stt = stt ? stt : new modules::STT();
  _tts = tts ? tts : new modules::TTS();
  _wake = wake ? wake : new modules::WakeTrigger();
  _avatar = avatar ? avatar : new modules::Avatar();
}

void ChatBackend::begin() {
  M5.begin();
  unfiedButton.begin(&M5.Display);
  {
    auto cfg = M5.Speaker.config();
    cfg.sample_rate = 96000;
    cfg.task_pinned_core = APP_CPU_NUM;
    M5.Speaker.config(cfg);
    M5.Speaker.end();
  }
  {
    auto cfg = M5.Mic.config();
    cfg.stereo = false;
    M5.Mic.config(cfg);
    M5.Mic.begin();
  }

  //TODO(hollyhockberry): 
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    ::delay(1000);
    M5_LOGI("Connecting to WiFi...");
  }
  M5_LOGI("Connected to WiFi");

  _lastAccess = 0;
  _resources.mutex = ::xSemaphoreCreateMutex();
  _resources.queue = ::xQueueCreate(20, sizeof(tasks::MessagePacket*));

  Chat().begin(_resources.queue, _resources.mutex);
  _wake->begin();
  STT().begin();
  TTS().begin();
  _avatar->begin();
  _actions->begin();
}

void ChatBackend::loop() {
  tasks::MessagePacket* ptr;
  if (::xQueueReceive(_resources.queue, &ptr, 0) != 0) {
    handleMessage(ptr->type, ptr->content);
    delete ptr;
    return;
  }
  if (update()) {
    return;
  }
  unfiedButton.update();
  const auto touch = M5.Touch.getDetail();
  M5.update();
  m5::Button_Class* btns[] = { &M5.BtnA, &M5.BtnB, &M5.BtnC };
  auto btn = 0;
  for (auto b : btns) {
    if (b->wasSingleClicked()) {
      _actions->onButtonClicked(btn, _resources);
      return;
    }
    else if (b->wasDoubleClicked()) {
      _actions->onButtonDoubleClicked(btn, _resources);
      return;
    }
    ++btn;
  }
  if (touch.wasPressed()) {
    if (!_actions->onTouched(touch.x, touch.y, _resources)) {
    }
  }
  else if (Serial.available()) {
    _actions->onAvailableStream(Serial, _resources);
  }
  else if (!_actions->loop(_resources)) {
    takeMutex();
    if (_wake->listen()) {
      sendMessage("wakeup");
    }
    giveMutex();
  }
}

void ChatBackend::handleMessage(const String& type, const String& content) {
  M5_LOGV("%s, %s", type.c_str(), content.c_str());
  if (type.equals("say")) {
    takeMutex();
    M5.Speaker.begin();
    if (!TTS().say(content.c_str())) {
M5_LOGE(); //TODO(hollyhockberry):
    }
    giveMutex();
  }
  else if (type.equals("record")) {
    sendMessage("beginRec");
  }
  else if (type.equals("_record")) {
      takeMutex();
      if (_wake->record()) {
        sendMessage("endRec");
      } else {
M5_LOGE(); //TODO(hollyhockberry):
      }
      M5.Mic.end();
      giveMutex();
  }
  else if (type.equals("beginRec")) {
    if (!_actions->onBeginRecording(_resources)) {
      sendMessage("_record");
    }
  }
  else if (type.equals("endRec")) {
    if (!_actions->onEndRecording(_resources)) {
    }
  }
  else if (type.equals("listen")) {
    sendMessage("beginListen");
  }
  else if (type.equals("_listen")) {
    takeMutex();
    if (STT().record()) {
      sendMessage("endListen");
    } else {
M5_LOGE(); //TODO(hollyhockberry):
    }
    M5.Mic.end();
    giveMutex();
  }
  else if (type.equals("_transcribe")) {
    String out;
    if (STT().transcribe(out)) {
      sendMessage("chat", out.c_str());
    } else {
M5_LOGE(); //TODO(hollyhockberry):
    }
  }
  else if (type.equals("wakeup")){
    if (!_actions->onWakeup(_resources)) {
      sendMessage("beginListen");
    }    
  }
  else if (type.equals("beginListen")) {
    if (!_actions->onBeginListening(_resources)) {
      sendMessage("_listen");
    }
  }
  else if (type.equals("endListen")) {
    if (!_actions->onEndListening(_resources)) {
      sendMessage("_transcribe");
    }
  }
  else if (type.equals("chat")) {
    sendMessage("beginThink", content.c_str());
  }
  else if (type.equals("_chat")) {
    if (!Chat().chat(content.c_str())) {
M5_LOGE(); //TODO(hollyhockberry):
    }
  }
  else if (type.equals("beginThink")) {
    if (!_actions->onBeginThinking(_resources)) {
      sendMessage("_chat", content.c_str());
    }
  }
  else if (type.equals("endThink")) {
    if (!_actions->onEndThinking(_resources)) {
    }
  }
  else if (type.equals("expression")) {
    _avatar->expression(content);
  }
  else if (type.equals("balloon")) {
    _avatar->balloon(content);
  }
  else if (type.equals("message")) {
    _avatar->systemMessage(content);
  }
  else {
    _actions->handleMessage(type, content);
  }
  // refresh access time
  _lastAccess = ::millis();
}

bool ChatBackend::update() {
  if (Chat().busy() || STT().busy() || TTS().busy()) {
    if (_lastAccess <= 0) {
      M5_LOGV("busy");
    }
    _lastAccess = ::millis();
    return true;
  }
  if (_lastAccess <= 0) {
    return false;
  }
  if (::millis() - _lastAccess < 1000) {
    return true;
  }
  _lastAccess = 0;
  M5_LOGV("idle");
  if (!_actions->onEnterIdle(_resources)) {
    _avatar->coolDown();
    takeMutex();
    M5.Mic.end();
    M5.Speaker.end();
    giveMutex();
  }
  return false;
}

bool ChatBackend::busy() const {
  return _lastAccess > 0;
}

void ChatBackend::sendMessage(const char* type, const char* message) {
  auto packet = new tasks::MessagePacket{ type, message };
  ::xQueueSend(_resources.queue, &packet, 0);
}

void ChatBackend::sendMessage(const char* type) {
  auto packet = new tasks::MessagePacket{ type };
  ::xQueueSend(_resources.queue, &packet, 0);
}

void ChatBackend::takeMutex() {
  ::xSemaphoreTake(_resources.mutex, portMAX_DELAY);
}

void ChatBackend::giveMutex() {
  ::xSemaphoreGive(_resources.mutex);
}

}  // chat_backend
