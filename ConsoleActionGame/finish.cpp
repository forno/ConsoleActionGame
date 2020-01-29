#include "updater.h"
#include "render.h"

#include <cstdlib>

state::finish::finish(input_manager&) {}

status updater::operator()(const state::finish&)
{
  std::exit(EXIT_SUCCESS);
}

void render::operator()(const state::finish&)
{
  std::exit(EXIT_SUCCESS);
}
