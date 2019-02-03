<?php

require 'functions.php';
showHeader('Home');

?>

			<p class="index_p">
				FilmsKollection is a free and open-source program that permit you to create a collection to manage your films. You can add films,
				fill their sheets automatically via internet, and then enjoy your collection.
			</p>

			<div id="clear">
				<img src="../design/images/add.png" alt="[Img]Add films to the collection" class="index_img" /><h3>Add films to the collection</h3>
				<ul class="index_ulFeatures">
					<li>Recursive scan of dirs</li>
					<li>Import films list</li>
					<li>Add film manually by writing its name</li>
				</ul>

				<img src="../design/images/update.png" alt="[Img]Update sheets via internet" class="index_img" /><h3>Update sheets via internet</h3>
				<ul class="index_ulFeatures">
					<li>Automatic search of informations</li>
					<li>Various possible propositions</li>
					<li>Complte modification of a film</li>
				</ul>

				<img src="../design/images/benefit.png" alt="[Img]Enjoy your collection" class="index_img" /><h3>Enjoy your collection</h3>
				<ul class="index_ulFeatures">
					<li>Sheet of films: jackets, informations (gender, duration, year, actors...)</li>
					<li>Launch film / trailer</li>
					<li>Manage seen / not seen films</li>
					<li>Search with automatic completion</li>
				</ul>
			</div>

			<h2>FilmsKollection, it's also:</h2>
			<ul class="index_ulFeatures">
				<li>A preview of next features: <a href="nextFeatures.php">Next features</a></li>
				<li>A disponibility of the software on all architectures: Windows, Linux, Mac OS X</li>
				<li>A portable application: no need to install, and you can bring it anywhere on usb key</li>
				<li>An access to program sources (under GPL v3 license): <a href="download.php">Download</a></li>
			</ul>

<?php

showFooter();

?>
