#include "msg.h"
#include <en_US.h>

en_US::en_US(): Lang("en_US") {
	setString(MSG::GET_HELP,					"Type \"cmd_help\" to get help");
	setString(MSG::FAKE_SERVER_ENABLED, 		"FAKE SERVER ENABLED OVERRIDE IP ADDRESS");
	setString(MSG::DISABLE, 					"Disable");
	setString(MSG::GOODBYE, 					"Good bye !");
	setString(MSG::CONNECTING_TO_XPLANE, 		"Connecting to X-Plane");
	setString(MSG::WAITING_FOR_AIRCRAFT, 		"Waiting for aircraft");
	setString(MSG::DONE, 						"done");
	setString(MSG::LANGUAGE_SET_TO, 			"Language set to");
	setString(MSG::LOADING_AIRPORTS, 			"Loading JSON airports");
	setString(MSG::LOADING_DIALOG, 			"Loading JSON dialog tree");
	setString(MSG::CREATING_SERVER, 			"Creating server");
	setString(MSG::CREATING_CLIENT,			"Creating client");
	setString(MSG::AIRPORTS_FOUND,				"airports found");
	setString(MSG::INITIALIZING_VOSK,			"Initializing Vosk");
	setString(MSG::LISTING_AVAILABLE_DEVICES,	"Listing available devices");
	setString(MSG::NO_DEVICE_FOUND, 			"No device found");
	setString(MSG::UNKNOWN_NAME, 				"Unknown name");
	setString(MSG::UNABLE_TO_OPEN_JOYSTICK,	"Unable to open joystick");
	setString(MSG::SELECTING_DEVICE, 			"Selecting device");
	setString(MSG::PRESS_ANY_BUTTON_ON_DEVICE, "Press any button on device");
	setString(MSG::LOADING_AUDIO_FILES, 		"Loading audio files");
	setString(MSG::LOADING_PHRASES, 			"Loading phrase file");
	setString(MSG::UNABLE_TO_OPEN_FILE, 		"Unable to open file");
	setString(MSG::MISFORMATTED_LINE, 			"Misformatted line");
	setString(MSG::NO_WAV_FILES_IN_DIRECTORY, 	"No WAV files in directory");
	setString(MSG::LOADING_ERROR, 				"Error loading");
	setString(MSG::ANALYZING, 					"Analyzing");
	setString(MSG::RECORDING,					"Recording... Press Enter to stop");
}
