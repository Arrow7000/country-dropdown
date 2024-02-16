module CountrySelect = {
  let makeResolvingPromise = (callback, ms) => {
    Js.Global.setTimeout(callback, ms) |> ignore;
  };

  let countriesUrl = "https://gist.githubusercontent.com/rusty-key/659db3f4566df459bd59c8a53dc9f71f/raw/4127f9550ef063121c564025f6d27dceeb279623/counties.json";

  type countryEntry = {
    label: string,
    value: string,
    count: int,
  };

  let entryDecoder = json => {
    Json.Decode.{
      label: json |> field("label", string),
      value: json |> field("value", string),
      count: Random.int(500_000),
    };
  };

  let countryArrayDecoder = Json.Decode.array(entryDecoder);

  let requestEntries = () =>
    Js.Promise.(
      Fetch.fetch(countriesUrl)
      |> then_(Fetch.Response.json)
      |> then_(json => {
           Js.log(json);

           countryArrayDecoder(json) |> resolve;
         })
    );

  let buttonHeight = 26;

  [@react.component]
  let make =
      (
        ~className: string="",
        ~selectedCountry: option(string),
        ~onChange: option(string) => unit=_ => (),
      ) => {
    let (isOpen, setIsOpen) = React.useState(_ => false);

    let (countriesList, setCountriesList) = React.useState(_ => None);

    React.useEffect1(
      () => {
        Js.Promise.(
          requestEntries()
          |> then_(array => {
               // This does a sneaky in-place sort, so we're mutating `array` here
               Array.sort(
                 (a, b) => {
                   let aCount = a.count;
                   let bCount = b.count;
                   // We're displaying this with the highest count at the top
                   aCount < bCount ? 1 : aCount > bCount ? (-1) : 0;
                 },
                 array,
               );

               setCountriesList(_ => Some(array));
               resolve();
             })
          |> catch(_ => {
               //  setCountriesList(_ => None);
               resolve()
             })
        )
        |> ignore;

        None;
      },
      // Only run on initial mount
      [||],
    );

    let selectedEntry =
      React.useMemo2(
        () => {
          switch (selectedCountry) {
          | None => None
          | Some(countryCode) =>
            Option.bind(countriesList, list => {
              Array.find_opt(
                countryEntry => countryEntry.value == countryCode,
                list,
              )
            })
          }
        },
        (selectedCountry, countriesList),
      );

    // For easier comparisons
    let selectedValue =
      selectedEntry
      |> Option.map(entry => entry.value)
      |> Option.value(~default="");

    let numberFormatter =
      NumberFormat.createFormatter(
        ~locale=NumberFormat.locale,
        ~options={notation: Some("compact")},
      );

    let numFormat = numberFormatter.format;

    <div className={"countrySelect " ++ className}>
      <button
        className="button"
        onClick={_ => setIsOpen(open_ => !open_)}
        style={ReactDOM.Style.make(
          ~minWidth=?
            Option.is_none(selectedEntry)
              ? Some("var(--emptyButtonWidth)") : None,
          (),
        )}>
        {switch (selectedEntry) {
         | Some(entry) =>
           <div className="button-selected">
             <span className={"fi fi-" ++ entry.value} />
             {React.string(entry.label)}
           </div>
         | None => <div />
         }}
        <i className="fa-solid fa-caret-down" />
      </button>
      <div className="dropdown-position-container">
        {isOpen
           ? <div className="dropdown-panel">
               {switch (countriesList) {
                | Some(list) =>
                  list
                  |> Array.map(countryEntry =>
                       <div
                         className={
                           "option "
                           ++ (
                             countryEntry.value == selectedValue
                               ? "selected" : ""
                           )
                         }
                         key={countryEntry.value}
                         onClick={_ => {
                           onChange(Some(countryEntry.value));
                           setIsOpen(_ => false);
                         }}>
                         <span className={"fi fi-" ++ countryEntry.value} />
                         <p className="option-text">
                           {React.string(countryEntry.label)}
                         </p>
                         <p className="option-count">
                           {countryEntry.count |> numFormat |> React.string}
                         </p>
                       </div>
                     )
                  |> React.array
                | None =>
                  // @TODO: make this look good
                  <div className="no-results">
                    {React.string("No countries found")}
                  </div>
                }}
             </div>
           : React.null}
      </div>
    </div>;
  };
};
