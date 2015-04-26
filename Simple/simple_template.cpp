// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ppapi/cpp/var_dictionary.h>
#include <stdio.h>
#include <stdlib.h>

#include "ppapi_simple/ps_interface.h"
#include "ppapi_simple/ps_main.h"

// The main object that runs SimpleTemplate simulation.
class SimpleTemplate {
 public:
  SimpleTemplate();
  virtual ~SimpleTemplate() {};
  // Handle event from user, or message from JS.
  void HandleEvent(PSEvent* ps_event);
  bool isRunning() { return !quit_; };

 private:
  // Helper to post small update messages to JS.
  void PostUpdateMessage(const char* message_name, double value);
  bool quit_;

};

SimpleTemplate::SimpleTemplate()  {
  quit_ = false;
  PSEventSetFilter(PSE_ALL);
}

// Handle input events from the user and messages from JS.
void SimpleTemplate::HandleEvent(PSEvent* ps_event) {
  if (ps_event->type == PSE_INSTANCE_HANDLEMESSAGE) {
    // Convert Pepper Simple message to PPAPI C++ var
    pp::Var var(ps_event->as_var);
    if (var.is_dictionary()) {
      pp::VarDictionary dictionary(var);
      std::string message = dictionary.Get("message").AsString();
      if (message == "hello") {
        printf("Hello world!\n");
      }
      else if (message == "quit") {
        printf("Exiting\n");
        quit_ = true;
      }
    }
  }
}

// PostUpdateMessage() helper function for sendimg small messages to JS.
void SimpleTemplate::PostUpdateMessage(const char* message_name, double value) {
  pp::VarDictionary message;
  message.Set("message", message_name);
  message.Set("value", value);
  PSInterfaceMessaging()->PostMessage(PSGetInstanceId(), message.pp_var());
}

// Starting point for the module.  We do not use main since it would
// collide with main in libppapi_cpp.
int app_main(int argc, char* argv[]) {
  SimpleTemplate app;

  while (app.isRunning()) {
    PSEvent* ps_event;
    // Consume all available events.
    while ((ps_event = PSEventTryAcquire()) != NULL) {
      app.HandleEvent(ps_event);
      PSEventRelease(ps_event);
    }
  }

  return 0;
}

// Register the function to call once the Instance Object is initialized.
// see: pappi_simple/ps_main.h
PPAPI_SIMPLE_REGISTER_MAIN(app_main);
