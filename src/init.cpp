#include <init.h>
#include <tools.h>

void init_logfile() {
    try {
    // Création des layouts
        log4cpp::PatternLayout *file_layout = new log4cpp::PatternLayout();
        file_layout->setConversionPattern("%m");

        // Création des appenders
        logfile = new log4cpp::FileAppender("logfile", "xtalk.log", false);
        logfile->setLayout(file_layout);

        // Initialisation de la catégorie racine
        xlog = &log4cpp::Category::getRoot();
        xlog->setPriority(log4cpp::Priority::DEBUG); // Niveau minimal de priorité
        xlog->addAppender(logfile);
    } catch (const log4cpp::ConfigureFailure &e) {
        std::cerr << "Erreur lors de l'initialisation de log4cpp : " << e.what() << std::endl;
    }
}
