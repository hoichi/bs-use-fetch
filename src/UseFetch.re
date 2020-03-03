type request = Fetch.request;

type resource =
  | Url(string)
  | Request(request);

type requestInit = Fetch.requestInit;

type fetchError = [ | `FetchError(Js.Promise.error)];

type t('d, 'e) =
  | Idle
  | Fetching
  | Refetching(Belt.Result.t('d, [> fetchError] as 'e))
  | Complete(Belt.Result.t('d, [> fetchError] as 'e));

/*
  Operations with t
 */
module FetchState = {
  let init = _ => Idle;

  let start =
    fun
    | Idle
    | Fetching => Fetching
    | Refetching(r)
    | Complete(r) => Refetching(r);

  let complete = result => Complete(result);

  let abort =
    fun
    | Idle
    | Fetching => Idle
    | Refetching(r)
    | Complete(r) => Complete(r);
};

/*
   Inner implementation. Takes a function that runs fetch and return a promise.
   The same function is also used as a dependecy, so it’s supposed to be run
   through useCallback.
 */
let useFetch_ = makeAPromise => {
  let (state, setState) = React.useState(FetchState.init);

  React.useEffect1(
    () => {
      // start fetching
      setState(FetchState.start);

      let cancelled = ref(false);

      let withRollback = (next, prev) =>
        if (! cancelled^) {
          next;
        } else {
          FetchState.abort(prev);
        };

      Js.Promise.(
        makeAPromise()
        |> then_(Fetch.Response.json)
        |> then_(
             // finish fetching
             json =>
             setState(Complete(Ok(json))->withRollback) |> resolve
           )
        |> catch(error =>
             setState(Complete(Error(`FetchError(error)))->withRollback)
             |> resolve
           )
        |> ignore
      );

      /*
         how do we abort w/o
       */

      Some(_ => cancelled := true);
    },
    [|makeAPromise|],
  );

  state;
};

let useFetchWithInit = (url, init) =>
  React.useCallback2(() => Fetch.fetchWithInit(url, init), (url, init))
  ->useFetch_;

let useFetchWithRequestInit = (request, init) =>
  React.useCallback2(
    () => Fetch.fetchWithRequestInit(request, init),
    (request, init),
  )
  ->useFetch_;

let defaultInit = Fetch.RequestInit.make();
let defaultDerive = (_, r, i) => (r, i);
type deriveFetchParams('a) =
  ('a, resource, requestInit) => (resource, requestInit);

let useFetch = (~init=defaultInit, resource) =>
  switch (resource) {
  | Url(u) => useFetchWithInit(u, init)
  | Request(r) => useFetchWithRequestInit(r, init)
  };

let useSubmit =
    (
      ~init=defaultInit,
      ~deriveFetchParams: deriveFetchParams('a)=defaultDerive,
      resource,
    )
    : (t(Js.Json.t, 'e), 'a => unit, unit => unit) => {
  let (state, setState) = React.useState(FetchState.init);
  let cancelled = ref(false); // not that the value matters before submit starts

  let submit = submitParams => {
    let (resource, init) = deriveFetchParams(submitParams, resource, init);
    cancelled := false;

    setState(FetchState.start);

    let withRollback = (next, prev) =>
      if (! cancelled^) {
        FetchState.complete(next);
      } else {
        FetchState.abort(prev);
      };

    let fetchPromise =
      switch (resource) {
      | Url(u) => Fetch.fetchWithInit(u, init)
      | Request(r) => Fetch.fetchWithRequestInit(r, init)
      };

    Js.Promise.(
      fetchPromise
      |> then_(Fetch.Response.json)
      |> then_(json => setState(withRollback(Ok(json))) |> resolve)
      |> catch(error =>
           setState(withRollback(Error(`FetchError(error)))) |> resolve
         )
      |> ignore
    );
  };

  /*
   Note that abort is quite rudimentary so far. We just set a flag that tells
   us to discard any fetch result that comes after calling abort().
   Also note that currently it’s possible for a fetch called earlier.
   (that may have stale params) to finish last.
   And we don’t in any way prevent multiple submits (and we probably
   shouldn’t).
   */
  let abort = () => cancelled := true;

  (state, submit, abort);
};

/**
 * Applies a function to an OK result, i.e., a successfuly fetch data.
 * Intended for decoding. If the function is heavy (or there’s a lot of json)
 * this may impair your render, but for the moment no async alternative
 * for decoding is provided.
 */
let mapOk = (t, f) =>
  switch (t) {
  | Complete(Ok(r)) => Complete(f(r))
  | Refetching(Ok(r)) => Refetching(f(r))
  //
  | Idle as same
  | Fetching as same
  | Complete(Error(_)) as same
  | Refetching(Error(_)) as same => same
  };

/**
 * Converts t to a tuple conisting of:
 * - loading: bool
 * - data: option('d) (Js.Json.t, if you didn’t map it to anything else)
 * - error: option([> fetchError])
 */
let toLoadingDataAndError =
  fun
  | Idle
  | Fetching => (true, None, None)
  | Refetching(Ok(r)) => (true, Some(r), None)
  | Refetching(Error(e)) => (true, None, Some(e))
  | Complete(Ok(r)) => (false, Some(r), None)
  | Complete(Error(e)) => (false, None, Some(e));

module HeadersInit = Fetch.HeadersInit;
module Headers = Fetch.Headers;
module BodyInit = Fetch.BodyInit;
module Body = Fetch.Body;
module RequestInit = Fetch.RequestInit;
module Request = Fetch.Request;
