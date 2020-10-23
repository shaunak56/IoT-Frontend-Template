import React from "react";
import { Container } from "react-bootstrap";

const Device = (props) => {
    const { name, type, status } = props.props;
    return (
        <Container
            style={{
                paddingTop: "20px",
            }}
        >
            <h4
                style={{
                    backgroundColor: "#d6d2c4",
                    padding: "4px",
                    borderRadius: "4px",
                }}
            >
                Device Info
            </h4>
            <div>
                <p
                    style={{
                        backgroundColor: "#fff5ea",
                        padding: "4px",
                        borderRadius: "4px",
                    }}
                >
                    Device Name: {name}
                    <br />
                    Device Type: {type}
                    <br />
                    {status === "Active" ? (
                        <span style={{ color: "#28df99" }}>
                            Device Stauts: {status}
                        </span>
                    ) : (
                        <span style={{ color: "#ff414d" }}>
                            Device Stauts: {status}
                        </span>
                    )}
                </p>
                <br />
            </div>
        </Container>
    );
};

export default Device;
