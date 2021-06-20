/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "register_types.h"
#include "core/io/resource_importer.h"
#include "resource_importer_lottie.h"
#include "video_stream_lottie.h"
#include "editor/editor_node.h"

#ifdef TOOLS_ENABLED
static void _editor_init() {
	Ref<ResourceImporterLottie> lottie_sprite_animation;
	lottie_sprite_animation.instantiate();
	ResourceFormatImporter::get_singleton()->add_importer(lottie_sprite_animation);
	Ref<ResourceImporterVideoLottie> lottie_video_animation;
	lottie_video_animation.instantiate();
	ResourceFormatImporter::get_singleton()->add_importer(lottie_video_animation);
}
#endif
void register_lottie_types() {
#ifdef TOOLS_ENABLED
	ClassDB::APIType prev_api = ClassDB::get_current_api();
	ClassDB::set_current_api(ClassDB::API_EDITOR);
	ClassDB::register_class<VideoStreamLottie>();
	ClassDB::register_class<ResourceImporterLottie>();
	ClassDB::set_current_api(prev_api);
	EditorNode::add_init_callback(_editor_init);
#endif
}

void unregister_lottie_types() {
}
