// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef CHATBACKEND_MODULES_WAKETRIGGER_HPP_
#define CHATBACKEND_MODULES_WAKETRIGGER_HPP_

namespace chat_backend::modules {

class WakeTrigger {
 public:
  virtual void begin() {}
  virtual bool canWake() const { return false; }

  virtual bool record() { return false; }
  virtual bool listen() { return false; }

 protected:
  void detected();
};

}  // chat_backend::modules

#endif  // CHATBACKEND_MODULES_WAKETRIGGER_HPP_
