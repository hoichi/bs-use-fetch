open Jest;
open Expect;
//open ReasonHooksTestingLibrary;
open BsJestFetchMock;

open UseFetch;

exception Rejected(string);

external promiseErrorAsExn_exn: Js.Promise.error => exn = "%identity";
let getFetchExn =
  fun
  | Complete(Error(`FetchError(e))) => Some(promiseErrorAsExn_exn(e))
  | Refetching(Error(`FetchError(e))) => Some(promiseErrorAsExn_exn(e))
  //
  | Idle
  | Fetching
  | Complete(Ok(_))
  | Refetching(Ok(_)) => None;

/*
 beforeEach(() => JestFetchMock.resetMocks());
 */

describe("useSubmit", () => {
  testAsync("Sucsessful submitting", finish => {
    JestFetchMock.mockResponseOnce(~response=Str({|{ "body": "ok"}|}), ());

    let HooksTesting.RenderResult.{result, waitForNextUpdate} =
      HooksTesting.renderHook(() => useSubmit(Url("https://what.evs")), ());

    let (stateInitial, submit, _) = result.current;
    HooksTesting.act(submit);
    let (stateFetching, _, _) = result.current;

    HooksTesting.actAsync(() => waitForNextUpdate(.))
    |> Js.Promise.(
         then_(_ => {
           let (stateDone, _, _) = result.current;

           expect((stateInitial, stateFetching, stateDone))
           |> toEqual((
                Idle,
                Fetching,
                Complete(Ok(Js.Json.parseExn({|{ "body": "ok"}|}))),
              ))
           |> finish
           |> resolve;
         })
       )
    |> ignore;
  });

  testAsync("Rejected sumbitting", finish => {
    let because = "because";
    JestFetchMock.mockRejectOnce(
      Fn(_ => Rejected(because) |> Js.Promise.reject),
    );

    let HooksTesting.RenderResult.{result, waitForNextUpdate} =
      HooksTesting.renderHook(() => useSubmit(Url("https://what.evs")), ());

    let (stateInitial, submit, _) = result.current;
    HooksTesting.act(submit);
    let (stateFetching, _, _) = result.current;

    HooksTesting.actAsync(() => waitForNextUpdate(.))
    |> Js.Promise.(
         then_(_ => {
           let (stateDone, _, _) = result.current;
           let doneExn = getFetchExn(stateDone);

           expect((stateInitial, stateFetching, doneExn))
           |> toEqual((Idle, Fetching, Some(Rejected(because))))
           |> finish
           |> resolve;
         })
       )
    |> ignore;
  });
});
