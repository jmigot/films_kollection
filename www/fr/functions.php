<?php

// Protection
if ($_SERVER['SCRIPT_FILENAME'] == __FILE__)
{
	header('Location: /');
	exit;
}

ob_start();
header('Content-type: text/html; charset=iso-8859-1');

function feature($text, $done)
{
	if ($done)
		$img = '<img src="../design/images/done.png" alt="[Img]Fait" />';
		
	else
		$img = '<img src="../design/images/notDone.png" alt="[Img]Pas encore fait" />';
		
	return '<tr>
				<td>' . $img . '</td>
				<td>' . $text . '</td>
			</tr>';
}

function showHeader($title)
{
	echo <<<EOF
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr" lang="fr">
	<head>
		<title>FilmsKollection - $title</title>
		<meta http-equiv="content-type" content="text/html; charset=iso8859-1" />
		<link href="../design/style.css" rel="stylesheet" media="screen" type="text/css" />
		<link rel="icon" type="image/png" href="../design/images/icon.png" />
	</head>

	<body>
		<!-- Logo -->
		<div id="logo">
			<table>
				<tr>
					<td><img src="../design/images/logo.png" alt="Icone FilmsKollection" /></td>
					<td><i><b>Gestionnaire de films</b></i></td>
				</tr>
			</table>
		</div>

		<!-- Menu -->
		<div id="menu">
			<ul>
				<li><a href="index.php">Accueil</a></li>
				<li><a href="download.php">Téléchargement</a></li>
				<li><a href="screenshots.php">Screenshots</a></li>
				<li><a href="nextFeatures.php">Prochainement</a></li>
				<li><a href="about.php">A propos</a></li>
			</ul>
		</div>
		<!-- End of menu -->

		<!-- Content -->
		<div id="content">
EOF;
}

function showFooter()
{
	echo <<<EOF
		</div>
		<!-- End of content -->

		<div id="footer">
		</div>
		<!-- End of footer -->
	</body>
</html>
EOF;

	ob_end_flush();
}

?>