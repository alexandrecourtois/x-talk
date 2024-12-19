#pragma once

#include <pch.h>
#include <constraints.h>
#include <msg.h>
#include <xprint.h>

const std::string& lang(MSG msg);

class Lang {
	protected:
		Lang(const std::string& lname);
		~Lang();

		static void setString(MSG msg, std::string str);

	private:
		static std::map<MSG, std::string> _sentences;
		static std::string _lname;

	public:
		template <typename TLang> static void setLang() {
			Derived_from<TLang, Lang>();
			TLang _lang;
			//OUT::xprint(MSG_STYLE::INFO, getString(MSG::LANGUAGE_SET_TO),getLang());
			OUT::xprint(MSG_STYLE::INFO, lang(MSG::LANGUAGE_SET_TO), getLang());
		}

		static std::string& getString(MSG msg);
		static std::string getLang();
};