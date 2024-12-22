#include <msg.h>
#include <lang.h>

const std::string& lang(T_MSG msg) {
	return Lang::getString(msg);
}

Lang::Lang(const std::string& lname) {
	_lname = lname;
	std::cout << lname << std::flush;
}

Lang::~Lang() {
}

void Lang::setString(T_MSG msg, std::string str) {
	_sentences[msg] = str;
}

std::string& Lang::getString(T_MSG msg) {
	return _sentences[msg];
}

std::string Lang::getLang() {
	return _lname;
}

std::map<T_MSG, std::string> Lang::_sentences = { { T_MSG::EMPTY, "" } };
std::string Lang::_lname = "";
