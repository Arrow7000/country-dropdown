external add_keyboard_event_listener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "addEventListener";

external remove_keyboard_event_listener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "removeEventListener";
