<?php
	include("atire_remote.php");

	$instance = new ATIRE_API_remote();
	$instance->open("localhost:8088");

	$got = $instance->search("Andrew", 1, 10);
	echo $got;
	echo "\n";

	$instance->close();
?>
