#include "updater.h"
#include "render.h"

status updater::operator()(state::gaming&)
{
  return state::result{};
}

void render::operator()(state::gaming&)
{
}
