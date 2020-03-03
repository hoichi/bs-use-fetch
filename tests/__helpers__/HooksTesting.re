module RenderResult = {
  type current('v) = {current: 'v};

  type t('v) = {
    result: current('v),
    waitForNextUpdate: (. unit) => Js.Promise.t(unit),
  };
};

module Options = {
  [@bs.deriving {abstract: light}]
  type t('props) = {
    [@bs.optional]
    initialProps: 'props,
    [@bs.optional]
    wrapper: React.component({. "children": React.element}),
  };
};

[@bs.module "@testing-library/react-hooks"]
external renderHook:
  ([@bs.uncurry] ('props => 'hook), ~options: Options.t('props)=?, unit) =>
  RenderResult.t('hook) =
  "renderHook";

[@bs.deriving {abstract: light}]
type waitOptions = {
  [@bs.optional]
  timeout: int,
  [@bs.optional]
  suppressError: bool,
};

[@bs.module "@testing-library/react-hooks"]
external jsAct: ([@bs.uncurry] (unit => Js.undefined('a))) => unit = "act";

let act = callback =>
  jsAct(() => {
    callback();
    Js.undefined;
  });

[@bs.module "@testing-library/react-hooks"]
external actAsync:
  ([@bs.uncurry] (unit => Js.Promise.t('a))) => Js.Promise.t('a) =
  "act";

[@bs.module "@testing-library/react-hooks"]
external waitForNextUpdate: (. unit) => Js.Promise.t(unit) =
  "waitForNextUpdate";

[@bs.module "@testing-library/react-hooks"]
external waitForNextUpdateWithOptions: (. waitOptions) => Js.Promise.t(unit) =
  "waitForNextUpdate";
