#include "updater.h"
#include "render.h"

state::gaming::gaming(input_manager& im)
{
  im.set_native(true);
}

status updater::operator()(state::gaming&)
{
  return state::result{ im };
}

void render::operator()(state::gaming&)
{
}
