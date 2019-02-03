
/**
 * QMainWindow
 *
 * @date April 2011
 * @author Jean-Luc Migot (jean-luc.migot@filmskollection.org)
 *
 *
 * Copyright (C) 2011 Jean-Luc Migot
 *
 * This file is part of FilmsKollection.
 *
 * FilmsKollection is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FilmsKollection is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "About.h"
#include "AllChoices.h"
#include "Config.h"
#include "Database.h"
#include "Dialog.h"
#include "Doubloons.h"
#include "Film.h"
#include "FilmModify.h"
#include "FilmView.h"
#include "Help.h"
#include "Network.h"
#include "Parser.h"
#include "Preferences.h"
#include "Progress.h"
#include "RenameFiles.h"
#include "ScanDirs.h"
#include "SearchBar.h"
#include "Statistics.h"
#include "Utils.h"
#include "Welcome.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSplitter>
#include <QUrl>

/*--------------------------------------------------------------------*\
		Public
\*--------------------------------------------------------------------*/
MainWindow::MainWindow(Config *config)
{
	// Seed for random functions
	QTime midnight(0, 0, 0);
	qsrand(midnight.secsTo(QTime::currentTime()));

	// Members
	_utils              = new Utils(this, config);
	_splitter           = new QSplitter;
	_listWidget         = new QListWidget;
	_filmView           = new FilmView(_utils);
	_currentFilm        = NULL;
	_isCollectionOpened = true;

	// Properties
	this->setCentralWidget(_splitter);
	this->setWindowIcon(QIcon(":/images/icon"));
	this->setWindowTitle(Fk::ApplicationTitle);

	_splitter->addWidget(_listWidget);
	_splitter->addWidget(new QWidget);

	_listWidget->setSortingEnabled(true);
	_listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	// Connects
	connect(_listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(launchFilm()));
	connect(_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(switchFilm(QListWidgetItem*)));
	connect(_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(switchFilm(QListWidgetItem*)));
	connect(_listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayMenu(QPoint)));

	// Menu
	QMenu *fileMenu(this->menuBar()->addMenu(tr("File")));
	fileMenu->addAction(QIcon(":/images/database"), tr("Create collection"), this, SLOT(newCollection()));
	fileMenu->addAction(QIcon(":/images/open"), tr("Open collection"), this, SLOT(openCollection()));
	fileMenu->addAction(QIcon(":/images/delete"), tr("Delete collection"), this, SLOT(deleteCollection()));
	fileMenu->addAction(QIcon(":/images/optimize"), tr("Clean collection"), this, SLOT(optimizeCollection()));
	fileMenu->addAction(QIcon(":/images/quit"), tr("Quit"), this, SLOT(close()));

	_collectionMenu = this->menuBar()->addMenu(tr("Collection"));

	QMenu* collectionMenuSub = _collectionMenu->addMenu(QIcon(":/images/add"), tr("Add films"));
	collectionMenuSub->addAction(QIcon(":/images/scan"), tr("Scan a directory"), this, SLOT(runDirectoryScan()));
	collectionMenuSub->addAction(QIcon(":/images/add"), tr("Add films by hand"), this, SLOT(addNewFilms()));

	_collectionMenu->addAction(QIcon(":/images/rescan"), tr("Re-scan all directories"), this, SLOT(rescan()));
	_collectionMenu->addSeparator();
	_collectionMenu->addAction(QIcon(":/images/update"), tr("Update films"), this, SLOT(updateFilms()));
	_collectionMenu->addAction(QIcon(":/images/doubloons"), tr("Search for doubloons"), this, SLOT(doubloons()));
	_collectionMenu->addAction(QIcon(":/images/rename"), tr("Rename files"), this, SLOT(renameFiles()));
	_collectionMenu->addSeparator();
	_collectionMenu->addAction(QIcon(":/images/statistics"), tr("Statistics"), this, SLOT(statistics()));

	QMenu *toolsMenu(this->menuBar()->addMenu(tr("Tools")));
	toolsMenu->addAction(QIcon(":/images/preferences"), tr("Preferences"), this, SLOT(preferences()));
	toolsMenu->addAction(QIcon(":/images/help"), tr("Help"), this, SLOT(help()));
	toolsMenu->addAction(QIcon(":/images/star_100"), tr("About"), this, SLOT(about()));

	// Add the search toolbar
	this->addToolBar(_utils->searchBar());
}

MainWindow::~MainWindow()
{
	#ifndef QT_NO_DEBUG
		qDebug() << "~MainWindow()";
	#endif

	delete _utils;
}

