#include <string>
#include <sstream>

#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)

#define STR(...) static_cast<std::stringstream &&>(std::stringstream() << __VA_ARGS__).str()

#define MARK_AS_UNUSED(ARG_NAME) ((void)(ARG_NAME))
