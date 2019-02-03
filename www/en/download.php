<?php

require 'functions.php';
showHeader('Download');

?>

			<h2>FilmsKollection</h2>

<?php

$file    = fopen('../downloads/version.txt', 'r');
$version = trim(fgets($file));
fclose($file);

echo '<h3>Latest version : ' . trim($version) . '</h3>';

?>

			<table class="download_table">
				<tr>
					<td><img src="../design/images/Windows.png" alt="[Img]Windows" width="50" /></td>
					<td><b>Windows</b></td>
					<td><a href="../downloads/<?php echo $version; ?>/FilmsKollection_<?php echo $version; ?>.zip">Download</a></td>
				</tr>

				<tr>
					<td><img src="../design/images/Linux.png" alt="[Img]Linux" width="50" /></td>
					<td><b>Linux</b></td>
					<td><a href="../downloads/<?php echo $version; ?>/FilmsKollection_<?php echo $version; ?>.tar.gz">Download</a></td>
				</tr>

				<tr>
					<td><img src="../design/images/Mac.jpg" alt="[Img]Mac" width="50" /></td>
					<td><b>Mac</b></td>
					<td>Not available</td>
				</tr>

				<tr>
					<td><img src="../design/images/sources.jpg" alt="[Image] Sources" width="50" /></td>
					<td><b>Sources</b></td>
					<td><a href="../downloads/<?php echo $version; ?>/FilmsKollection_<?php echo $version; ?>_src.zip">Download</a></td>
				</tr>
			</table>

<?php

showFooter();

?>
