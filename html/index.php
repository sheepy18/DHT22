 <html>
         <head> 
                <meta charset='utf-8'>
                <meta name="viewport" content="width=device-width, initial-scale=1.0">   
         </head>      
<body>
        <table>
                <td> 
                        <tr> <a href="index.php"> Home </a> | </tr>
                        <tr> <a href="queue.php"> Übersicht</a> |  </tr>
                        <tr> <a href="history.php"> Historie </a></tr>
                </td>
        </table>
        <h1> Aktuell: </h1>
        <script>         
                    
                const http = new XMLHttpRequest();
                const url = 'weatherdata.php?lastData=true';
                http.open("GET", url);
                http.responseType = "text";
                http.send();
                                    
                http.onreadystatechange = function() {
                if(this.readyState == 4 && this.status == 200) {
                        const weatherDataArray = JSON.parse(http.responseText);
                        weatherDataArray.forEach(function(row) {
                                var tempDiv = document.createElement("div"); 

                                tempDiv.innerHTML = "<h2>"+ row.temperatur + "&deg;C</h2>";
                                tempDiv.innerHTML += "<h2> " + row.humidity + " %</h2>";
                                tempDiv.innerHTML += "<h2> " + row.illumination + " lx</h2>";

                                document.body.appendChild(tempDiv);
                                });
                        }
                }
</script>                                           
</body>      
        
</html>

