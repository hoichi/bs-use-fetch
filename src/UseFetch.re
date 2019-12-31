// - http://keleshev.com/composable-error-handling-in-ocaml
type fetchError = [ | `FetchError(Js.Promise.error)];

type t('d, 'e) =
  | Fetching
  | Refetching(Belt.Result.t('d, [> fetchError] as 'e))
  | Complete(Belt.Result.t('d, [> fetchError] as 'e));

let useFetch = url => {
  let (state, setState) = React.useState(_ => Fetching);

  React.useEffect1(
    () => {
      setState(
        fun
        | Fetching => Fetching
        | Refetching(r)
        | Complete(r) => Refetching(r),
      );

      let cancelled = ref(false);

      let withRollback = (next, prev) =>
        if (! cancelled^) {
          next;
        } else {
          // The only branch where we do something useful is the last one,
          // but let’s be exhaustive just in case
          switch (prev) {
          | Fetching as same
          | Complete(_) as same => same
          | Refetching(r) => Complete(r)
          };
        };

      Js.Promise.(
        Fetch.fetch(url)
        |> then_(Fetch.Response.json)
        |> then_(json =>
             setState(prev => Complete(Ok(json))->withRollback(prev))
             |> resolve
           )
        |> catch(error =>
             setState(prev =>
               Complete(Error(`FetchError(error)))->withRollback(prev)
             )
             |> resolve
           )
        |> ignore
      );

      Some(_ => cancelled := true);
    },
    [|url|],
  );

  state;
};

/**
 * Applies a function to an OK result, i.e., a successfuly fetch data.
 * Intended for decoding. If the function is heavy (or there’s a lot of json)
 * this may impair your render, but for the moment no async alternative
 * for decoding is provided.
 */
let mapOk = (t, f) =>
  switch (t) {
  | Fetching => Fetching
  | Complete(Ok(r)) => Complete(f(r))
  | Complete(Error(_)) as e => e
  | Refetching(Ok(r)) => Refetching(f(r))
  | Refetching(Error(_)) as e => e
  };

/**
 * Converts t to a tuple conisting of:
 * - loading: bool
 * - data: option('d) (Js.Json.t, if you didn’t map it to anything else)
 * - error: option([> fetchError])
 */
let toLoadingDataAndError =
  fun
  | Fetching => (true, None, None)
  | Refetching(Ok(r)) => (true, Some(r), None)
  | Refetching(Error(e)) => (true, None, Some(e))
  | Complete(Ok(r)) => (false, Some(r), None)
  | Complete(Error(e)) => (false, None, Some(e));
