#include <msg.h>
#include <lang.h>

const std::string& lang(MSG msg) {
	return Lang::getString(msg);
}

Lang::Lang(const std::string& lname) {
	_lname = lname;
	std::cout << lname << std::flush;
}

Lang::~Lang() {
}

void Lang::setString(MSG msg, std::string str) {
	_sentences[msg] = str;
}

std::string& Lang::getString(MSG msg) {
	return _sentences[msg];
}

std::string Lang::getLang() {
	return _lname;
}

std::map<MSG, std::string> Lang::_sentences = { { MSG::EMPTY, "" } };
std::string Lang::_lname = "";
