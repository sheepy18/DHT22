<?php
        $dbs_path = $_SERVER['DOCUMENT_ROOT'] . "/../../dbs/sensordata.db";
        $db = new SQLite3($dbs_path,SQLITE3_OPEN_READWRITE);

        if($db)
        {
                try {
                        $sql = "SELECT timestamp, temperatur, humidity, illumination FROM data";
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
