import React from "react";
import Alarm from "../../components/Alarm";
import { Container, Col } from "react-bootstrap";

const Safemode = () => {
    return (
        <Container>
            <h1
                style={{
                    borderRadius: "8px",
                    padding: "5px",
                    backgroundColor: "#c56183",
                }}
            >
                Safe Mode Settings
            </h1>
            <h3
                style={{
                    borderRadius: "8px",
                    padding: "5px",
                    backgroundColor: "#ffa5a5",
                }}
            >
                List of Protected Alarms:{" "}
            </h3>

            <Col
                style={{
                    borderRadius: "8px",
                    backgroundColor: "#968c83",
                    padding: "20px",
                }}
            >
                <Alarm />
            </Col>
        </Container>
    );
};

export default Safemode;
