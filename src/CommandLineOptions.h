#include <string>
#include <vector>

#include "usdtweak_api.h"

class USDTWEAK_API CommandLineOptions {
  public:
    CommandLineOptions(int argc, char *const *argv);

    const std::vector<std::string> &stages() { return _stages; }

  private:
    std::vector<std::string> _stages;
};
