// Keyboard

external add_keyboard_event_listener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "addEventListener";

external remove_keyboard_event_listener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "removeEventListener";

// Pointer

external add_pointer_event_listener:
  (string, ReactEvent.Pointer.t => unit) => unit =
  "addEventListener";

external remove_pointer_event_listener:
  (string, ReactEvent.Pointer.t => unit) => unit =
  "removeEventListener";
