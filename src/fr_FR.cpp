#include <fr_FR.h>

fr_FR::fr_FR(): Lang("fr_FR") {
	setString(T_MSG::GET_HELP,					"Tapez \"cmd_help\" pour obtenir de l'aide");
	setString(T_MSG::FAKE_SERVER_ENABLED,		"MODE BAC-A-SABLE");
	setString(T_MSG::DISABLE,					"Désactive");
	setString(T_MSG::GOODBYE,					"Au revoir !");
	setString(T_MSG::CONNECTING_TO_XPLANE, 		"Connexion à X-Plane");
	setString(T_MSG::WAITING_FOR_AIRCRAFT, 		"Détection de l'avion");
	setString(T_MSG::DONE, 						"terminé");
	setString(T_MSG::LANGUAGE_SET_TO, 			"Langue sélectionnée");
	setString(T_MSG::LOADING_AIRPORTS, 			"Chargement des aéroports");
	setString(T_MSG::LOADING_DIALOG, 			"Chargement des dialogues");
	setString(T_MSG::CREATING_SERVER, 			"Démarrage du serveur");
	setString(T_MSG::CREATING_CLIENT, 			"Démarrage du client");
	setString(T_MSG::AIRPORTS_FOUND, 			"aéroports trouvés");
	setString(T_MSG::INITIALIZING_VOSK,			"Initialisation de Vosk");
	setString(T_MSG::LISTING_AVAILABLE_DEVICES,	"Recherche de périphériques compatibles");
	setString(T_MSG::NO_DEVICE_FOUND, 			"Aucun périphérique trouvé");
	setString(T_MSG::UNKNOWN_NAME, 				"Nom inconnu");
	setString(T_MSG::UNABLE_TO_OPEN_JOYSTICK,	"Impossible d'ouvrir le joystick");
	setString(T_MSG::SELECTING_DEVICE, 			"Sélectionner un joystick");
	setString(T_MSG::PRESS_ANY_BUTTON_ON_DEVICE, "Appuyez sur un bouton du joystick");
	setString(T_MSG::LOADING_AUDIO_FILES, 		"Chargement des fichiers audio");
	setString(T_MSG::LOADING_PHRASES, 			"Chargement du fichier de phrases");
	setString(T_MSG::UNABLE_TO_OPEN_FILE, 		"Impossible d'ouvrir le fichier");
	setString(T_MSG::MISFORMATTED_LINE, 			"Ligne mal formattée");
	setString(T_MSG::NO_WAV_FILES_IN_DIRECTORY, 	"Aucun fichier WAV dans le répertoire");
	setString(T_MSG::LOADING_ERROR, 				"Erreur au chargement de");
	setString(T_MSG::ANALYZING, 					"Analyse");
	setString(T_MSG::RECORDING, 					"Enregistrement... Appuyez sur Entrée pour arrêter");
}
