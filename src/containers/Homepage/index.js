import React from "react";
import Device from "../../components/Device";
import Header from "../../components/Header";
import { Container, Col } from "react-bootstrap";

const devices = [
    {
        name: "iPhone",
        type: "Mobile",
        status: "Active",
    },
    {
        name: "LG",
        type: "fridge",
        status: "Active",
    },
    {
        name: "Samsung",
        type: "Mobile",
        status: "Inactive",
    },
    {
        name: "Sony",
        type: "TV",
        status: "Inactive",
    },
];

const listOfDevices = () => {
    return;
};

const Homepage = () => {
    return (
        <Container>
            <br />
            <h1
                style={{
                    borderRadius: "8px",
                    padding: "5px",
                    backgroundColor: "#ff7171",
                }}
            >
                Home Settings
            </h1>
            <h3
                style={{
                    borderRadius: "8px",
                    padding: "5px",
                    backgroundColor: "#ffaa71",
                }}
            >
                List of connected Devices:{" "}
            </h3>
            <Col style={{ borderRadius: "8px", backgroundColor: "#968c83" }}>
                {devices.map((device) => (
                    <div>
                        <Device props={device} />
                    </div>
                ))}
            </Col>
        </Container>
    );
};

export default Homepage;
