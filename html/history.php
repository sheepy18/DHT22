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
<table>
         <td> 
                <tr> <a href="index.php"> Home </a> | </tr>
                <tr> <a href="queue.php"> Übersicht</a> |  </tr>
                <tr> <a href="history.php"> Historie </a> </tr>
                </td>
        </table>


<canvas id="TempChart"></canvas>
<canvas id="HumiChart"></canvas>
<canvas id="IlluChart"></canvas>
<div id="Test"></div>

<form action="/history.php" method="get">
  Start date(yyyy-MM-dd HH:mm:ss):<br>
  <input type="text" name="start" value="">
  <br>
  <br>
  End date(yyyy-MM-dd HH:mm:ss):<br>
  <input type="text" name="end" value="">
  <br><br>
  <input type="submit" value="submit">
</form> 
</html>
<script>
const tempCtx = document.getElementById('TempChart').getContext('2d');
const humiCtx = document.getElementById('HumiChart').getContext('2d');
const illuCtx = document.getElementById('IlluChart').getContext('2d');
const tempChart = new Chart(tempCtx, {
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
                data: [/*5,20,8,-7,0,5*/]
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
        let url = 'weatherdata.php';
        <?php
                $startDate= "";
                $startTime= "";
                $endDate= "";
                $endTime= "";


                if( isset($_GET['start']) && isset($_GET['end']))
                {
                        $startDate = substr( filter_input(INPUT_GET,'start',FILTER_SANITIZE_STRING), 0, 10 );
                        $startTime = substr( filter_input(INPUT_GET,'start',FILTER_SANITIZE_STRING), 11, 8 );
               
                        $endDate = substr( filter_input(INPUT_GET,'end',FILTER_SANITIZE_STRING), 0, 10 );
                        $endTime = substr( filter_input(INPUT_GET,'end',FILTER_SANITIZE_STRING), 11, 8 );



                        
                } else {
                        $startDate = '1971-01-01';
                        $startTime = '00:00:00';
               
                        $endDate = '1971-01-01';
                        $endTime = '00:00:00';
                       
                }
                echo "url='weatherdata.php?startDate=".$startDate."&startTime=".$startTime."&endDate=".$endDate."&endTime=".$endTime."';";
        ?>

        const http = new XMLHttpRequest();
       
        http.open("GET", url);
        http.responseType = "text";
        http.send();

        http.onreadystatechange = function() {
                if(this.readyState == 4 && this.status == 200) {
                        const weatherDataArray = JSON.parse(http.responseText);
                        console.log(weatherDataArray);
                        tempChart.data.labels.length = 0;
                        tempChart.data.datasets[0].data.length = 0;

                        humiChart.data.labels.length = 0;
                        humiChart.data.datasets[0].data.length = 0;

                        illuChart.data.labels.length = 0;
                        illuChart.data.datasets[0].data.length = 0;

                        weatherDataArray.forEach(function(row) {
                                tempChart.data.labels.push(row.timestamp);
                                humiChart.data.labels.push(row.timestamp);
                                illuChart.data.labels.push(row.timestamp);

                                tempChart.data.datasets[0].data.push(row.temperatur);
                                humiChart.data.datasets[0].data.push(row.humidity);
                                illuChart.data.datasets[0].data.push(row.illumination);

                                tempChart.update(0);
                                humiChart.update(0);
                                illuChart.update(0);
                        });
                }
        }

}

</script>
