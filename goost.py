# Complete list of functional components used by this extension.
#
# Components can be disabled with build options matching `goost_*_enabled=no`.
# A branch of components can be disabled as well, like: `goost_core_enabled=no`.
#
# NOTE: Components may not necessarily have structural meaning.
#
components = [
    "core/image",
    "core/math",
    "scene/physics",
    "editor",
]


def get_components():
    comp = set()

    for n in components:
        parts = n.split("/")
        comp.update(parts)

    comp_list = list(comp)
    comp_list.sort()

    return comp_list


def get_child_components(parent):
    comp_list = []

    for n in components:
        parts = n.split("/")
        if not parent in parts:
            continue
        parts.reverse()
        for p in parts:
            if p == parent:
                break
            comp_list.append(p)

    return comp_list


#
# Complete list of all classes currently implemented in the extension,
# excluding any classes provided from within `modules/` directory.
#
# This is used by config.py::get_doc_classes(), and potentially allow to disable
# each of the class in the future.
#
classes = [
    "GoostEngine",
    "GoostGeometry2D",
    "GoostImage",
    "GradientTexture2D",
    "Grid2D",
    "ImageBlender",
    "ImageIndexed",
    "InvokeState",
    "LinkedList",
    "ListNode",
    "PolyBoolean2D",
    "PolyBooleanParameters2D",
    "PolyDecomp2D",
    "PolyDecompParameters2D",
    "PolyOffset2D",
    "PolyOffsetParameters2D",
    "PolyNode2D",
    "PolyCircle2D",
    "PolyRectangle2D",
    "PolyShape2D",
    "PolyCollisionShape2D",
    "Random",
    "Random2D",
    "ShapeCast2D",
    "VariantResource",
    "VisualShape2D",
]
# ClassA : Depends on ClassB.
# "GoostEngine" : "InvokeState"
# "PolyBoolean2D" : "PolyBooleanParameters2D"
# "PolyBoolean2D" : "PolyNode2D"
# "PolyDecomp2D" : "PolyDecompParameters2D"
# "PolyOffset2D" : "PolyOffsetParameters2D"
# "GoostGeometry2D" : "PolyBoolean2D"
# "GoostGeometry2D" : "PolyDecomp2D"
# "GoostGeometry2D" : "PolyOffset2D"
# "PolyCircle2D" : "PolyNode2D"
# "PolyRectangle2D" : "PolyNode2D"
# "PolyShape2D" : "PolyNode2D"
# "PolyCollisionShape2D" : "PolyNode2D"
