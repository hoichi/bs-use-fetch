  type decodeError = [ | `DecodeError(Decco.decodeError)];

  let mapDecodingError =
    fun
    | Ok(x) => Ok(x)
    | Error(e) => Error(`DecodeError(e));
