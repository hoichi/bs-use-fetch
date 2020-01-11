  [@decco]
  type repo = {
    [@decco.key "full_name"]
    fullName: string,
    [@decco.key "html_url"]
    htmlUrl: string,
  };
  [@decco]
  type t = {items: array(repo)};
