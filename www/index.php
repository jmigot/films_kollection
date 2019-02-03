<?php
	$language = explode(',', $_SERVER['HTTP_ACCEPT_LANGUAGE']);
	$language = strtolower(substr(rtrim($language[0]), 0, 2));
	
	if ($language == 'fr')
		header('Location: fr/');
	
	else
		header('Location: en/');
?>