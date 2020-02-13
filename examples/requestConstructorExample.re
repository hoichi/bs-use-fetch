[@react.component]
let make = () => {
  let (language, setLanguage) = React.useState(() => "reason");

  let request =
    React.useMemo1(
      () =>
        UseFetch.Request.make(
          "https://api.github.com/search/repositories"
          ++ "?sort=stars&order=desc&q=language:"
          ++ language,
        ),
      [|language|],
    );

  let fetchResult =
    UseFetch.(
      useFetch(Request(request))
      ->mapOk(r => GhRepo.t_decode(r)->Decode.mapDecodingError)
    );

  <main>
    <div>
      <input
        value=language
        onChange={event => setLanguage(ReactEvent.Form.target(event)##value)}
      />
    </div>
    {switch (fetchResult) {
     | Idle => ReasonReact.null
     | Fetching
     | Refetching(_) => ReasonReact.string("Loading...")
     | Complete(Error(_)) =>
       <div> {ReasonReact.string("Something went wrong")} </div>
     | Complete(Ok(({items}: GhRepo.t))) =>
       <ul>
         {Belt.Array.map(items, ({fullName, htmlUrl}: GhRepo.repo) =>
            <li key=fullName>
              <a href=htmlUrl> {ReasonReact.string(fullName)} </a>
            </li>
          )
          ->React.array}
       </ul>
     }}
  </main>;
};
