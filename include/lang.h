#pragma once

#include <pch.h>
#include <constraints.h>
#include <msg.h>
#include <xprint.h>

const std::string& lang(T_MSG msg);

class Lang {
	protected:
		Lang(const std::string& lname);
		~Lang();

		static void setString(T_MSG msg, std::string str);

	private:
		static std::map<T_MSG, std::string> _sentences;
		static std::string _lname;

	public:
		template <typename TLang> static void setLang() {
			Derived_from<TLang, Lang>();
			TLang _lang;
			//X_OUTPUT::xprint(MSG_STYLE::INFO, getString(T_MSG::LANGUAGE_SET_TO),getLang());
			X_OUTPUT::xprint(MSG_STYLE::INFO, lang(T_MSG::LANGUAGE_SET_TO), getLang());
		}

		static std::string& getString(T_MSG msg);
		static std::string getLang();
};