module CountrySelect = {
  type countryEntry = {
    label: string,
    value: string,
    count: int,
  };

  let countriesUrl = "https://gist.githubusercontent.com/rusty-key/659db3f4566df459bd59c8a53dc9f71f/raw/4127f9550ef063121c564025f6d27dceeb279623/counties.json";

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
      |> then_(json => {countryArrayDecoder(json) |> resolve})
    );

  let buttonHeight = 26;

  let numberFormatter =
    NumberFormat.createFormatter(
      ~locale=NumberFormat.locale,
      ~options={notation: Some("compact")},
    );

  let numFormat = numberFormatter.format;

  [@react.component]
  let make =
      (
        ~className: string="",
        ~country: option(string),
        ~onChange: option(string) => unit,
      ) => {
    let (isOpen, setIsOpen) = React.useState(_ => false);

    let (countriesList, setCountriesList) = React.useState(_ => None);

    let (searchStr, setSearchStr) = React.useState(_ => "");

    // The index of the items in the dropdown that is "selected"
    let (focusedInDropdownIndex, setFocusedInDropdownIndex) =
      React.useState(_ => 0);

    React.useEffect1(
      () => {
        Js.Promise.(
          requestEntries()
          |> then_(array => {
               let list = array |> Array.to_list;

               let sorted =
                 List.sort(
                   (a, b) => {
                     let aCount = a.count;
                     let bCount = b.count;
                     // We're displaying this with the highest count at the top
                     aCount < bCount ? 1 : aCount > bCount ? (-1) : 0;
                   },
                   list,
                 );

               setCountriesList(_ => Some(sorted));
               resolve();
             })
          |> catch(_ => {
               // @TODO: might want to use a Result type instead of an option here – or even better, something like the Elm RemoteData type
               resolve()
             })
        )
        |> ignore;

        None;
      },
      // Only run on initial mount
      [||],
    );

    // The full entry of the currently set country as set in the country prop
    let currentCountryEntry =
      React.useMemo2(
        () => {
          switch (country) {
          | None => None
          | Some(countryCode) =>
            Option.bind(countriesList, list => {
              List.find_opt(
                countryEntry => countryEntry.value == countryCode,
                list,
              )
            })
          }
        },
        (country, countriesList),
      );

    let rowHeight = 28;

    let filteredList =
      switch (countriesList) {
      | Some(list) =>
        switch (searchStr) {
        | "" => list
        | _ =>
          List.filter(
            countryEntry => {
              let containsStr = str =>
                Js.String.includes(
                  String.lowercase_ascii(searchStr),
                  String.lowercase_ascii(str),
                );

              containsStr(countryEntry.label)
              || containsStr(countryEntry.value);
            },
            list,
          )
        }
      | None => []
      };

    let filteredListLen = List.length(filteredList);

    let rec boundAndModIndex = (index: int) =>
      index == 0
        ? 0
        : index < 0
            ? boundAndModIndex(filteredListLen + index)
            : index mod filteredListLen;

    let boundedAndModdedFocusIndex = boundAndModIndex(focusedInDropdownIndex);

    let focusedInDropdownEntryOpt =
      switch (filteredListLen) {
      | 0 =>
        // To prevent divide by zero errors
        None

      | _ => List.nth_opt(filteredList, boundedAndModdedFocusIndex)
      };

    // For easier comparisons
    let selectedValue =
      currentCountryEntry
      |> Option.map(entry => entry.value)
      |> Option.value(~default="");

    let focusedValue =
      focusedInDropdownEntryOpt
      |> Option.map(entry => entry.value)
      |> Option.value(~default="");

    Js.log4(
      filteredList,
      focusedInDropdownIndex,
      focusedInDropdownEntryOpt,
      focusedValue,
    );

    React.useEffect1(
      () => {
        let enterHandler: ReactEvent.Keyboard.t => unit =
          event => {
            let key = ReactEvent.Keyboard.key(event);
            Js.log(key);

            switch (key) {
            | "ArrowUp" => setFocusedInDropdownIndex(curr => curr - 1)
            | "ArrowDown" => setFocusedInDropdownIndex(curr => curr + 1)

            | "Enter" =>
              switch (focusedInDropdownEntryOpt) {
              | Some(focused) =>
                Js.log(focused);
                onChange(Some(focused.value));
                setIsOpen(_ => false);
                setSearchStr(_ => "");
              | None => ()
              }

            | _ => ()
            };
          };

        EventListener.add_keyboard_event_listener("keyup", enterHandler);

        Some(
          () =>
            EventListener.remove_keyboard_event_listener(
              "keyup",
              enterHandler,
            ),
        );
      },
      [|focusedInDropdownEntryOpt|],
    );

    let rowRenderer: VirtualizedList.rowRenderer =
      ({key, index, style, _}) => {
        let thisCountryEntry = List.nth(filteredList, index);

        <div
          key
          style
          tabIndex=(-1)
          className={
            "option"
            ++ (thisCountryEntry.value == selectedValue ? " selected" : "")
            ++ (thisCountryEntry.value == focusedValue ? " focused" : "")
          }
          key={countryEntry.value}
          onClick={_ => {
            onChange(Some(thisCountryEntry.value));
            setIsOpen(_ => false);
          }}>
          <>
            <div className="option-flag">
              <span className={"fi fib fi-" ++ thisCountryEntry.value} />
            </div>
            <p className="option-text">
              {React.string(thisCountryEntry.label)}
            </p>
            <p className="option-count">
              {thisCountryEntry.count |> numFormat |> React.string}
            </p>
          </>
        </div>;
      };

    <div className={"country-select " ++ className}>
      <button
        className="button"
        onClick={_ => setIsOpen(open_ => !open_)}
        style={ReactDOM.Style.make(
          ~minWidth=?
            Option.is_none(currentCountryEntry)
              ? Some("var(--emptyButtonWidth)") : None,
          (),
        )}>
        {switch (currentCountryEntry) {
         | Some(entry) =>
           <div className="button-selected">
             <div className="button-flag">
               <span className={"fi fib fi-" ++ entry.value} />
             </div>
             {React.string(entry.label)}
           </div>
         | None => <div />
         }}
        <i className="fa-solid fa-caret-down" />
      </button>
      <div className="dropdown-position-container">
        {isOpen
           ? <div className="dropdown-panel">
               <div className="search-field">
                 <div className="search-icon">
                   <i className="fa-solid fa-magnifying-glass" />
                 </div>
                 <input
                   className="search-field-input"
                   placeholder="Search"
                   type_="text"
                   onChange={e =>
                     setSearchStr(_ => ReactEvent.Form.target(e)##value)
                   }
                   value=searchStr
                 />
               </div>
               <div className="search-list-separator" />
               <VirtualizedList
                 rowRenderer
                 rowCount=filteredListLen
                 width=230
                 rowHeight
                 height={min(400, filteredListLen * rowHeight)}
                 scrollToIndex=boundedAndModdedFocusIndex
               />
             </div>
           : React.null}
      </div>
    </div>;
  };
};