void MainWindow::run()
{
	// Open default collection
	this->openCollection(_utils->config()->dataLocation() + Fk::CollectionDir + _utils->config()->value(Fk::DefaultCollection).toString() + Fk::CollectionExtension);

	// Check if a newer version is available
	if (_utils->network()->get(Fk::UrlCheckVersion, Fk::NewVersionTimeout) && _utils->network()->response().simplified().toFloat() > Fk::Version)
	{
		// Create a new toolbar
		QToolBar    *toolBar(new QToolBar);
		QLabel      *text(new QLabel("<b>" + tr("A new version is available !") + "</b>"));
		QWidget     *separator(new QWidget);
		QPushButton *download(new QPushButton(QIcon(":/images/download"), tr("Download")));

		// Properties
		toolBar->setMovable(false);
		toolBar->setStyleSheet("QToolBar{border: 1px solid grey}");
		text->setStyleSheet("color: #5555CC");
		separator->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

		// Connects
		connect(download, SIGNAL(clicked()), this, SLOT(downloadNewVersion()));
		connect(download, SIGNAL(clicked()), toolBar, SLOT(close()));

		// Layouts
		toolBar->addAction(QIcon(":/images/informations"), NULL);
		toolBar->addWidget(text);
		toolBar->addWidget(separator);
		toolBar->addWidget(download);
		toolBar->addAction(QIcon(":/images/close"), NULL, toolBar, SLOT(close()));

		// Add toolbar to main window
		this->addToolBar(Qt::BottomToolBarArea, toolBar);
	}
}

void MainWindow::updateVisible(bool enableInterface)
{
	// This function is called each time a film is added or removed, or when widget in right is switched

	// If interface doesn't need to be disabled, we can handle menu, list widget and searchbar manually
	// Else we don't need to bother because we will disable all of this
	if (enableInterface)
	{
		// If there is no more films, switch to Welcome widget and set current film indicators to NULL
		if (_utils->filmsList(Fk::Visible).count() == 0)
		{
			_currentFilm = NULL;

			// Only if a film view is active, switch to Welcome
			if (_splitter->widget(1) == _filmView)
				this->switchWidget(new Welcome(_utils));
		}

		// Enable/Disable searchBar if at least one film is in list
		_utils->searchBar()->setEnabled(_listWidget->count() > 0);

		// Enable/Disable add films menues if collection is opened (i.e no error appeared during opening)
		_collectionMenu->actions()[0]->setEnabled(_isCollectionOpened);

		// Enable/Disable collection menus if at least one film is in list
		for (int i = 1; i < _collectionMenu->actions().count(); i++)
			_collectionMenu->actions()[i]->setEnabled(_listWidget->count() > 0);
	}

	/*
	   This is for integrated widgets: enable/disable the entire window (menu, toolbar, list widget) except the integrated widget
	   This is to prevent the user for clicking on menu, list widget or searchbar while an integrated widget is running: he has to close the integrated
	   widget before doing anything else
	*/
	this->menuBar()->setEnabled(enableInterface);
	_utils->searchBar()->setEnabled(enableInterface);
	_listWidget->setEnabled(enableInterface);
}

void MainWindow::addFilm(const QString &fileName, const QString &dateTimeAdd, bool isFilename)
{
	// Create a film
	Film *film = new Film(_utils, fileName, dateTimeAdd, isFilename);

	// Insert it in database and attach it to program
	if (_utils->db()->insertFilm(film))
		this->attach(film);
}

void MainWindow::attach(Film *film)
{
	// Add film to internal list
	_utils->appendFilm(film);

	// If film is not a part, add it to list widget
	if ( ! film->isPart())
		_listWidget->addItem(film);

	// Update interface
	this->updateVisible();
}

void MainWindow::clearListWidget()
{
	// Delete all films present in list widget
	_listWidget->clear();
}

/*--------------------------------------------------------------------*\
		Protected
\*--------------------------------------------------------------------*/
void MainWindow::closeEvent(QCloseEvent *event)
{
	// This will delete current widget if different of _filmView (example: close window when you are editing a film)
	// Then automatic qt delete will delete _filmView corretly
	this->switchWidget(_filmView);
	event->accept();
}

/*--------------------------------------------------------------------*\
		Public slots
\*--------------------------------------------------------------------*/
void MainWindow::reloadCurrentFilm()
{
	// If current film exists, refresh it
	if (_currentFilm != NULL)
	{
		// Force refresh
		Film *film = _currentFilm;
		_currentFilm = NULL;
		this->switchFilm(film, true);
	}

	// Else show Welcome widget (this case appears when, for example, we display help and no film was shown just before,
	// at opening of program, Welcome widget is shown and not a film)
	else
		this->switchWidget(new Welcome(_utils));
}

