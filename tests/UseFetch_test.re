open Jest;
open Expect;
//open ReasonHooksTestingLibrary;
open BsJestFetchMock;

module T = ReasonHooksTestingLibrary.Testing;

open UseFetch;

beforeEach(() => JestFetchMock.resetMocks());

describe("useSubmit", () => {
  let container =
    T.renderHook(
      () =>
        useSubmit(
          Url(
            "https://api.github.com/search/repositories?q=language:reason&sort=stars&order=desc",
          ),
        ),
      (),
    );

  test("Initial state", () => {
    let (state, _, _) = T.Result.(container->result->current);

    expect(state) |> toBe(Idle);
  });

  test("Submitting", () => {
    let (_, submit, _) = T.Result.(container->result->current);

    T.act(() => {
      JestFetchMock.mockResponse(~response=Str({|{ "body": "ok"}|}), ());
      submit();
    });

    let (state, _, _) = T.Result.(container->result->current);
    expect(state) |> toBe(Fetching);
  });
});
