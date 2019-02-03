<?php

require 'functions.php';
showHeader('Accueil');

?>

			<p class="index_p">
				FilmsKollection est un logiciel libre et gratuit qui vous permet de cr�er une collection pour g�rer vos films. Vous pouvez ajouter des films,
				remplir leurs fiches automatiquement par internet et profiter ensuite de votre collection.
			</p>

			<div id="clear">
				<img src="../design/images/add.png" alt="[Img]Ajouter vos films � la collection" class="index_img" /><h3>Ajoutez vos films � la collection</h3>
				<ul class="index_ulFeatures">
					<li>Scan de dossier r�cursif</li>
					<li>Importation de liste de films</li>
					<li>Ajout de film � la main en �crivant son nom</li>
				</ul>

				<img src="../design/images/update.png" alt="[Img]Mettez les fiches � jour avec internet" class="index_img" /><h3>Mettez les fiches � jour avec internet</h3>
				<ul class="index_ulFeatures">
					<li>Recherche automatique des informations</li>
					<li>Plusieurs propositions possibles</li>
					<li>Modification compl�te de la fiche d'un film</li>
				</ul>

				<img src="../design/images/benefit.png" alt="[Img]Profiter de votre collection" class="index_img" /><h3>Profitez de votre collection</h3>
				<ul class="index_ulFeatures">
					<li>Fiche des films : jaquettes, informations (genre, dur�e, ann�e, acteurs...)</li>
					<li>Lancement du film / de la bande-annonce</li>
					<li>Gestion des films vus / non-vus</li>
					<li>Recherche avec compl�tion automatique</li>
				</ul>
			</div>

			<h2>FilmsKollection, c'est aussi :</h2>
			<ul class="index_ulFeatures">
				<li>Un aper�u des prochaines fonctionnalit�s : <a href="nextFeatures.php">Prochainement</a></li>
				<li>Une disponibilit� du logiciel sur toutes les plateformes : Windows, Linux, Mac OS X</li>
				<li>Une application portable : aucune installation requise, et vous pouvez l'emmener sur cl� usb</li>
				<li>Un acc�s aux sources du logiciel (sous licence GPL v3) : <a href="download.php">T�l�chargement</a></li>
			</ul>

<?php

showFooter();

?>
