import React from "react";
import ReactDOM from "react-dom";

const App: React.FC<{}> = (): React.ReactElement => {
  return <p>Hello</p>;
};

ReactDOM.render(<App />, document.getElementById("root") as HTMLElement);
