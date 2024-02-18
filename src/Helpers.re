open Webapi.Dom;

type stateSetter('t) = ('t => 't) => unit;

// Slightly adapted from https://github.com/MargaretKrutikova/use-click-outside-re/blob/master/src/ClickOutside.re
let handleClickOutside = (domElement: Dom.element, e: Dom.mouseEvent, fn) => {
  let targetElement = MouseEvent.target(e) |> EventTarget.unsafeAsElement;

  !(domElement |> Element.contains(targetElement)) ? fn(e) : ();
};

let useClickOutside = (onClickOutside: Dom.mouseEvent => unit, deps) => {
  let elementRef = React.useRef(Js.Nullable.null);

  let handleMouseDown = (e: Dom.mouseEvent) => {
    elementRef.current
    |> Js.Nullable.toOption
    |> Option.map(refValue =>
         handleClickOutside(refValue, e, onClickOutside)
       )
    |> ignore;
  };

  React.useEffect1(
    () => {
      Document.addMouseDownEventListener(handleMouseDown, document);
      Some(
        () =>
          Document.removeMouseDownEventListener(handleMouseDown, document),
      );
    },
    deps,
  );

  elementRef;
};
