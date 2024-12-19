#include <fr_FR.h>

fr_FR::fr_FR(): Lang("fr_FR") {
	setString(MSG::GET_HELP,					"Tapez \"cmd_help\" pour obtenir de l'aide");
	setString(MSG::FAKE_SERVER_ENABLED,		"MODE BAC-A-SABLE");
	setString(MSG::DISABLE,					"Désactive");
	setString(MSG::GOODBYE,					"Au revoir !");
	setString(MSG::CONNECTING_TO_XPLANE, 		"Connexion à X-Plane");
	setString(MSG::WAITING_FOR_AIRCRAFT, 		"Détection de l'avion");
	setString(MSG::DONE, 						"terminé");
	setString(MSG::LANGUAGE_SET_TO, 			"Langue sélectionnée");
	setString(MSG::LOADING_AIRPORTS, 			"Chargement des aéroports");
	setString(MSG::LOADING_DIALOG, 			"Chargement des dialogues");
	setString(MSG::CREATING_SERVER, 			"Démarrage du serveur");
	setString(MSG::CREATING_CLIENT, 			"Démarrage du client");
	setString(MSG::AIRPORTS_FOUND, 			"aéroports trouvés");
	setString(MSG::INITIALIZING_VOSK,			"Initialisation de Vosk");
	setString(MSG::LISTING_AVAILABLE_DEVICES,	"Recherche de périphériques compatibles");
	setString(MSG::NO_DEVICE_FOUND, 			"Aucun périphérique trouvé");
	setString(MSG::UNKNOWN_NAME, 				"Nom inconnu");
	setString(MSG::UNABLE_TO_OPEN_JOYSTICK,	"Impossible d'ouvrir le joystick");
	setString(MSG::SELECTING_DEVICE, 			"Sélectionner un joystick");
	setString(MSG::PRESS_ANY_BUTTON_ON_DEVICE, "Appuyez sur un bouton du joystick");
	setString(MSG::LOADING_AUDIO_FILES, 		"Chargement des fichiers audio");
	setString(MSG::LOADING_PHRASES, 			"Chargement du fichier de phrases");
	setString(MSG::UNABLE_TO_OPEN_FILE, 		"Impossible d'ouvrir le fichier");
	setString(MSG::MISFORMATTED_LINE, 			"Ligne mal formattée");
	setString(MSG::NO_WAV_FILES_IN_DIRECTORY, 	"Aucun fichier WAV dans le répertoire");
	setString(MSG::LOADING_ERROR, 				"Erreur au chargement de");
	setString(MSG::ANALYZING, 					"Analyse");
	setString(MSG::RECORDING, 					"Enregistrement... Appuyez sur Entrée pour arrêter");
}
