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

#include "core/core_bind.h"
#include "core/io/file_access_pack.h"
#include "scene/2d/animated_sprite_2d.h"
#include "scene/2d/sprite_2d.h"
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
	if (pb->open_data(data)) {
		return pb;
	}
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

VideoStreamPlaybackLottie::VideoStreamPlaybackLottie() {
}

VideoStreamPlaybackLottie::~VideoStreamPlaybackLottie() {
}

bool VideoStreamPlaybackLottie::open_data(const String &p_file) {
	if (p_file.is_empty()) {
		return false;
	}
	data = p_file;
	lottie = rlottie::Animation::loadFromData(p_file.utf8().ptrw(), p_file.md5_text().utf8().ptrw());
	size_t width = 0;
	size_t height = 0;
	lottie->size(width, height);
	video_frames_pos = 0;
	video_frames.resize(1);
	return true;
}

void VideoStreamPlaybackLottie::stop() {
	if (playing) {
		open_data(data); //Should not fail here...
		video_frames_capacity = video_frames_pos = 0;
		num_decoded_samples = 0;
		video_frame_delay = video_pos = 0.0;
	}
	time = 0.0;
	playing = false;
}

void VideoStreamPlaybackLottie::play() {
	stop();
	delay_compensation = ProjectSettings::get_singleton()->get("audio/video_delay_compensation_ms");
	delay_compensation /= 1000.0;
	playing = true;
}

bool VideoStreamPlaybackLottie::is_playing() const {
	return playing;
}

void VideoStreamPlaybackLottie::set_paused(bool p_paused) {
	paused = p_paused;
}

bool VideoStreamPlaybackLottie::is_paused() const {
	return paused;
}

void VideoStreamPlaybackLottie::set_loop(bool p_enable) {
	loop = p_enable;
}

bool VideoStreamPlaybackLottie::has_loop() const {
	return loop;
}

float VideoStreamPlaybackLottie::get_length() const {
	return lottie->totalFrame() / lottie->frameRate();
}

float VideoStreamPlaybackLottie::get_playback_position() const {
	return video_pos;
}

void VideoStreamPlaybackLottie::seek(float p_time) {
	time = p_time;
}

void VideoStreamPlaybackLottie::set_audio_track(int p_idx) {
}

Ref<Texture2D> VideoStreamPlaybackLottie::get_texture() const {
	return texture;
}

bool VideoStreamPlaybackLottie::has_enough_video_frames() const {
	if (video_frames_pos > 0) {
		// FIXME: AudioServer output latency was fixed in af9bb0e, previously it used to
		// systematically return 0. Now that it gives a proper latency, it broke this
		// code where the delay compensation likely never really worked.
		//const double audio_delay = AudioServer::get_singleton()->get_output_latency();
		const double video_time = (video_frames_pos - 1) / lottie->frameRate();
		return video_time >= time + /* audio_delay + */ delay_compensation;
	}
	return false;
}

bool VideoStreamPlaybackLottie::should_process() {
	// FIXME: AudioServer output latency was fixed in af9bb0e, previously it used to
	// systematically return 0. Now that it gives a proper latency, it broke this
	// code where the delay compensation likely never really worked.
	//const double audio_delay = AudioServer::get_singleton()->get_output_latency();
	return get_playback_position() >= time + /* audio_delay + */ delay_compensation;
}

void VideoStreamPlaybackLottie::update(float p_delta) {
	if ((!playing || paused))
		return;

	time += p_delta;

	if (time < video_pos) {
		return;
	}
	video_frames_pos = time / lottie->totalFrame();
	if (loop && video_frames_pos >= lottie->totalFrame()) {
		set_paused(true);
		seek(0.0f);
		play();
		return;
	}

	bool video_frame_done = false;
	size_t width = 0;
	size_t height = 0;
	lottie->size(width, height);
	ERR_FAIL_COND(!video_frames.size());
	while (!has_enough_video_frames() ||
			video_frames_pos == 0 && video_frames_pos < lottie->totalFrame()) {
		++video_frames_capacity;
		video_frames.resize(video_frames_capacity);
		ERR_FAIL_INDEX(video_frames.size() - 1, video_frames.size());
		video_frames[video_frames.size() - 1].resize(width * height);
		++video_frames_pos;
	}
	while (video_frames_pos > 0 && !video_frame_done) {
		ERR_FAIL_INDEX(video_frames.size() - 1, video_frames.size());
		LocalVector<uint32_t> video_frame = video_frames.ptr()[video_frames.size() - 1];
		if (should_process()) {
			rlottie::Surface surface(video_frame.ptr(), width, height, width * sizeof(uint32_t));
			lottie->renderSync(video_frames_pos, surface);
			LocalVector<uint8_t> frame_data;
			int32_t buffer_byte_size = video_frame.size() * sizeof(uint32_t);
			frame_data.resize(buffer_byte_size);
			memcpy(frame_data.ptr(), video_frame.ptr(), buffer_byte_size);
			for (int32_t pixel_i = 0; pixel_i < frame_data.size(); pixel_i += 4) {
				SWAP(frame_data.ptr()[pixel_i + 2], frame_data.ptr()[pixel_i + 0]);
			}
			Ref<Image> img = memnew(Image(width, height, 0, Image::FORMAT_RGBA8, frame_data));
			texture->create_from_image(img); //Zero copy send to visual server
			video_frame_done = true;
		}
		video_pos = video_frames_pos / lottie->frameRate();
		ERR_FAIL_INDEX(video_frames_pos - 1, video_frames.size());
		memmove(video_frames.ptr(), video_frames.ptr() + 1, (--video_frames_pos) * sizeof(void *));
		video_frames[video_frames_pos] = video_frame;
	}
	if (video_frames_pos == 0 && video_frames_pos >= lottie->totalFrame()) {
		stop();
	}
}

void VideoStreamPlaybackLottie::set_mix_callback(AudioMixCallback p_callback, void *p_userdata) {
	mix_callback = p_callback;
	mix_udata = p_userdata;
}

int VideoStreamPlaybackLottie::get_channels() const {
	return 0;
}

int VideoStreamPlaybackLottie::get_mix_rate() const {
	return 0;
}