void MainWindow::switchFilm(QListWidgetItem *newItem, bool selectFilm)
{
	// Try to cast QListWidget* to Film*
	Film *film = dynamic_cast<Film*>(newItem);

	// If cast is a success
	if (film != NULL && film != _currentFilm)
	{
		// Set currentFilm to film (the current film is always the film shown on filmView and the film selected in list widget
		_currentFilm = film;

		// Switch film view to update contents with the new film infos
		_filmView->setFilm(film);

		// Set the current widget to _filmView to display the film view instead of any other widget
		this->switchWidget(_filmView);

		// Scroll to the film in the list widget
		_listWidget->scrollToItem(film);

		// If asked, select the film
		if (selectFilm)
			_listWidget->setCurrentItem(film);
	}
}

void MainWindow::launchTrailer() const
{
	// Try yo cast an Action* (means that this function have been called by a Action* from a QMenu in a multiple-fusionned-films buttons view)
	ActionFilm *action = dynamic_cast<ActionFilm*>(sender());
	QString url;

	// If cast is a success, take the corresponding film url
	if (action != NULL)
		url = action->film()->trailerUrl();

	// Else take the current film url
	else if (_currentFilm != NULL)
		url = _currentFilm->trailerUrl();

	// Open url of the trailer in default application (Web Browser generally)
	if ( ! url.isEmpty())
		QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::launchFilm() const
{
	// Try yo cast an Action* (means that this function have been called by a Action* from a QMenu in a multiple-fusionned-films buttons view)
	ActionFilm *action = dynamic_cast<ActionFilm*>(sender());
	QString filename;

	// If cast is a success, take the corresponding film filename
	if (action != NULL)
		filename = action->film()->filename();

	// Else take the current film filename
	else if (_currentFilm != NULL)
		filename = _currentFilm->filename();

	// Open filename in default application (Video player generally)
	if ( ! filename.isEmpty())
	{
		QString file = _utils->searchFilename(filename);

		if ( ! file.isEmpty())
			QDesktopServices::openUrl(QUrl("file:///" + file));
	}
}

void MainWindow::removeFilm(Film *film, bool removeFile)
{
	// Get all film parts, and add film itself to list
	QList<Film*> filmsList(_utils->filmParts(film));
	filmsList.append(film);

	// Foreach film
	foreach (Film* filmPart, filmsList)
	{
		// Remove file if asked
		if (removeFile)
			_utils->removeFile(filmPart);

		// Remove film from database
		_utils->db()->removeFilm(filmPart);

		// Remove film from internal list and delete it (this will remove it from list widget also)
		_utils->removeFilm(filmPart);
	}

	// Update interface
	this->updateVisible();
}

/*--------------------------------------------------------------------*\
		Private slots
\*--------------------------------------------------------------------*/
// Toolbar
void MainWindow::downloadNewVersion()
{
	// Foreach operating system, try to execute updater and close this program
	// Some OS aren't supported, new version download is manual so

	QStringList args(QString::number(Fk::Version));

	// Windows
	#if defined(Q_WS_WIN)
		if (QProcess::startDetached(QApplication::applicationDirPath() + "/Updater.exe", args))
			this->close();

		else
			QMessageBox::critical(this, tr("Error"), tr("Impossible to launch updater program.<br />Retry with administrator rights."));

	// Unix
	#elif defined(Q_WS_X11)
		if (QProcess::startDetached(QApplication::applicationDirPath() + "/Updater", args))
			this->close();

		else
			QMessageBox::critical(this, tr("Error"), tr("Impossible to launch updater program.<br />Retry with administrator rights."));

	// Others operating systems
	#else
		QMessageBox::critical(this, tr("Error"), tr("We are sorry, automatic update doesn't exist for your system."));

	#endif
}

// Menu
void MainWindow::newCollection()
{
	Dialog dialog(_utils, tr("New collection"));
	dialog.installNewCollectionEdit();

	// Ask for collection name and create-open it
	if (dialog.exec() == QDialog::Accepted && ! dialog.lineEditText().isEmpty())
	{
		if (QFile::exists(_utils->config()->dataLocation() + Fk::CollectionDir + dialog.lineEditText() + Fk::CollectionExtension))
			QMessageBox::critical(this, tr("Collection already exists"), tr("A collection already exists with this name."));

		else
			this->openCollection(_utils->config()->dataLocation() + Fk::CollectionDir + dialog.lineEditText() + Fk::CollectionExtension);
	}
}

void MainWindow::openCollection()
{
	// Get collection filename to open
	QString filename(QFileDialog::getOpenFileName(this, tr("Open a collection"), _utils->config()->dataLocation() + Fk::CollectionDir));

	// Create-open it
	if ( ! filename.isEmpty())
		this->openCollection(filename);
}

void MainWindow::deleteCollection()
{
	Dialog dialog(_utils, tr("Delete collection"));

	QStringList collections;
	QDir dir(_utils->config()->dataLocation() + Fk::CollectionDir);

	if (dir.exists())
	{
		dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

		// Foreach file, if it has a collection extension, add it to collections names
		foreach (QFileInfo itemInfo, dir.entryInfoList())
			if (itemInfo.fileName().endsWith(Fk::CollectionExtension, Qt::CaseInsensitive))
				collections << itemInfo.fileName().left(itemInfo.fileName().count() - Fk::CollectionExtension.count());
	}

	dialog.addComboBox(collections);

	// Ask for collection name and create-open it
	if (dialog.exec() == QDialog::Accepted)
	{
		if (QFile::remove(_utils->config()->dataLocation() + Fk::CollectionDir + dialog.comboBoxEditText() + Fk::CollectionExtension))
			QMessageBox::information(this, tr("Collection deleted"), tr("The collection has been correctly deleted."));

		else
			QMessageBox::information(this, tr("Error deleting collection"), tr("An error occured while deleting this collection."));
	}
}

void MainWindow::optimizeCollection() const
{
	Progress progress(tr("Clean collection"), tr("Cleaning collection...<br /><br />This removes extra-disk-space used by deleted films<br /><br /><b>This can take few minutes so please be patient and don't kill the application !</b>"), 0);

	// Compact database
	_utils->db()->compact();
}

void MainWindow::addNewFilms()
{
	Dialog dialog(_utils, tr("Add new films"));
	dialog.addTextEdit(tr("Each film must be separated with a line feed: "));

	// Ask for list of films
	if (dialog.exec() == QDialog::Accepted)
	{
		// Prepare date and time of add, formating it from locale format (depending on language)
		QString dateTime(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));

		// Foreach film found, add it to collection
		foreach (QString title, dialog.textEditText().split("\n"))
			if ( ! title.simplified().isEmpty())
				this->addFilm(title.simplified(), dateTime, false);
	}
}

