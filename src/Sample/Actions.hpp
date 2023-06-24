// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef SAMPLE_ACTIONS_HPP_
#define SAMPLE_ACTIONS_HPP_

#include <Actions.hpp>

namespace sample {

class Actions : public chat_backend::Actions {
  unsigned long _lastMonologue = 0;
 public:
  void begin() override;
  bool loop(Resources& resources) override;

  bool onTouched(int x, int y, Resources& resources) override;
  bool onButtonClicked(int button, Resources& resources) override;
  bool onButtonDoubleClicked(int button, Resources& resources) override;
  bool onAvailableStream(Stream& stream, Resources& resources) override;
  bool onEnterIdle(Resources& resources) override;
  // bool onWakeup(Resources& resources) override;
  bool onBeginListening(Resources& resources) override;
  bool onEndListening(Resources& resources) override;
  bool onBeginThinking(Resources& resources) override;
  bool onEndThinking(Resources& resources) override;
  bool onBeginRecording(Resources& resources) override;
  bool onEndRecording(Resources& resources) override;

 private:
  void beep(Resources& resources) const { beep(1, resources); }
  void beep(int n, Resources& resources) const;
  void reportBatteryLevel(Resources& resources);

  void switchMonologueMode(Resources& resources);
  bool isMonologueMode() const;
  bool monologue(Resources& resources);
};

}  // sample

#endif  // SAMPLE_ACTIONS_HPP_
