<?php


class MicroController
{
	private $address;
	
	private $port;
	
	public function __construct($address, $port)
	{
		$this->address = $address;
		$this->port = $port;
	}
	
	public function runCommand($command)
	{				
		$msg = $command . "\n";
		
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		
		if( !$socket ) {
			throw new Exception("Problem z utworzeniem socketa");
		}
		
		$isConnected = socket_connect($socket, $this->address, $this->port);
		
		if( $isConnected === false ) {
			throw new Exception("Nie udało się nawiązać połączenia");
		}
		
		
		$bytes = socket_write($socket, $msg, strlen($msg));
		
		if( $bytes != strlen($msg) ) {
			throw new Exception("Nie udało się zapisać wszystkich danych");
		}
		
		usleep(2000);
		
		$read = socket_read($socket, 4096, PHP_NORMAL_READ);
		
		return json_decode($read, true);
	}
}


$controller = new MicroController('192.168.0.66', 80);

switch(true)
{
	case array_key_exists('power', $_GET):
		$controller = new MicroController('192.168.0.66', 80);
		$controller->runCommand('power');
	break;
}

$status = $controller->runCommand('status');
$status = $status['data'];
?>


<body>
	<div style="border-right: 1px #2d6898 solid; border-bottom: 1px #2d6898 solid; height: 72px; width: 72px;text-align: center;vertical-align: middle;">
		<a href="/SmartHome/?power" style="font-size: 48px; display: block;color: <?= $status? "green": "red" ?>">&#9883;</a>
	</div>
<?php
	if( file_exists('./data.log') ) {
		$time = file_get_contents('./data.log');
		echo 'Ostatnia aktualizacja statusu kontrollera: ' . date('Y-m-d H:i:s', $time);
	}
?>
</body>