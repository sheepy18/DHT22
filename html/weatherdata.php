<?php
        $dbs_path = $_SERVER['DOCUMENT_ROOT'] . "/../../dbs/weather.sqlite";
        $db = new SQLite3($dbs_path,SQLITE3_OPEN_READWRITE);

        $param = 'all';
        $start = '';
        $end = '';

        if(isset($_GET['startDate']) && isset($_GET['startTime']) && isset($_GET['endDate']) && isset($_GET['endTime'])  ){
                 $param = 'notAll';
                 $start = filter_input(INPUT_GET,'startDate',FILTER_SANITIZE_STRING)." ";
                 $start = $start.filter_input(INPUT_GET,'startTime',FILTER_SANITIZE_STRING);

                 $end = filter_input(INPUT_GET,'endDate',FILTER_SANITIZE_STRING)." ";
                 $end = $end.filter_input(INPUT_GET,'endTime',FILTER_SANITIZE_STRING);
        } else if( isset($_GET['lastData']) ) {
                $param = "lastData";
        }

        if($db)
        {
                try {
                        if( $param == 'all'){
                                $sql = "SELECT timestamp, temperatur, humidity, illumination FROM data";
                                
                        }  else if($param == "lastData")  {
                                $sql = "SELECT timestamp, temperatur, humidity, illumination FROM data WHERE id = ( SELECT max(id) FROM data )";
                        }                          
                        else{
                                $sql = 'SELECT timestamp, temperatur, humidity, illumination FROM data where timestamp >='."'".$start."'".' AND timestamp <='."'".$end."'";
                                
                        }
                        $rs = $db->query($sql);
                        $phpArray = array();
                        while($row = $rs->fetchArray(SQLITE3_ASSOC))
                        {
                                array_push($phpArray, $row);
                        }
                        $jsArray = json_encode($phpArray);
                        echo $jsArray;
                }
                catch(Exception $e)
                {
                        echo "Caught Exception: " . $e->getMessage() . "\n";
                }
        }
?>
