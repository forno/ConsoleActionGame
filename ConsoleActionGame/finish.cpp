#include "updater.h"
#include "render.h"

#include <cstdlib>

status updater::operator()(state::finish&)
{
  std::exit(EXIT_SUCCESS);
}

void render::operator()(state::finish&)
{
  std::exit(EXIT_SUCCESS);
}
