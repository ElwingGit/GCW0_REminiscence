/* REminiscence - Flashback interpreter
 * Copyright (C) 2005-2011 Gregory Montoir
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MIXER_H__
#define MIXER_H__

#include "intern.h"

struct MixerChunk {
	uint8 *data;
	uint32 len;

	MixerChunk()
		: data(0), len(0) {
	}

	int8 getPCM(int offset) const {
		if (offset < 0) {
			offset = 0;
		} else if (offset >= (int)len) {
			offset = len - 1;
		}
		return (int8)data[offset];
	}
};

struct MixerChannel {
	uint8 active;
	uint8 volume;
	MixerChunk chunk;
	uint32 chunkPos;
	uint32 chunkInc;
};

struct SystemStub;

struct Mixer {
	typedef bool (*PremixHook)(void *userData, int8 *buf, int len);

	enum {
		NUM_CHANNELS = 4,
		FRAC_BITS = 12,
		MAX_VOLUME = 64
	};

	SystemStub *_stub;
	MixerChannel _channels[NUM_CHANNELS];
	PremixHook _premixHook;
	void *_premixHookData;

	Mixer(SystemStub *stub);
	void init();
	void free();
	void setPremixHook(PremixHook premixHook, void *userData);
	void play(const MixerChunk *mc, uint16 freq, uint8 volume);
	void stopAll();
	uint32 getSampleRate() const;
	void mix(int8 *buf, int len);

	static void addclamp(int8 &a, int b);
	static void mixCallback(void *param, uint8 *buf, int len);
};

template <class T>
int resampleLinear(T *sample, int pos, int step, int fracBits) {
	const int inputPos = pos >> fracBits;
	const int inputFrac = pos & ((1 << fracBits) - 1);
	int out = sample->getPCM(inputPos);
	out += (sample->getPCM(inputPos + 1) - out) * inputFrac >> fracBits;
	return out;
}

#endif // MIXER_H__