void MainWindow::runDirectoryScan()
{
	// Ask for the dir to scan
	QString dirName(QFileDialog::getExistingDirectory(this, tr("Select a dir to scan"), _utils->config()->homeLocation()));

	if ( ! dirName.isEmpty())
	{
		Progress progress(tr("Add films"), tr("Scanning dir..."), 0);

		// Scan recursivly the dir
		QStringList filmsList;
		_utils->scanDirectory(dirName, filmsList);

		// Prepare date and time of add, formating it from locale format (depending on language)
		QString dateTime(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));

		progress.setMax(filmsList.count());
		progress.setText(tr("Adding films to collection..."));

		// Foreach film found, add it to collection
		foreach (QString fileName, filmsList)
		{
			this->addFilm(fileName, dateTime);
			progress.incrementValue();
		}
	}
}

void MainWindow::rescan() const
{
	// Start the ScanDirs extern widget
	new ScanDirs(_utils);
}

void MainWindow::updateFilms()
{
	// Get all uncomplete films
	QList<Film*> filmsList(_utils->filmsList(Fk::Visible | Fk::Uncomplete));

	// If there is at least one film to complete
	if (filmsList.count() > 0)
	{
		Progress* progress = new Progress(tr("Update collection"), tr("Updating collection..."), filmsList.count());
		progress->installStopButton();

		QTime time;
		time.start();

		// Foreach film
		foreach (Film *film, filmsList)
		{
			// Is stop is asked by user, stop the loop
			if (progress->isStopped())
				break;

			// Parse film with internet
			_utils->parser()->complete(film);

			// Show the film immediatly (the user can follow completion progression by seeing all films one by one)
			this->switchFilm(film, true);

			progress->incrementValue();
		}

		if (progress != NULL)
			delete progress;

		QMessageBox::information(this, tr("Finished"), tr("Update of collection finished in %n minute(s).", "", time.elapsed() / 60000));
	}
}

void MainWindow::doubloons() const
{
	// Start the Doubloons extern widget
	new Doubloons(_utils);
}

void MainWindow::renameFiles() const
{
	// Start the RenameFiles extern widget
	new RenameFiles(_utils);
}

void MainWindow::statistics() const
{
	// Start the Statistics extern widget
	new Statistics(_utils);
}

