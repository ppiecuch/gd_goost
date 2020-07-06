#include "register_types.h"

#include "core/register_core_types.h"
#include "scene/register_scene_types.h"
#include "editor/register_editor_types.h"

void register_gd_goost_types() {
#ifdef GOOST_CORE_ENABLED
	goost::register_core_types();
#endif
#ifdef GOOST_SCENE_ENABLED
	goost::register_scene_types();
#endif
#if defined(TOOLS_ENABLED) && defined(GOOST_EDITOR_ENABLED)
	goost::register_editor_types();
#endif
}

void unregister_gd_goost_types() {
#ifdef GOOST_CORE_ENABLED
	goost::unregister_core_types();
#endif
#ifdef GOOST_SCENE_ENABLED
	goost::unregister_scene_types();
#endif
}
