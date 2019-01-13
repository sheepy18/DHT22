<?php
	$dbs_path = $_SERVER['DOCUMENT_ROOT'] . "/../../dbs/weather.sqlite";
	if(isset($_POST['lastIndex']))
	{
		if(is_numeric($_POST['lastIndex']))
		{
			$db = new SQLite3($dbs_path,SQLITE3_OPEN_READWRITE);
			if($db)
			{
				$lastIndex = (int)$_POST['lastIndex'];
				//$lastIndex = 0;
				$sql;
				$rs;

				if($lastIndex <= 0)
				{
					//$sql = "SELECT id, timestamp, temperatur, humidity, illumination " . "FROM (SELECT id, timestamp, temperatur, humidity, illumination FROM data ORDER BY id DESC LIMIT 12) " . "ORDER BY id ASC LIMIT 12";
					$sql = "SELECT * FROM (SELECT * FROM data ORDER BY id DESC LIMIT 12) ORDER BY id ASC";
					$rs = $db->query($sql);
				}
				else
				{
					$sql =	"SELECT id, timestamp, temperatur, humidity, illumination " . "FROM data " . "WHERE id > :id " . "ORDER BY id ASC";
					$statement = $db->prepare($sql);
					$statement->bindValue(':id', $lastIndex, SQLITE3_INTEGER);
					$rs = $statement->execute();
				}
				if($rs)
				{
					try {
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
				$db->close();

			}
		}
		else
		{
			echo "is not a number!";
		}
	}
	else
	{
		echo "GET =";
		var_dump($_GET);
		echo "POST =";
		var_dump($_POST);
	}
?>