void MainWindow::help()
{
	// Start the integrated Help widget
	this->switchWidget(new Help(_utils), false);
}

void MainWindow::preferences() const
{
	// Start the Preferences extern widget
	new Preferences(_utils);
}

void MainWindow::about()
{
	// Start the About extern widget
	new About(_utils);
}

// Right-clic menu
void MainWindow::displayMenu(QPoint position) const
{
	// Get mouse position
	QWidget *sender(qobject_cast<QWidget*>(this->sender()));

	// Set position to absolute mouse position (so the menu will display at exactly mouse position)
	if (sender != NULL)
		position = sender->mapToGlobal(position);

	// If there is more than 1 film in selection, menu is this one
	if (_listWidget->selectedItems().count() > 1)
	{
		// Create menu
		QMenu *menu(new QMenu);

		// Switch menu
		QMenu *menuChange(menu->addMenu(QIcon(":/images/switch"), tr("Switch")));

		// Seen/Not seen
		menuChange->addAction(QIcon(":/images/apply_green"), tr("Seen"), this, SLOT(setSeen()));
		menuChange->addAction(QIcon(":/images/cross"), tr("Not seen"), this, SLOT(setNotSeen()));
		menuChange->addSeparator();

		// Check/Uncheck
		menuChange->addAction(QIcon(":/images/apply_blue"), tr("Check"), this, SLOT(setChecked()));
		menuChange->addAction(QIcon(":/images/cross"), tr("Uncheck"), this, SLOT(setUnchecked()));
		menuChange->addSeparator();

		// Complete/Uncomplete
		menuChange->addAction(QIcon(":/images/apply_blue"), tr("Complete"), this, SLOT(setComplete()));
		menuChange->addAction(QIcon(":/images/cross"), tr("Uncomplete"), this, SLOT(setUncomplete()));

		menu->addSeparator();

		// Switch comment/dir path
		menu->addAction(QIcon(":/images/switchComment"), tr("Switch comment"), this, SLOT(switchComment()));
		menu->addAction(QIcon(":/images/switchDirPath"), tr("Switch dir path"), this, SLOT(switchDirPath()));
		menu->addSeparator();

		// Fusion/Unfusion
		menu->addAction(QIcon(":/images/fusion"), tr("Fusion"), this, SLOT(fusion()));
		menu->addAction(QIcon(":/images/unfusion"), tr("Unfusion"), this, SLOT(unfusion()));
		menu->addSeparator();

		// Delete
		menu->addAction(QIcon(":/images/delete"), tr("Delete"), this, SLOT(remove()));

		// Exec menu and call deleteLater to delete it as soon as the user made a choice
		menu->exec(position);
		menu->deleteLater();
	}

	// Else menu is this one (note that == 1 is used, because if right-clic can be done without any film, no-one of the two possibles menus should appear,
	// so the 0 case is intentionally not treated here)
	else if(_listWidget->selectedItems().count() == 1)
	{
		// Create menu
		QMenu *menu(new QMenu);

		// Seen/Not seen
		if (_currentFilm->isSeen())
			menu->addAction(QIcon(":/images/cross"), tr("Not seen"), this, SLOT(setNotSeen()));

		else
			menu->addAction(QIcon(":/images/apply_green"), tr("Seen"), this, SLOT(setSeen()));

		// Check/Uncheck
		if (_currentFilm->isChecked() )
			menu->addAction(QIcon(":/images/cross"), tr("Uncheck"), this, SLOT(setUnchecked()));

		else
			menu->addAction(QIcon(":/images/apply_blue"), tr("Check"), this, SLOT(setChecked()));

		// Complete/Uncomplete
		if (_currentFilm->isComplete() )
			menu->addAction(QIcon(":/images/cross"), tr("Uncomplete"), this, SLOT(setUncomplete()));

		else
			menu->addAction(QIcon(":/images/apply_blue"), tr("Complete"), this, SLOT(setComplete()));

		menu->addSeparator();

		// Propose unfusion only if film as parts
		if (_currentFilm->idNextPart() != -1)
		{
			menu->addAction(QIcon(":/images/unfusion"), tr("Unfusion"), this, SLOT(unfusion()));
			menu->addSeparator();
		}

		// Update methods: all choices, update, update by url, modify manually
		menu->addAction(QIcon(":/images/allChoices"), tr("All choices"), this, SLOT(allChoices()));
		menu->addAction(QIcon(":/images/update"), tr("Update"), this, SLOT(update()));
		menu->addAction(QIcon(":/images/modify"), tr("Modify"), this, SLOT(modify()));
		menu->addSeparator();

		// Delete
		menu->addAction(QIcon(":/images/delete"), tr("Delete"), this, SLOT(remove()));

		// Exec menu and call deleteLater to delete it as soon as the user made a choice
		menu->exec(position);
		menu->deleteLater();
	}
}

