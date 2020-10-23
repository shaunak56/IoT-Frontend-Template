import React from "react";
import ReactDOM from "react-dom";
import "bootstrap/dist/css/bootstrap.min.css";
import Header from "./components/Header";
import Homepage from "./containers/Homepage";
import Safemode from "./containers/Safemode";

ReactDOM.render(
    <React.StrictMode>
        <Header />
        <Homepage />
        <br />
        <Safemode />
    </React.StrictMode>,
    document.getElementById("root")
);
