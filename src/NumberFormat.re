[@mel.scope "navigator"] external locale: string = "language";

type options = {notation: option(string)};
type t = {format: int => string};

[@mel.new] [@mel.scope "Intl"]
external createFormatter: (~locale: string, ~options: options) => t =
  "NumberFormat";
