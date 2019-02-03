<?php

require 'functions.php';
showHeader('Prochaines versions');

?>

			<h2>Dans les prochaines versions</h2>
			<p>
				<b><u>Attention</u> :</b> ces fonctionnalités sont susceptibles de changer selon l'évolution des discussions les concernant
			</p>

			<table class="nextFeatures_table">
				<?php
					echo feature('Gestion des séries', false);
					echo feature('Web service hebergé par FilmsKollection pour le remplissage des infos', false);
					echo feature('Import/Export d\'une sélection de films vers une autre collection (pour partager des fiches)', false);
					echo feature('Gestion des DVD/Blue-Ray en tant que dossiers rippés', false);
					echo feature('Lecture des codecs des films pour les afficher (info sur la qualité du film)', false);
				?>
			</table>

<?php

showFooter();

?>