void MainWindow::setSeen()
{
	// Get selected and not seen films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::NotSeen | Fk::Parts));

	// If there is only 1 film
	if (filmsList.count() == 1)
	{
		// Set seen and update in database
		filmsList.at(0)->setIsSeen(true);
		_utils->db()->updateFilm(filmsList.at(0), false);
	}

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		Progress progress(tr("Set seen"), tr("Setting selection to seen..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set seen and update in database
			film->setIsSeen(true);
			_utils->db()->updateFilm(film, false);
			progress.incrementValue();
		}
	}

	// Reload search to update list of visible films
	_utils->searchBar()->reload();
}

void MainWindow::setNotSeen()
{
	// Get selected and seen films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::Seen | Fk::Parts));

	// If there is only 1 film
	if (filmsList.count() == 1)
	{
		// Set not seen and update in database
		filmsList.at(0)->setIsSeen(false);
		_utils->db()->updateFilm(filmsList.at(0), false);
	}

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		Progress progress(tr("Set not seen"), tr("Setting selection to not seen..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set not seen and update in database
			film->setIsSeen(false);
			_utils->db()->updateFilm(film, false);
			progress.incrementValue();
		}
	}

	// Reload search to update list of visible films
	_utils->searchBar()->reload();
}

void MainWindow::switchSeen()
{
	if (_currentFilm != NULL)
	{
		// Toggle seen and update in database
		_currentFilm->setIsSeen( ! _currentFilm->isSeen());
		_utils->db()->updateFilm(_currentFilm, false);

		// Reload search because seen/not seen films can be checked
		_utils->searchBar()->reload();
	}
}

void MainWindow::setComplete()
{
	// Get selected and uncomplete films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::Uncomplete));

	// If there is only 1 film
	if (filmsList.count() == 1)
	{
		// Set complete and update in database
		filmsList.at(0)->setIsComplete(true);
		_utils->db()->updateFilm(filmsList.at(0), false);
	}

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		Progress progress(tr("Set complete"), tr("Setting selection to complete..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set complete and update in database
			film->setIsComplete(true);
			_utils->db()->updateFilm(film, false);
			progress.incrementValue();
		}
	}
}

void MainWindow::setUncomplete()
{
	// Get selected and complete films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::Complete));

	// If there is only 1 film
	if (filmsList.count() == 1)
	{
		// Set uncomplete and update in database
		filmsList.at(0)->setIsComplete(false);
		_utils->db()->updateFilm(filmsList.at(0), false);
	}

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		Progress progress(tr("Set not complete"), tr("Setting selection to not complete..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set uncomplete and update in database
			film->setIsComplete(false);
			_utils->db()->updateFilm(film, false);
			progress.incrementValue();
		}
	}
}

void MainWindow::setChecked()
{
	// Get selected and unchecked films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::Unchecked));

	// If there is only 1 film
	if (filmsList.count() == 1)
		filmsList.at(0)->setIsChecked(true);

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		Progress progress(tr("Set checked"), tr("Setting selection to checked..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set checked
			film->setIsChecked(true);
			progress.incrementValue();
		}
	}


	// Reload search to update list of visible films
	_utils->searchBar()->reload();
}

void MainWindow::setUnchecked()
{
	// Get selected and checked films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::Checked));

	// If there is only 1 film
	if (filmsList.count() == 1)
		// Set unchecked
		filmsList.at(0)->setIsChecked(false);

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		Progress progress(tr("Set unchecked"), tr("Setting selection to unchecked..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set unchecked
			film->setIsChecked(false);
			progress.incrementValue();
		}
	}

	// Reload search to update list of visible films
	_utils->searchBar()->reload();
}

void MainWindow::switchComment()
{
	// Get selected films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected));

	Dialog dialog(_utils, tr("Switch comment"));

	// Pre-fill lineEdit with comment if filmslist count is 1
	QString comment;

	dialog.addLineEdit(tr("Comment: "), ".{0," + QString::number(Fk::CommentLimit) + "}", comment);

	// Ask for new comment
	if (dialog.exec() == QDialog::Accepted)
	{
		Progress progress(tr("Set comment"), tr("Setting comment for selection..."), filmsList.count());

		foreach (Film *film, filmsList)
		{
			// Set comment and update in database
			film->setComment(dialog.lineEditText());
			_utils->db()->updateFilm(film, false);
			progress.incrementValue();
		}
	}

	// Reload search to update list of visible films
	_utils->searchBar()->reload();
}

