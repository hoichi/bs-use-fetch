/**
 * The {{: https://developer.mozilla
 .org/en-US/docs/Web/API/Request } Request object }
 * Reexported from [ bs-fetch ] for convenience.
 * Created using [ UseFetch.Request.make() ] or suchlike. Please note
 that there’s no built-in memoization neither in those constructor functions
 nore in the hooks, so if you call them inside your component, it’s your
 responsibility to wrap them in [ useMemo ] to prevent an infinite effect loop.
 */
type request = Fetch.request;

/**
 * Resource, the first Fetch parameter. Either a url string, or a Fetch object.
 */
type fetchResource =
  | Url(string)
  | Request(request);

/**
 * The {{: https://developer.mozilla
    .org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/fetch#Parameters } init
    options} object.
  * Rexported from [ bs-fetch ].
  * Created using [ UseFetch.RequestInit.make() ]. Please note that the
  constructor functions is not memoized out of the box, so if you call it
  inside your component, it’s your responsibility to wrap them in [ useMemo
  ] to prevent an infinite effect loop.
 */
type requestInit = Fetch.requestInit;

/**
 * A fetch error, a polymorphic variant.
 * See http://keleshev.com/composable-error-handling-in-ocaml
 * Also, see examples/basicExample.re
 */
type fetchError = [ | `FetchError(Js.Promise.error)];

/**
 * The type that all the hooks return.
 * Polymorphic towards its error variant, because see above.
 * The data type is also parametrised, to make functions like [ mapOk ]
 possible.
 */
type t('d, 'e) =
  | Fetching
  | Refetching(Belt.Result.t('d, [> fetchError] as 'e))
  | Complete(Belt.Result.t('d, [> fetchError] as 'e));

/**
 * The hook that runs a fetch right away.
 * @param resource (fetchResource). Either Url(string) or Request(request) (aka {{: https://developer.mozilla
 .org/en-US/docs/Web/API/Request } request object}).
 * @param ~init (requestInit=?) {{:https://developer.mozilla
 .org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/fetch#Parameters} Init
 options}. Created [ using UseFetch.RequestInit.make() ]
 * @return t(Js.Json.t, [> fetchError])
 */
let useFetch: (~init: requestInit=?, fetchResource) => t(Js.Json.t, 'e);

/**
 * Maps an successful fetch data. Most likely usage is to decode json.
 * @param t ([ t ]) The value returned by [ useFetch... ]
 * @param f ([ 'a => result('b, 'e) ])
 * @return [ t('b, 'e) ]
 */
let mapOk: (t('a, 'e), 'a => result('b, 'e)) => t('b, 'e);

/**
 * A helper converting the hook result (a sum type) to a product type
 * of [ { loading, data, error } ] that might be more familiar and occasionally
 * more convenient, depending on your UX conventions.
 * @param t ([ t ]) The value returned by a [ useFetch... ] hook
 * @return [ (bool, option('data), option('error) ]
 */
let toLoadingDataAndError: t('d, 'e) => (bool, option('d), option('e));

/**
  Some modules reexported from bs-fetch for convenience:
  you probably don’t want to clutter your package.json and bsconfig
 *******************************************************************/
module HeadersInit = Fetch.HeadersInit;
module Headers = Fetch.Headers;
module BodyInit = Fetch.BodyInit;
module Body = Fetch.Body;
module RequestInit = Fetch.RequestInit;
module Request = Fetch.Request;
