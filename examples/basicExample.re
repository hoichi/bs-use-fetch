module Component = {
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
          // below, no distinction is made between fetching and refetcsplithing,
          // but you're free to make other UX choices
          fun
          | Fetching => ReasonReact.string("Loading...")
          | Refetching(Ok(({items}: GhRepo.t)))
          | Complete(Ok(({items}: GhRepo.t))) =>
            <ul>
              {Belt.Array.map(items, ({fullName, htmlUrl}: GhRepo.repo) =>
                <li key=fullName>
                  <a href=htmlUrl> {ReasonReact.string(fullName)} </a>
                </li>
              )
              ->React.array}
            </ul>
          | Refetching(Error(`FetchError(_)))
          | Complete(Error(`FetchError(_))) =>
            <div> {ReasonReact.string("Fetch error!")} </div>
          | Refetching(Error(`DecodeError((err: Decco.decodeError))))
          | Complete(Error(`DecodeError((err: Decco.decodeError)))) =>
            <div>
              <h2> {ReasonReact.string("Decode error!")} </h2>
              <ul>
                <li> {ReasonReact.string(err.path)} </li>
                <li> {ReasonReact.string(err.message)} </li>
                <li> {ReasonReact.string(err.value->Js.Json.stringify)} </li>
              </ul>
            </div>
        )
    );
  };
};