void MainWindow::switchDirPath()
{
	Dialog dialog(_utils, tr("Switch dir path"));
	dialog.installPathEdit();

	// Ask for new dir path
	if (dialog.exec() == QDialog::Accepted && ! dialog.comboBoxEditText().isEmpty())
	{
		// Get selected films
		QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::Parts));

		Progress progress(tr("Set dir path"), tr("Setting dir path..."), filmsList.count());

		// Foreach film
		foreach (Film *film, filmsList)
		{
			// Set path and update in database
			film->setPath(dialog.comboBoxEditText());
			_utils->db()->updateFilm(film, false);
			progress.incrementValue();
		}

		// Reload search to update list of visible films
		_utils->searchBar()->reload();
	}
}

void MainWindow::fusion()
{
	// Get selected films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected));

	// If there is more than 1 film (the case "0 film" and "1 film" aren't treated)
	if (filmsList.count() > 1)
	{
		// Foreach film, add all their parts in filmsList
		foreach (Film *film, filmsList)
			foreach (Film *filmPart, _utils->filmParts(film))
				if ( ! filmsList.contains(filmPart))
					filmsList.append(filmPart);

		Progress progress(tr("Fusion"), tr("Fusionning selection..."), filmsList.count());

		// Then foreach film (which is all selected films + their parts)
		for (int i = 0; i < filmsList.count(); i++)
		{
			// If we aren't to the last film of the list yet, set the idNextPart to the next film in the list
			if (i < filmsList.count() - 1)
				filmsList.at(i)->setIdNextPart(filmsList.at(i + 1)->id());

			// Declare all films but the first to be a part, and remove it from list widget
			if (i > 0)
			{
				filmsList.at(i)->setIsPart(true);
				_listWidget->takeItem(_listWidget->row(filmsList.at(i)));
			}

			// Update current film in database
			_utils->db()->updateFilm(filmsList.at(i), false);
			progress.incrementValue();
		}

		// Reload the first film in the list: the one which contains all other parts
		this->switchFilm(filmsList.at(0), true);
	}
}

void MainWindow::unfusion()
{
	// Get selected films, then only thus which contain parts
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected | Fk::ContainingParts));

	// Calcul the correct number of films to treat
	int nbElements = filmsList.count();

	foreach (Film *film, filmsList)
		nbElements += _utils->filmParts(film).count();

	Progress progress(tr("Unfusion"), tr("Unfusionning selection..."), nbElements);

	// Foreach film (not including their parts)
	foreach (Film *film, filmsList)
	{
		int idNextPart;

		// Current film is the first
		Film *currentFilm = film;

		// If current film has part(s)
		while (currentFilm->idNextPart() != -1)
		{
			// Get the part id
			idNextPart = currentFilm->idNextPart();

			// Detach next film from current film and update in database
			currentFilm->setIdNextPart(-1);
			_utils->db()->updateFilm(currentFilm, false);

			// The part become the current film
			currentFilm = _utils->film(idNextPart);

			// The old part (which is now a real film) becomes no longer a part, update in database and add it to list widget (as a normal film)
			// Here we can't call this->attach(Film *film) because this function will append Film* to internal list, but the part was already in this list,
			// so we only have to add it to list widget
			currentFilm->setIsPart(false);
			_utils->db()->updateFilm(currentFilm, false);
			_listWidget->addItem(currentFilm);
			progress.incrementValue();
		}

		progress.incrementValue();
	}

	// Reload search to update list of visible films (and take account of new films added in list widget)
	_utils->searchBar()->reload();
}

void MainWindow::allChoices()
{
	// Start the AllChoices extern widget
	new AllChoices(_utils, _currentFilm);
}

void MainWindow::update()
{
	// This only is possible for 1 film: the one which is selected
	if (_currentFilm != NULL)
	{
		Progress *progress = new Progress(tr("Update"), tr("Updating of %1...").arg("<b>" + _currentFilm->title() + "</b>"), 0);

		// Complete film automatically
		bool result = _utils->parser()->complete(_currentFilm);
		delete progress;

		// Reload search
		_utils->searchBar()->reload();

		// Show message of success or error
		if (result)
			QMessageBox::information(this, tr("Ok"), tr("%1 has been updated successfully.").arg("<b>" + _currentFilm->title() + "</b>"));

		else
			QMessageBox::critical(this, tr("Error"), tr("Impossible to update %1.").arg("<b>" + _currentFilm->title() + "</b>"));
	}
}

