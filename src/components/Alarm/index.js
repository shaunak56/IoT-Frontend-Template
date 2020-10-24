import React, { Component } from "react";
import Switch from "react-switch";
import { Container } from "react-bootstrap";

const alarms = [
    {
        name: "Fire Alert",
    },
];

class Alarm extends Component {
    constructor() {
        super();
        this.state = { checked: false };
        this.handleChange = this.handleChange.bind(this);
    }

    handleChange(checked) {
        this.setState({ checked });
    }

    render() {
        return (
            <>
                <Container style={{ backgroundColor: "#f7dad9" }}>
                    <hr />
                    {alarms.map((alarm) => (
                        <div style={{ paddingBottom: "5px" }}>
                            <h4>
                                {alarm.name} &nbsp;
                                <span style={{ marginTop: "-5px" }}>
                                    <Switch
                                        onChange={this.handleChange}
                                        checked={this.state.checked}
                                    />
                                </span>
                            </h4>
                        </div>
                    ))}
                </Container>
            </>
        );
    }
}

export default Alarm;
