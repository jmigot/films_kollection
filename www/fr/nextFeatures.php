<?php

require 'functions.php';
showHeader('Prochaines versions');

?>

			<h2>Dans les prochaines versions</h2>
			<p>
				<b><u>Attention</u> :</b> ces fonctionnalit�s sont susceptibles de changer selon l'�volution des discussions les concernant
			</p>

			<table class="nextFeatures_table">
				<?php
					echo feature('Gestion des s�ries', false);
					echo feature('Web service heberg� par FilmsKollection pour le remplissage des infos', false);
					echo feature('Import/Export d\'une s�lection de films vers une autre collection (pour partager des fiches)', false);
					echo feature('Gestion des DVD/Blue-Ray en tant que dossiers ripp�s', false);
					echo feature('Lecture des codecs des films pour les afficher (info sur la qualit� du film)', false);
				?>
			</table>

<?php

showFooter();

?>
