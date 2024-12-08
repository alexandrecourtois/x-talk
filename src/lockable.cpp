#include <lockable.h>

std::vector<std::unique_ptr<std::recursive_mutex>> Lockable::__mutexes;