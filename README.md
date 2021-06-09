# bs-use-fetch

A React useFetch hook. It:
- wraps [bs-fetch](https://github.com/reasonml-community/bs-fetch)
- is composable
  - uses a `result` type
  - makes impossible state irrepresentable
  - lets you use a JSON decoding library of your choice
  - lets you compose fetch errors and decoding error via [polymorphic variants](http://keleshev.com/composable-error-handling-in-ocaml)

It seems workable, especially for getting data, but expect some breaking
changes before v1.0.

## Basic usage

```reason
[@react.component]
let make = () => {
  UseFetch.(
    // fetch the data
    useFetch(
      "https://api.github.com/search/repositories?q=language:reason&sort=stars&order=desc",
    )
    // use whichever decoding library you like
    ->mapOk(r => GhRepo.t_decode(r)->Decode.mapDecodingError)
    ->(
        // below, no distinction is made between fetching and refetching,
        // but you're free to make other UX choices
        fun
        | Fetching => React.string("Loading...")
        | Refetching(Ok(({items}: GhRepo.t)))
        | Complete(Ok(({items}: GhRepo.t))) =>
          <ul>
            {Belt.Array.map(items, ({fullName, htmlUrl}: GhRepo.repo) =>
               <li key=fullName>
                 <a href=htmlUrl> {React.string(fullName)} </a>
               </li>
             )
             ->React.array}
          </ul>
        | Refetching(Error(`FetchError(_)))
        | Complete(Error(`FetchError(_))) =>
          <h2> {React.string("Fetch error!")} </h2>
        | Refetching(Error(`DecodeError((err: Decco.decodeError))))
        | Complete(Error(`DecodeError((err: Decco.decodeError)))) =>
          <h2> {React.string("Decode error!")} </h2>
  );
};
```

(See the full example in `/examples`.)

Or, if you prefer the style prevalent in JS-land,

```reason
  let (loading, data, error) = UseFetch.(
    useFetch(
      "https://api.github.com/search/repositories?q=language:reason&sort=stars&order=desc",
    )
    ->mapOk(r => GhRepo.t_decode(r)->Decode.mapDecodingError)
    ->toLoadingDataAndError
  );

  // ...
```

## Documentation

See `UseFetch.rei`.

## TODOs

- [x] Add request/init parameters
- [ ] [Support submitting](https://github.com/hoichi/bs-use-fetch/issues/2)
- [ ] `toLoadingAndResult` helper
- [ ] use `AbortController` (`bs-fetch` has [a stale PR](https://github.com/reasonml-community/bs-fetch/pull/15) for same)
- [ ] tests (will probably require bindings for something like
      [jest-fetch-mock](https://github.com/jefflau/jest-fetch-mock))
- [ ] Generate API docs (the comments in the `.rei` should already be
      odoc-compatible.
- [ ] React Suspense compatibility?
