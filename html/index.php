<meta charset='utf-8'>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.4.0/Chart.min.js"></script>
<script>

console.log( <?php echo "'Hello'"; ?> );

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
        <title>Weather Information</title>
</head>
<canvas id="TempChart"></canvas>
<div id="Test"></div>
</html>
<script>
const divTest = document.getElementById('Test');
const tempCtx = document.getElementById('TempChart').getContext('2d');
const chart = new Chart(tempCtx, {
        // The type of chart we want to create
        type: 'line',

        // The data for our dataset
        data: {
        labels: [],
        datasets: [
                {
                        label: "Temperatur",
                        backgroundColor: 'rgb(255, 0, 0)',
                        borderColor: 'rgb(255, 0, 0)',
                        fill: false,
                        data: [/*5,20,8,-7,0,5*/]
                },
                {
                        label: "Humidity",
                        backgroundColor: 'rgb(0, 0, 255)',
                        borderColor: 'rgb(0, 0, 255)',
                        fill: false,
                        data: [/*5,20,8,-7,0,5*/]
                },
                {
                        label: "Illumination",
                        backgroundColor: 'rgb(0, 255, 0)',
                        borderColor: 'rgb(0, 255, 0)',
                        fill: false,
                        data: [/*5,20,8,-7,0,5*/]
                }
                ]
        },
        // Configuration options go here
        options: {}
});
requestWeatherData();
setInterval( function() { requestWeatherData(); }, 30000);


function pushData(chart, numDataset, value) {
        chart.data.datasets[numDataset].data.push(temp);
        chart.update();
}

function shiftData(chart, numDataset) {
        chart.data.datasets[numDataset].data.shift();
        chart.update();
}

function pushLabel(chart, value) {
        chart.data.labels.push(value);
        chart.update();
}

function shiftLabel(chart) {
        chart.data.labels.shift();
        chart.update();
}

function requestWeatherData() {
        const http = new XMLHttpRequest();
        const url = 'weatherdata.php';
        http.open("GET", url);
        http.responseType = "text";
        http.send();

        http.onreadystatechange = function() {
                if(this.readyState == 4 && this.status == 200) {
                        const weatherDataArray = JSON.parse(http.responseText);
                        //console.log(weatherDataArray);
                        chart.data.labels.length = 0;
                        chart.data.datasets[0].data.length = 0;
                        chart.data.datasets[1].data.length = 0;
                        weatherDataArray.forEach(function(row) {
                                //console.log("Time: " + row.timestamp + "\n");
                                //console.log(chart.data);
                                //console.log(chart.data.labels);
                                //pushLabel(row.timestamp);
                                chart.data.labels.push(row.timestamp);
                                //pushData(chart, 0, row.temperatur);
                                chart.data.datasets[0].data.push(row.temperatur);
                                //pushData(chart, 1, row.humidity);
                                chart.data.datasets[1].data.push(row.humidity);
                                chart.data.datasets[2].data.push(row.illumination);
                                chart.update(0);
                        });
                }
        }

}

</script>
