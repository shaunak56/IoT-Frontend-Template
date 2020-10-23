import React from 'react';

const Device = (props) => {
	const { name, type } = props;
	return (
		<>
			<h4>Device Info</h4>
			<div>
				<p>Device Name: {name}</p>
				<br />
				<p>Device Type: {type}</p>
				<br />
			</div>
		</>
	);
};

export default Device;