void MainWindow::modify()
{
	// This only is possible for 1 film: the one which is selected
	if (_currentFilm != NULL)
		// Start the integrated FilmModify widget
		this->switchWidget(new FilmModify(_utils, _currentFilm), false);
}

void MainWindow::remove()
{
	// Get selected films
	QList<Film*> filmsList(_utils->filmsList(Fk::Selected));

	// If there is only 1 film
	if (filmsList.count() == 1)
	{
		// Confirmation text
		QString text(tr("Remove %1 from collection ?").arg("<b>" + filmsList.at(0)->title() + "</b>"));

		// If there is part(s), add it to confirmation text
		int nbParts = _utils->filmParts(filmsList.at(0)).count();

		if (nbParts > 0)
			text += "<br /><br />" + tr("Warning: this film is composed of %n part(s) which will be deleted as well.", "", nbParts);

		Dialog dialog(_utils, tr("Remove a film"));
		dialog.addText(text + "<br />");
		dialog.addRemoveFile(QFile::exists(filmsList.at(0)->filename()), tr("Also remove file"));

		// Ask confirmation
		if (dialog.exec() == QDialog::Accepted)
		{
			// Deleted films will disappear from list, so switch to next film
			_utils->searchBar()->nextFilm();

			// Remove film
			this->removeFilm(filmsList.at(0), dialog.isRemoveFileChecked());
		}
	}

	// Else if there is more than 1 film (the case "0 film" isn't treated)
	else if (filmsList.count() > 1)
	{
		// Confirmation text
		QString text(tr("Remove selected films from collection ?") + "<br /><br />" +
					 tr("Warning: all composed parts will be deleted as well."));

		Dialog dialog(_utils, tr("Remove films"));
		dialog.addText(text + "<br />");
		dialog.addRemoveFile(true, tr("Also remove files"));

		// Ask confirmation
		if (dialog.exec() == QDialog::Accepted)
		{
			// Deleted films will disappear from list, so switch to next film
			_utils->searchBar()->nextFilm();

			Progress progress(tr("Delete films"), tr("Deleting films..."), filmsList.count());

			// Foreach film, remove it
			foreach (Film *film, filmsList)
			{
				this->removeFilm(film, dialog.isRemoveFileChecked());
				progress.incrementValue();
			}
		}
	}
}

/*-----------------------------------------------------------------*\
		Private
\*-----------------------------------------------------------------*/
void MainWindow::openCollection(const QString &name)
{
	// By default, collection is opened with success
	_isCollectionOpened = true;

	// Remove all films loaded in memory of program (and clear list widget)
	_utils->removeAllFilms();
	QApplication::processEvents();

	// Switch database name to switch file
	_utils->db()->setDatabaseName(name);

	// Create needed tables in database
	if ( ! _utils->db()->create())
	{
		_isCollectionOpened = false;
		QMessageBox::critical(this, tr("Error"), tr("The collection %1 couldn't be created.<br />(%2)").arg("<b>" + name + "</b>", _utils->db()->error()));
	}

	else
	{
		QList<Film*> filmsList;

		// Load films from database
		if ( ! _utils->db()->loadFilms(filmsList))
		{
			// In this case only, the database couldn't be opened
			_isCollectionOpened = false;
			QMessageBox::critical(this, tr("Error"), tr("The collection %1 couldn't be opened.<br />(%2)").arg("<b>" + name + "</b>", _utils->db()->error()));
		}

		// If loaded is a success, attach all films to program (internal list and list widget)
		else
			foreach (Film *film, filmsList)
				this->attach(film);
	}

	// Show Welcome widget
	this->switchWidget(new Welcome(_utils));
}

void MainWindow::switchWidget(QWidget *widget, bool enableInterface)
{
	QWidget *oldWidget(NULL);

	// If the new widget type is different from the current widget
	if (_splitter->widget(1) != widget)
	{
		// Get the old widget installed and remove it from the rigth of splitter, and install the new one
		oldWidget = _splitter->widget(1);
		oldWidget->setParent(NULL);
		_splitter->addWidget(widget);

		// If old widget isn't a film view, we have to delete it manually (only _filmView is kept in memory, we delete it at close())
		if (oldWidget != _filmView)
			delete oldWidget;

		/*
		   Set splitter sizes (1/5 for left and 4/5 for right)
		   This only happen when a new widget type is install on right
		   If we switch film, the same widget is used so this resize isn't called and the user can resize the splitter without seeing it
		   being resized all the time
		*/
		_splitter->setSizes(QList<int>() << this->width() / 5 << (this->width() / 5) * 4);
	}

	// Update interface (collection menu)
	this->updateVisible(enableInterface);
}
