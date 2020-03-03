open Jest;
open Expect;
//open ReasonHooksTestingLibrary;
open BsJestFetchMock;

open UseFetch;

beforeEach(() => JestFetchMock.resetMocks());

describe("useSubmit", () => {
  let container =
    HooksTesting.renderHook(
      () =>
        useSubmit(
          Url(
            "https://api.github.com/search/repositories?q=language:reason&sort=stars&order=desc",
          ),
        ),
      (),
    );

  let HooksTesting.RenderResult.{result, waitForNextUpdate} = container;

  testAsync("Sucsessful submitting", finish => {
    JestFetchMock.mockResponse(~response=Str({|{ "body": "ok"}|}), ());

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
});
