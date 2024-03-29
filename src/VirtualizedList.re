type rowRendererProps = {
  key: string,
  index: int,
  isScrolling: bool,
  isVisible: bool,
  style: ReactDOM.style,
};

type rowRenderer = rowRendererProps => React.element;

[@mel.module "react-virtualized"] [@react.component]
external make:
  (
    ~rowCount: int,
    ~rowHeight: int,
    ~rowRenderer: rowRenderer,
    ~width: int,
    ~height: int,
    ~scrollToIndex: int=?,
    ~overscanRowCount: int=?
  ) =>
  React.element =
  "List";
