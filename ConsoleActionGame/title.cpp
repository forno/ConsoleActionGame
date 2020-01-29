#include "updater.h"
#include "render.h"

status updater::operator()(state::title&)
{
  return state::gaming{};
}

void render::operator()(state::title&)
{
}
