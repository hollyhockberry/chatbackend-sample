// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_ACTIONS_HPP_
#define CHATBACKEND_ACTIONS_HPP_

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <Stream.h>

namespace chat_backend {

class Actions {
 public:
  struct Resources {
    QueueHandle_t queue;
    SemaphoreHandle_t mutex;
  };  
  virtual void begin() {}
  virtual bool loop(Resources& resources) { return false; }
  virtual void handleMessage(const String& type, const String& content) {}
  virtual bool onEnterIdle(Resources& resources) { return false; }
  virtual bool onWakeup(Resources& resources) { return false; }
  virtual bool onBeginListening(Resources& resources) { return false; }
  virtual bool onEndListening(Resources& resources) { return false; }
  virtual bool onBeginThinking(Resources& resources) { return false; }
  virtual bool onEndThinking(Resources& resources) { return false; }
  virtual bool onBeginRecording(Resources& resources) { return false; }
  virtual bool onEndRecording(Resources& resources) { return false; }
  virtual bool onTouched(int x, int y, Resources& resources) { return false; }
  virtual bool onButtonClicked(int button, Resources& resources) { return false; }
  virtual bool onButtonDoubleClicked(int button, Resources& resources) { return false; }
  virtual bool onAvailableStream(Stream& stream, Resources& resources) { return false; }
//TODO(hollyhockberry):
  // virtual bool onError(Resources& resources) { return false; }

 protected:
  static void sendMessage(const char* type, const char* message, Resources& resources);
  static void sendMessage(const char* type, Resources& resources);
  static void takeMutex(Resources& resources);
  static void giveMutex(Resources& resources);
};

}  // chat_backend

#endif // CHATBACKEND_ACTIONS_HPP_

