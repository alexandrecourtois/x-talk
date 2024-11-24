#ifndef XSCANF_H
#define XSCANF_H

#include <string>

bool kbhit(char &c);
void xscan(std::string &str, void (*callback)() = nullptr, int delay = 100);
void xscan();
void enableInput();
void disableInput();

#endif // XSCANF_H