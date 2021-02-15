/*************************************************************************/
/*  resource_importer_lottie.cpp                                         */
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

#include "video_stream_lottie.h"

#include "core/bind/core_bind.h"
#include "core/io/file_access_pack.h"
#include "scene/2d/animated_sprite.h"
#include "scene/2d/sprite.h"
#include "scene/3d/sprite_3d.h"
#include "scene/gui/video_player.h"

#include "thirdparty/rlottie/inc/rlottie.h"
#include "thirdparty/rlottie/inc/rlottiecommon.h"

String ResourceImporterVideoLottie::get_preset_name(int p_idx) const {
	return String();
}

void ResourceImporterVideoLottie::get_import_options(List<ImportOption> *r_options, int p_preset) const {
	r_options->push_back(ImportOption(PropertyInfo(Variant::VECTOR2, "scale"), Vector2(1.0f, 1.0f)));
}

bool ResourceImporterVideoLottie::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {
	return true;
}

String ResourceImporterVideoLottie::get_importer_name() const {
	return "packed_scene_lottie_video";
}

String ResourceImporterVideoLottie::get_visible_name() const {
	return "Packed Scene Lottie Video";
}

void ResourceImporterVideoLottie::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("json");
}

String ResourceImporterVideoLottie::get_save_extension() const {
	return "scn";
}

String ResourceImporterVideoLottie::get_resource_type() const {
	return "PackedScene";
}

int ResourceImporterVideoLottie::get_preset_count() const {
	return 0;
}

Error ResourceImporterVideoLottie::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	FileAccess *f = FileAccess::open(p_source_file, FileAccess::READ);
	if (!f) {
		return ERR_CANT_OPEN;
	}

	VideoStreamLottie *stream = memnew(VideoStreamLottie);

	Vector2 scale = p_options["scale"];
	Error err;
	String data = f->get_file_as_string(p_source_file, &err);
	ERR_FAIL_COND_V(err != OK, FAILED);
	stream->set_data(data);
	stream->set_scale(scale);
	f->close();
	memdelete(f);
	VideoPlayer *root = memnew(VideoPlayer);
    root->set_autoplay(true);
	root->set_stream(stream);
	Ref<PackedScene> scene;
	scene.instance();
	scene->pack(root);
	String save_path = p_save_path + ".scn";
	r_gen_files->push_back(save_path);
	return ResourceSaver::save(save_path, scene);
}

void VideoStreamLottie::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_data", "data"), &VideoStreamLottie::set_data);
	ClassDB::bind_method(D_METHOD("get_data"), &VideoStreamLottie::get_data);
	ClassDB::bind_method(D_METHOD("set_scale", "scale"), &VideoStreamLottie::set_scale);
	ClassDB::bind_method(D_METHOD("get_scale"), &VideoStreamLottie::get_scale);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "scale", PROPERTY_HINT_NONE), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "data", PROPERTY_HINT_NONE), "set_data", "get_data");
}

VideoStreamLottie::VideoStreamLottie() {}

Ref<VideoStreamPlayback> VideoStreamLottie::instance_playback() {
	Ref<VideoStreamPlaybackLottie> pb = memnew(VideoStreamPlaybackLottie);
	if (pb->open_data(data))
		return pb;
	return NULL;
}

void VideoStreamLottie::set_data(const String &p_file) {
	data = p_file;
}
String VideoStreamLottie::get_data() {
	return data;
}
void VideoStreamLottie::set_scale(Vector2 p_scale) {
	scale = p_scale;
}
Vector2 VideoStreamLottie::get_scale() const {
	return scale;
}
void VideoStreamLottie::set_audio_track(int p_track) {}