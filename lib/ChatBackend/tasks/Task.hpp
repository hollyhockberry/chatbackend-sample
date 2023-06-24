// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_TASKS_TASK_HPP_
#define CHATBACKEND_TASKS_TASK_HPP_

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

namespace chat_backend::tasks {

class Task {
  const char* _taskName;
  const uint32_t _stackSize;
  QueueHandle_t _sendQueue = nullptr;
  SemaphoreHandle_t _resourceMutex = nullptr;
  struct TaskContext {
    void* task;
    TickType_t tick;
  } _context;

 public:
  Task(const char* taskName, uint32_t stackSize, TickType_t tick);
  void begin(QueueHandle_t queue, SemaphoreHandle_t mutex);

 protected:
  virtual void setup() {}
  virtual void loop() = 0;
  void sendMessage(const char* type, const char* message);
  void sendMessage(const char* type);
  void takeMutex();
  void giveMutex();

 private:
  static void main(void* arg);
};

}  // chat_backend::modules

#endif  // CHATBACKEND_TASKS_TASK_HPP_
