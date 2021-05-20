/*************************************************************************/
/*  resource_importer_lottie.h	                                         */
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
#ifndef RESOURCE_IMPORTER_VIDEO_LOTTIE
#define RESOURCE_IMPORTER_VIDEO_LOTTIE

#include "core/bind/core_bind.h"
#include "core/io/file_access_pack.h"
#include "core/io/resource_importer.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "scene/2d/animated_sprite.h"
#include "scene/2d/sprite.h"
#include "scene/3d/mesh_instance.h"
#include "scene/3d/spatial.h"
#include "scene/3d/sprite_3d.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/primitive_meshes.h"
#include "scene/resources/video_stream.h"

#include "thirdparty/rlottie/inc/rlottie.h"
#include "thirdparty/rlottie/inc/rlottiecommon.h"
class VideoStreamPlaybackLottie;
class VideoStreamLottie : public VideoStream {

	GDCLASS(VideoStreamLottie, VideoStream);

	String data;
	Vector2 scale = Vector2(1.0f, 1.0f);

protected:
	static void _bind_methods();

public:
	VideoStreamLottie();
	virtual Ref<VideoStreamPlayback> instance_playback();
	virtual void set_data(const String &p_file);
	String get_data();
	void set_scale(Vector2 p_scale);
	Vector2 get_scale() const;
	virtual void set_audio_track(int p_track);
};

class VideoStreamPlaybackLottie : public VideoStreamPlayback {

	GDCLASS(VideoStreamPlaybackLottie, VideoStreamPlayback);

	String data;

	int video_frames_pos, video_frames_capacity = 0;

	Vector<Vector<uint32_t> > video_frames;
	int num_decoded_samples, samples_offse = 0;
	AudioMixCallback mix_callback;
	void *mix_udata = nullptr;

	bool playing = false;
	bool paused = false;
	bool loop = true;
	double delay_compensation = 0.0;
	double time, video_frame_delay, video_pos = 0.0;

	PoolVector<uint8_t> frame_data;
	Ref<ImageTexture> texture = memnew(ImageTexture);

	std::unique_ptr<rlottie::Animation> lottie = nullptr;

public:
	VideoStreamPlaybackLottie();
	~VideoStreamPlaybackLottie();

	bool open_data(const String &p_file);

	virtual void stop();
	virtual void play();

	virtual bool is_playing() const;
	virtual void set_paused(bool p_paused);
	virtual bool is_paused() const;
	virtual void set_loop(bool p_enable);
	virtual bool has_loop() const;
	virtual float get_length() const;

	virtual float get_playback_position() const;
	virtual void seek(float p_time);

	virtual void set_audio_track(int p_idx);

	virtual Ref<Texture> get_texture() const;
	bool has_enough_video_frames() const;
	bool should_process();
	virtual void update(float p_delta);

	virtual void set_mix_callback(AudioMixCallback p_callback, void *p_userdata);
	virtual int get_channels() const;
	virtual int get_mix_rate() const;
};

class ResourceImporterVideoLottie : public ResourceImporter {
	GDCLASS(ResourceImporterVideoLottie, ResourceImporter);

public:
	virtual String get_importer_name() const;
	virtual String get_visible_name() const;
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual String get_save_extension() const;
	virtual String get_resource_type() const;

	virtual int get_preset_count() const;
	virtual String get_preset_name(int p_idx) const;

	virtual void get_import_options(List<ImportOption> *r_options,
			int p_preset = 0) const;
	virtual bool
	get_option_visibility(const String &p_option,
			const Map<StringName, Variant> &p_options) const;
	virtual Error import(const String &p_source_file, const String &p_save_path,
			const Map<StringName, Variant> &p_options,
			List<String> *r_platform_variants,
			List<String> *r_gen_files = NULL,
			Variant *r_metadata = NULL);

	ResourceImporterVideoLottie() {}
	~ResourceImporterVideoLottie() {}
};

#endif // RESOURCE_IMPORTER_VIDEO_LOTTIE
