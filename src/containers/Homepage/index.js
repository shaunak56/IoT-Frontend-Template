import React from 'react';
import Device from '../../components/Device';
import Header from '../../components/Header';

const devices = [
	{
		name: 'iPhone',
		type: 'Mobile',
	},
	{
		name: 'Samsung',
		type: 'Mobile',
	},
	{
		name: 'LG',
		type: 'fridge',
	},
	{
		name: 'Sony',
		type: 'TV',
	},
];

const Homepage = () => {
	return (
		<div>
			<Header />
			Hello World
			<br />
			<div class="container">
				{devices.map((device) => {
					<Device props={device} />;
				})}
			</div>
		</div>
	);
};

export default Homepage;
