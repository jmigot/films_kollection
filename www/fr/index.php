<?php

require 'functions.php';
showHeader('Accueil');

?>

			<p class="index_p">
				FilmsKollection est un logiciel libre et gratuit qui vous permet de créer une collection pour gérer vos films. Vous pouvez ajouter des films,
				remplir leurs fiches automatiquement par internet et profiter ensuite de votre collection.
			</p>

			<div id="clear">
				<img src="../design/images/add.png" alt="[Img]Ajouter vos films à la collection" class="index_img" /><h3>Ajoutez vos films à la collection</h3>
				<ul class="index_ulFeatures">
					<li>Scan de dossier récursif</li>
					<li>Importation de liste de films</li>
					<li>Ajout de film à la main en écrivant son nom</li>
				</ul>

				<img src="../design/images/update.png" alt="[Img]Mettez les fiches à jour avec internet" class="index_img" /><h3>Mettez les fiches à jour avec internet</h3>
				<ul class="index_ulFeatures">
					<li>Recherche automatique des informations</li>
					<li>Plusieurs propositions possibles</li>
					<li>Modification complète de la fiche d'un film</li>
				</ul>

				<img src="../design/images/benefit.png" alt="[Img]Profiter de votre collection" class="index_img" /><h3>Profitez de votre collection</h3>
				<ul class="index_ulFeatures">
					<li>Fiche des films : jaquettes, informations (genre, durée, année, acteurs...)</li>
					<li>Lancement du film / de la bande-annonce</li>
					<li>Gestion des films vus / non-vus</li>
					<li>Recherche avec complétion automatique</li>
				</ul>
			</div>

			<h2>FilmsKollection, c'est aussi :</h2>
			<ul class="index_ulFeatures">
				<li>Un aperçu des prochaines fonctionnalités : <a href="nextFeatures.php">Prochainement</a></li>
				<li>Une disponibilité du logiciel sur toutes les plateformes : Windows, Linux, Mac OS X</li>
				<li>Une application portable : aucune installation requise, et vous pouvez l'emmener sur clé usb</li>
				<li>Un accès aux sources du logiciel (sous licence GPL v3) : <a href="download.php">Téléchargement</a></li>
			</ul>

<?php

showFooter();

?>
