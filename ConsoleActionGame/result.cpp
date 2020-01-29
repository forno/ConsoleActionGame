#include "updater.h"
#include "render.h"

state::result::result(input_manager& im)
  : score{}
{
  im.set_native(false);
}

status updater::operator()(state::result&)
{
  return state::title{ im };
}

void render::operator()(state::result&)
{
}
