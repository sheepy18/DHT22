<meta charset='utf-8'>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
	/*.floating-box {
		display: inline-block;
		width: 600px;
		height: 300px;
		margin: 5px;
		border: 2px solid #000000;
	}*/
	/*.flex-container {
		display: flex;
		flex-wrap: wrap;
	}*/
</style>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.4.0/Chart.min.js"></script>
<script>
if(window.Chart) {
	console.log("Remote chart lib loaded successfully.");
} else {
	console.log("Remote chart lib not available!");
	console.log("Switching to local version.");
	document.write('<script src="js/libs/Chart.min.js"><\/script>');
}
</script>
<html>
<head>
	<title>Weather Information
</title>
</head>
<body>
	<!--<div class="floating-box">-->
	<table>
                <td> 
                        <tr> <a href="index.php"> Home </a> | </tr>
                        <tr> <a href="queue.php"> Übersicht</a> |  </tr>
                        <tr> <a href="history.php"> Historie </a></tr>
                </td>
        </table>
	<div style="width: 99%; height: 97%">
		<div>
			<canvas id="TempChart"></canvas>
		</div>
		<div>
			<canvas id="HumiChart"></canvas>
		</div>
		<div>
			<canvas id="IlluChart"></canvas>
		</div>
	</div>
<body>
</html>
<script>
const tempCtx = document.getElementById('TempChart').getContext('2d');
const humiCtx = document.getElementById('HumiChart').getContext('2d');
const illuCtx = document.getElementById('IlluChart').getContext('2d');
const idQueue = [];

const tempChart = new Chart(tempCtx, {
	// The type of chart we want to create
	type: 'line',
	// The data for our dataset
	data: {
	labels: [],
	datasets: [
		{
			label: "Temperature",
			backgroundColor: 'rgb(255, 0, 0)',
			borderColor: 'rgb(255, 0, 0)',
			fill: false,
			data: []
		}
		]
	},
	// Configuration options go here
	options: {}
});

const humiChart = new Chart(humiCtx, {
	// The type of chart we want to create
	type: 'line',
	// The data for our dataset
	data: {
	labels: [],
	datasets: [
		{
			label: "Humidity",
			backgroundColor: 'rgb(0, 0, 255)',
			borderColor: 'rgb(0, 0, 255)',
			fill: false,
            data: []
		}
		]
	},
	// Configuration options go here
	options: {}
});

const illuChart = new Chart(illuCtx, {
	// The type of chart we want to create
	type: 'line',
	// The data for our dataset
	data: {
	labels: [],
	datasets: [
		{
			label: "Illumination",
			backgroundColor: 'rgb(0, 255, 0)',
			borderColor: 'rgb(0, 255, 0)',
			fill: false,
			data: []
        }
		]
	},
	// Configuration options go here
	options: {}
});

const charts = [tempChart, humiChart, illuChart];
//console.log(charts); 

function updateCharts() {
	for (let i = 0; i < charts.length; i++) {
		charts[i].update(0);
	}
}

function clearCharts() {
	for (let i = 0; i < charts.length; i++) {
		const chart = charts[i];
		while(chart.data.labels.length > 0) {
			chart.data.labels.pop();
		}
		while(chart.data.datasets[0].data.length > 0) {
			chart.data.datasets[0].data.pop();
		}
	}
}
function pushTimestamp(timestamp) {
	for (let i = 0; i < charts.length; i++) {
		charts[i].data.labels.push(timestamp);
	}
}
function shiftTimestamp() {
	for (let i = 0; i < charts.length; i++) {
		charts[i].data.labels.shift();
	}
}

requestWeatherDataInitial();
setInterval( function() { requestWeatherData(); }, 30000);

function requestWeatherDataInitial() {
	const http = new XMLHttpRequest();
	const url = 'recently.php';
	const params = "lastIndex=0";
	http.responseType = "text";
	http.open("POST", url, true);
	http.setRequestHeader("Content-type","application/x-www-form-urlencoded");

	http.onreadystatechange = function() {
		if(this.readyState == 4 && this.status == 200) {
			const weatherDataArray = JSON.parse(http.responseText);

			clearCharts();
			weatherDataArray.forEach(function(row) {
				idQueue.push(row.id);
				pushTimestamp(row.timestamp);
				tempChart.data.datasets[0].data.push(row.temperatur);
				humiChart.data.datasets[0].data.push(row.humidity);
				illuChart.data.datasets[0].data.push(row.illumination);
			});
			updateCharts();
		}
	}
	//console.log(params);
	http.send(params);
}
function requestWeatherData() {
	const http = new XMLHttpRequest();
	const url = 'recently.php';
	let params = "lastIndex=" + idQueue[idQueue.length-1];
	http.responseType = "text";
	http.open("POST", url, true);
	http.setRequestHeader("Content-type","application/x-www-form-urlencoded");

	http.onreadystatechange = function() {
		if(this.readyState == 4 && this.status == 200) {
			const weatherDataArray = JSON.parse(http.responseText);

			weatherDataArray.forEach(function(row) {
				idQueue.push(row.id);
				idQueue.shift();
				pushTimestamp(row.timestamp);
				shiftTimestamp();
				tempChart.data.datasets[0].data.push(row.temperatur);
				tempChart.data.datasets[0].data.shift();
				humiChart.data.datasets[0].data.push(row.humidity);
				humiChart.data.datasets[0].data.shift();
				illuChart.data.datasets[0].data.push(row.illumination);
				illuChart.data.datasets[0].data.shift();
			});
			updateCharts();
		}
	}
	//console.log(params);
	http.send(params);
}
</script>
