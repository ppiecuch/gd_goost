#include "register_core_types.h"

#include "core/engine.h"
#include "scene/main/scene_tree.h"

#include "goost_engine.h"
#include "invoke_state.h"

#include "image/register_image_types.h"
#include "math/register_math_types.h"
#include "types/grid_2d.h"
#include "types/list.h"
#include "types/variant_resource.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/editor_resource_preview.h"
#include "types/editor/variant_resource_preview.h"
#endif

namespace goost {
	
static GoostEngine *_goost = nullptr;
#ifdef TOOLS_ENABLED
static void _variant_resource_preview_init();
#endif

void register_core_types() {
	_goost = memnew(GoostEngine);
	ClassDB::register_class<GoostEngine>();
	Engine::get_singleton()->add_singleton(
			Engine::Singleton("GoostEngine", GoostEngine::get_singleton()));
	SceneTree::add_idle_callback(&GoostEngine::flush_calls);
	ClassDB::register_class<InvokeState>();

	ClassDB::register_class<Grid2D>();
	ClassDB::register_class<ListNode>();
	ClassDB::register_class<LinkedList>();

	ClassDB::register_class<VariantResource>();
#ifdef TOOLS_ENABLED
	EditorNode::add_init_callback(_variant_resource_preview_init);
#endif

#ifdef GOOST_IMAGE_ENABLED
	register_image_types();
#endif
#ifdef GOOST_MATH_ENABLED
	register_math_types();
#endif
}

void unregister_core_types() {
	if (_goost) { 
		memdelete(_goost);
	}
#ifdef GOOST_IMAGE_ENABLED
	unregister_image_types();
#endif
#ifdef GOOST_MATH_ENABLED
	unregister_math_types();
#endif
}

#ifdef TOOLS_ENABLED
void _variant_resource_preview_init() {
	Ref<VariantResourcePreviewGenerator> variant_resource_preview;
	variant_resource_preview.instance();
	EditorResourcePreview::get_singleton()->add_preview_generator(variant_resource_preview);
}
#endif

} // namespace goost
