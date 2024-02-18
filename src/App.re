module App = {
  // open CountrySelect;

  type countryCode = option(string);

  [@react.component]
  let make = () => {
    let (country, setCountry): (
      countryCode,
      Helpers.stateSetter(countryCode),
    ) =
      React.useState(() => Some("us"));

    <div className="app">
      <div className="container">
        <CountrySelect.CountrySelect
          className="country-select"
          country
          onChange={newCountry => {
            Js.log(newCountry);

            setCountry(_ => newCountry);
          }}
        />
      </div>
    </div>;
  };
};

ReactDOM.querySelector("#root")
->(
    fun
    | Some(root) => ReactDOM.render(<App />, root)
    | None =>
      Js.Console.error(
        "Failed to start React: couldn't find the #root element",
      )
  );
