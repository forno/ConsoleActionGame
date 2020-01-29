#include "updater.h"
#include "render.h"

status updater::operator()(state::result&)
{
  return state::title{};
}

void render::operator()(state::result&)
{
}
