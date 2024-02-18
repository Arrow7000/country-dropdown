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
        <h1> {React.string("Country select")} </h1>
        <h2> {React.string("Click to select a country")} </h2>
        <CountrySelect.CountrySelect
          className="country-select-in-app"
          country
          onChange={newCountry => {
            Js.log(newCountry);

            setCountry(_ => newCountry);
          }}
        />
        <p>
          {React.string(
             "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec lobortis maximus feugiat. Nulla semper vulputate leo, feugiat tempus erat efficitur in. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nullam lacus justo, iaculis sit amet egestas quis, elementum nec sem. Mauris efficitur nulla at tellus tempor sollicitudin. Donec sagittis dapibus urna, eget varius ex scelerisque ut. Aliquam viverra, erat eu hendrerit viverra, libero turpis placerat lorem, pulvinar feugiat lacus velit a eros. Interdum et malesuada fames ac ante ipsum primis in faucibus. Duis et sem orci. Morbi arcu tortor, pretium ut ultricies sed, porta quis libero. Phasellus sit amet libero libero. Duis at mauris mi.",
           )}
        </p>
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
