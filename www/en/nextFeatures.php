<?php

require 'functions.php';
showHeader('Next features');

?>

			<h2>In the next versions</h2>
			<p>
				<b><u>Warning</u> :</b> these functionnalities can change depending on discussions conceirning them
			</p>

			<table class="nextFeatures_table">
				<?php
					echo feature('Management of series', false);
					echo feature('Web service hosted by FilmsKollection for informations filling', false);
					echo feature('Import/Export of a selection of films to another collection (to share sheets)', false);
					echo feature('Management of DVD/Blue-Ray as ripped dirs', false);
					echo feature('Reading of film\'s codecs to show them (info on quality of film)', false);
				?>
			</table>

<?php

showFooter();

?>
