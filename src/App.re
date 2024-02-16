module App = {
  // open CountrySelect;

  type countryCode = option(string);

  [@react.component]
  let make = () => {
    let (selectedCountry, setSelectedCountry): (
      countryCode,
      Helpers.stateSetter(countryCode),
    ) =
      React.useState(() => Some("us"));

    let simpleSetCountry = c => setSelectedCountry(_ => c);

    <div className="app">
      <div className="container">
        <CountrySelect.CountrySelect
          className="country-select"
          selectedCountry
          onChange=simpleSetCountry
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
