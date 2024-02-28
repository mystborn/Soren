#include <soren_std.h>

E4C_DEFINE_EXCEPTION(NotImplementedException, "Function not implemented", RuntimeException);
E4C_DEFINE_EXCEPTION(SdlException, "SDL encountered an error", RuntimeException);