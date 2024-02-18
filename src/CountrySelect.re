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

  // Make API request for the country entries and sort by count descending
  let requestEntries = () =>
    Js.Promise.(
      Fetch.fetch(countriesUrl)
      |> then_(Fetch.Response.json)
      |> then_(json => {
           let sorted =
             json
             |> countryArrayDecoder
             |> Array.to_list
             |> List.sort(
                  (a, b) => {
                    let aCount = a.count;
                    let bCount = b.count;
                    // We're displaying this with the highest count at the top
                    aCount < bCount ? 1 : aCount > bCount ? (-1) : 0;
                  },
                  _,
                );
           sorted |> resolve;
         })
    );

  let rowHeight = 28;

  let numberFormatter =
    NumberFormat.createFormatter(
      ~locale=NumberFormat.locale,
      ~options={notation: Some("compact")},
    );
  let numFormat = numberFormatter.format;

  // Map any integer to a valid list index (or 0, if list is empty)
  let rec boundToListLenIndex = (index: int, listLen: int) =>
    listLen == 0
      ? 0
      : index < 0
          ? boundToListLenIndex(listLen + index, listLen) : index mod listLen;

  [@react.component]
  let make =
      (
        ~className: string="",
        ~country: option(string),
        ~onChange: option(string) => unit,
      ) => {
    let (isOpen, setIsOpen) = React.useState(_ => false);

    let (countriesList, setCountriesList) =
      React.useState(_ => RemoteData.NotAsked);

    let (searchStr, setSearchStr) = React.useState(_ => "");

    let (focusedInDropdownIndex, setFocusedInDropdownIndex) =
      React.useState(_ => 0);

    // We need this so we can avoid running the effect that sets the keyboard event handlers every time we hover over a different option. Inspired by: https://stackoverflow.com/a/63261270/3229534
    let focusedIndexRef = React.useRef(focusedInDropdownIndex);

    // Keep the ref in sync with state
    React.useEffect1(
      () => {
        focusedIndexRef.current = focusedInDropdownIndex;
        None;
      },
      [|focusedInDropdownIndex|],
    );

    // Reset all the states
    let closeDropdown = () => {
      setIsOpen(_ => false);
      setSearchStr(_ => "");
      setFocusedInDropdownIndex(_ => 0);
    };

    // Select country and reset all the states
    let selectCountry = (entry: countryEntry) => {
      onChange(Some(entry.value));
      closeDropdown();
    };

    // The full entry of the currently set country, as set in the country prop
    let currentCountryEntry: option(countryEntry) =
      React.useMemo2(
        () => {
          switch (country) {
          | None => None
          | Some(countryCode) =>
            countriesList
            |> RemoteData.toOption
            |> Option.bind(_, list => {
                 List.find_opt(
                   countryEntry => countryEntry.value == countryCode,
                   list,
                 )
               })
          }
        },
        (country, countriesList),
      );

    // Filtered by the search string
    let filteredList =
      React.useMemo2(
        () => {
          switch (countriesList) {
          | RemoteData.Success(list) =>
            switch (searchStr) {
            | "" => list
            | _ =>
              list
              |> List.filter(countryEntry => {
                   let containsStr = str =>
                     Js.String.includes(
                       String.lowercase_ascii(searchStr),
                       String.lowercase_ascii(str),
                     );

                   containsStr(countryEntry.label)
                   || containsStr(countryEntry.value);
                 })
            }
          | _ => []
          }
        },
        (countriesList, searchStr),
      );

    let filteredListLen = List.length(filteredList);

    // Hook to close the dropdown on outside click
    let elRef = Helpers.useClickOutside(_ => closeDropdown(), [||]);

    // Fetch the list of countries
    React.useEffect1(
      () => {
        setCountriesList(_ => RemoteData.Loading);

        Js.Promise.(
          requestEntries()
          |> then_(list => {
               setCountriesList(_ => RemoteData.Success(list));
               resolve();
             })
          |> catch(_ => {
               // @TODO: might want to use a Result type instead of an option here – or even better, something like the Elm RemoteData type

               setCountriesList(_ =>
                 RemoteData.Fail("Request or parsing failed")
               );
               resolve();
             })
        )
        |> ignore;

        None;
      },
      // Only run on initial mount
      [||],
    );

    // Attach keyboard event handlers
    React.useEffect3(
      () => {
        let keypressHandler: ReactEvent.Keyboard.t => unit =
          event => {
            let key = ReactEvent.Keyboard.key(event);

            switch (key) {
            | "Escape" =>
              closeDropdown();
              ReactEvent.Keyboard.preventDefault(event);

            | "ArrowUp" =>
              setFocusedInDropdownIndex(curr => curr - 1);
              ReactEvent.Keyboard.preventDefault(event);

            | "ArrowDown" =>
              setFocusedInDropdownIndex(curr => curr + 1);
              ReactEvent.Keyboard.preventDefault(event);

            | "Enter" =>
              let boundedFocusIndex_ =
                boundToListLenIndex(
                  // Accessing a ref here means we don't need to re-run this effect every time the focusedInDropdownIndex state changes – which is every time we hover over an option, so this makes quite a difference to performance
                  focusedIndexRef.current,
                  filteredListLen,
                );

              let focusedEntryOpt =
                switch (filteredListLen) {
                | 0 => None
                | _ => List.nth_opt(filteredList, boundedFocusIndex_)
                };

              switch (focusedEntryOpt) {
              | Some(focused) =>
                selectCountry(focused);
                ReactEvent.Keyboard.preventDefault(event);

              | None => ()
              };

            | _ => ()
            };
          };

        if (isOpen) {
          EventListener.add_keyboard_event_listener(
            "keydown",
            keypressHandler,
          );

          Some(
            () =>
              EventListener.remove_keyboard_event_listener(
                "keydown",
                keypressHandler,
              ),
          );
        } else {
          None;
        };
      },
      (filteredList, filteredListLen, isOpen),
    );

    // For easier comparisons
    let selectedValue =
      currentCountryEntry
      |> Option.map(entry => entry.value)
      |> Option.value(~default="");

    let boundedFocusIndex =
      boundToListLenIndex(focusedInDropdownIndex, filteredListLen);

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
            ++ (index == boundedFocusIndex ? " focused" : "")
          }
          onPointerEnter={_ => setFocusedInDropdownIndex(_ => index)}
          onClick={_ => selectCountry(thisCountryEntry)}>
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

    <div
      className={"country-select " ++ className}
      ref={ReactDOM.Ref.domRef(elRef)}>
      <button
        className="button"
        onClick={_ => setIsOpen(open_ => !open_)}
        style={ReactDOM.Style.make(
          ~minWidth=?
            Option.is_none(currentCountryEntry) ? Some("150px") : None,
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
               {switch (countriesList) {
                | RemoteData.NotAsked
                | RemoteData.Loading =>
                  <div className="loading-panel">
                    <p> {React.string("Loading countries list...")} </p>
                  </div>
                | RemoteData.Success(_) =>
                  <>
                    <div className="search-field">
                      <div className="search-icon">
                        <i className="fa-solid fa-magnifying-glass" />
                      </div>
                      <input
                        autoFocus=true
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
                      scrollToIndex=boundedFocusIndex
                      overscanRowCount=25
                    />
                  </>
                | RemoteData.Fail(err) =>
                  <div className="error-panel">
                    <p> {React.string(err)} </p>
                  </div>
                }}
             </div>
           : React.null}
      </div>
    </div>;
  };
};
