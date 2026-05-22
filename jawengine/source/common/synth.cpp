// ABC parser and digital synthesizer
// https://abcnotation.com/wiki/abc:standard:v2.1

// TODO
// triplets, 1st/2nd endings, P: field support, octave option on V: and K:, broken rhythm, accidentals carry through bar
// and probably other things...

#include "../../JawEngine.h" // sound.h & JAW_DBGPRINT
#include "../../source/common/internal_sound.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>	//remove_if
#include <cstdio>	//sscanf
#include <cmath>

constexpr float PI = 3.1415926535f;

struct ABCVoice {
	std::vector<int16_t> samples{};
	std::vector<size_t> repeatStack;
	float(*waveform)(float){};
	int16_t amplitude{};
};

enum Accidental : int { 
	DBLFLAT = -2, FLAT = -1, NAT = 0, SHARP = 1, DBLSHARP = 2, EXPLICIT_NAT
};

// https://abcnotation.com/wiki/abc:standard:v2.1#information_fields
struct InfoFields {
	// Tune title
	// Required to be second, after X, in the tune header
	// Not allowed in file header
	std::string T;

	// Meter
	// Computed as a ratio
	// 0 means free meter
	struct Meter {
		unsigned num=0, denom=0;
		float div() { return (float)num/denom; }
	} M{};

	// Unit note length
	// { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 }
	// 0 means unspecified, will be calculated based on M as per the standard
	unsigned L = 0;

	// Tempo
	// Not allowed in file header
	struct Tempo {
		unsigned bpm = 0;
		unsigned base = 0;
	} Q;

	// Key
	// Not allowed in file header, final field of tune header
	struct Key {
		union {
			struct { Accidental a, b, c, d, e, f, g; };
			Accidental accidentals[7]{};
		};

		Accidental get(char c) const {
			c = toupper(c);
			if (c < 'A' || c > 'G') return Accidental::NAT;
			else return accidentals[c - 'A'];
		}
	} K;
};

static std::vector<void*> allocations;

static float square(float x) {
	if (x <= PI) return 1;
	else return -1;
	/*
		// Fourier series form
		constexpr size_t NUM_HARMONICS = 20;
		float ret = 0;
		for (size_t i = 0; i < NUM_HARMONICS; i++) {
			float n = 1 + 2*(float)i;
			ret += sin(n * x) / n;
		}
		return ret * 0.7f;
	*/
}

static float triangle(float x) {
	// Triangle that has f(0)=0 and f(2pi)=0
	constexpr float AMP = 2/PI;
	if (x < PI/2) {
		return AMP * x;
	}
	else if (x < 3*PI/2) {
		return AMP * (PI - x);
	}
	else return AMP * (x - 2*PI);
	/*
		// Fourier series form
		constexpr size_t NUM_HARMONICS = 20;
		float ret = 0;
		for (size_t i = 0; i < NUM_HARMONICS; i++) {
			float n = 1 + 2*(float)i;
			float sign = (i & 1) ? 1.f : -1.f;
			ret += sign * sin(n * x) / (n*n);
		}
		return ret * 0.7f;
	*/
}

static float sawtooth(float x) {
	return (x - PI) / PI;
	/*
		// Fourier series form
		constexpr size_t NUM_HARMONICS = 20;
		float ret = 0;
		for (size_t i = 1; i < NUM_HARMONICS; i++) {
			float sign = (i & 1) ? 1.f : -1.f;
			ret += sign * sin(i * x) / i;
		}
		return ret * 0.7f;
	*/
}

static int16_t *mix(ABCVoice *voices, size_t numVoices, size_t *numSamples, float lowpass, float gain) {
	// Figure out which voice's samples are the longest
	size_t longest = 0;
	for (size_t i = 0; i < numVoices; i++) {
		size_t x = voices[i].samples.size();
		if (x > longest) longest = x;
	}

	// Allocate enough to fit the longest
	*numSamples = longest * 2;	// mono to stereo
	void *ptr = malloc(*numSamples * sizeof(int16_t));
	allocations.push_back(ptr);
	int16_t *ret = (int16_t*)ptr;
	if (!ret) return nullptr;

	// Using a low-pass filter to smooth out the sound of these harsh samples
	// https://kiritchatterjee.wordpress.com/2014/11/10/a-simple-digital-low-pass-filter-in-c/
	double smoothed = 0;

	for (size_t i = 0; i < longest; i++) {
		double sum = 0;
		for (size_t j = 0; j < numVoices; j++) {
			auto &vec = voices[j].samples;
			if (i >= vec.size()) continue;
			else sum += (double)vec[i];
		}
		sum /= sqrt((double)numVoices);
		smoothed = smoothed - (lowpass * (smoothed - sum));
		int16_t sample = (int16_t)(smoothed * gain);
		ret[2*i] = ret[2*i+1] = sample;
	}

	return ret;
}

// freq=0 indicates silence
// length is given in seconds
// waveform should be a funciton like math.h's sin(),
// ie it should repeat every 2pi, and return a value between approx. -1 and 1
static void writeSample(std::vector<int16_t> *vec, float length, float freq, int16_t amplitude, float(*waveform)(float)) {
	constexpr unsigned SAMPLE_RATE = 44100;

	// The most important thing is to write the exact correct number of samples for the given length of time
	// that way we're guaranteed that two voices of the same length will have the same number of samples
	unsigned totalSamples = (unsigned)(length * SAMPLE_RATE);

	// if freq=0, just write zeros and return
	if (freq == 0.f) {
		for (unsigned i = 0; i < totalSamples; i++) {
			vec->push_back(0);
		}
		return;
	}
	
	// Here only if freq != 0
	float samplesPerCycle = SAMPLE_RATE / freq;

	// Write audio data
	float phase = 0;
	float step = (2*PI) / samplesPerCycle;
	for (unsigned i = 0; i < totalSamples; i++) {
		int16_t sample = (int16_t)(amplitude * waveform(phase));

		// Apply a simple fade to the first and last samples
		constexpr unsigned FADE_SIZE = 200;
		if (totalSamples-i <= FADE_SIZE) {
			sample = (int16_t)((float)sample * (totalSamples-i) / FADE_SIZE);
		}
		else if (i < FADE_SIZE) {
			sample = (int16_t)((float)sample * i / FADE_SIZE);
		}

		vec->push_back(sample);

		phase += step;
		if (phase > 2*PI) phase -= 2*PI;
	}
}

/*
	End of synthesizer, begin parser
*/

static std::vector<ABCVoice> voices;
static std::unordered_map<std::string, size_t> voiceIndices;
static size_t numVoices;
static size_t currentVoice;

static void pushRepeat() {
	ABCVoice &v = voices[currentVoice];
	v.repeatStack.push_back(v.samples.size());
}

static void popRepeat() {
	ABCVoice &v = voices[currentVoice];
	size_t i;
	if (v.repeatStack.empty()) {
		i = 0;
	}
	else {
		i = v.repeatStack.back();
		v.repeatStack.pop_back();
	}
	v.samples.insert(v.samples.end(), v.samples.begin()+i, v.samples.end());
}

static unsigned parseL(std::string &s) {
	unsigned x;
	int n = sscanf(s.c_str(), "L:1/%u", &x);
	if (n < 1) {
		JAW_DBGPRINT("Malformed abc: unable to parse L field");
		return 0;
	}

	constexpr unsigned acceptable[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
	bool good = false;
	for (unsigned i : acceptable) if (x == i) good = true;
	if (!good) {
		JAW_DBGPRINT("Malformed abc: unacceptable value in L field");
		return 0;
	}

	return x;
}

static InfoFields::Meter parseM(std::string &s) {
	// Check for free meter
	if (s.size() >= 6 && s.compare(0, 6, "M:none") == 0) {
		return {};
	}

	// Check for common time or cut time
	if (s.size() >= 3 && s.compare(0, 3, "M:C") == 0) {
		if (s.size() >= 4 && s[3] == '|') return { 2, 2 };
		else return { 4, 4 };
	}

	// Otherwise, look at the "fraction"
	unsigned numerator, denominator;
	int n = sscanf(s.c_str(), "M:%u/%u", &numerator, &denominator);
	if (n != 2) {
		JAW_DBGPRINT("Malformed abc: unable to parse M field");
		return {};
	}

	return { numerator, denominator };
}

static std::string parseT(std::string &s) {
	return s.substr(2);
}

static InfoFields::Tempo parseQ(std::string &s) {
	InfoFields::Tempo tempo;
	int n = sscanf(s.c_str(), "%*[^1]1/%u=%u", &tempo.base, &tempo.bpm);
	if (n < 2) {
		JAW_DBGPRINT("Malformed abc: unable to parse Q field");
		return {};
	}
	return tempo;
}

// We currently only support major key signatures
static InfoFields::Key parseK(std::string &s) {
	constexpr InfoFields::Key KEYS_LUT[15] = {
		{.accidentals = {FLAT,  FLAT,  FLAT,  FLAT,  FLAT,  FLAT,  FLAT }}, // Cb
		{.accidentals = {FLAT,  FLAT,  FLAT,  FLAT,  FLAT,  NAT,   FLAT }}, // Gb
		{.accidentals = {FLAT,  FLAT,  NAT,   FLAT,  FLAT,  NAT,   FLAT }}, // Db
		{.accidentals = {FLAT,  FLAT,  NAT,   FLAT,  FLAT,  NAT,   NAT  }}, // Ab
		{.accidentals = {FLAT,  FLAT,  NAT,   NAT,   FLAT,  NAT,   NAT  }}, // Eb
		{.accidentals = {NAT,   FLAT,  NAT,   NAT,   FLAT,  NAT,   NAT  }}, // Bb
		{.accidentals = {NAT,   FLAT,  NAT,   NAT,   NAT,   NAT,   NAT  }}, // F
		{.accidentals = {NAT,   NAT,   NAT,   NAT,   NAT,   NAT,   NAT  }}, // C
		{.accidentals = {NAT,   NAT,   NAT,   NAT,   NAT,   SHARP, NAT  }}, // G
		{.accidentals = {NAT,   NAT,   SHARP, NAT,   NAT,   SHARP, NAT  }}, // D
		{.accidentals = {NAT,   NAT,   SHARP, NAT,   NAT,   SHARP, SHARP}}, // A
		{.accidentals = {NAT,   NAT,   SHARP, SHARP, NAT,   SHARP, SHARP}}, // E
		{.accidentals = {SHARP, NAT,   SHARP, SHARP, NAT,   SHARP, SHARP}}, // B
		{.accidentals = {SHARP, NAT,   SHARP, SHARP, SHARP, SHARP, SHARP}}, // F#
		{.accidentals = {SHARP, SHARP, SHARP, SHARP, SHARP, SHARP, SHARP}}, // C#
	};

	constexpr int NOTE_LUT[7] = {
		10, 12, 7, 9, 11, 6, 8
	};

	char keyLetter = s.size() > 2 ? toupper(s[2]) : 0;
	if (keyLetter < 'A' || keyLetter > 'G') {
		JAW_DBGPRINT("Malformed abc: unable to parse K field");
		return {};
	}
	int keyIndex = NOTE_LUT[keyLetter - 'A'];

	char keyAcc = s.size() > 3 ? s[3] : 0;
	if (keyAcc == '#') keyIndex += 7;
	else if (keyAcc == 'b') keyIndex -= 7;

	if (keyIndex < 0 || keyIndex >= 15) {
		JAW_DBGPRINT("Malformed abc: unable to parse K field; invalid key");
		return {};
	}

	return KEYS_LUT[keyIndex];
}

static void parseV(std::string &s) {
	static const char *WHITESPACE = " \n\t\v\f\r";

	// Locate the label
	size_t begin = s.find(':')+1;
	size_t end = s.find_first_of(WHITESPACE, begin);
	if (end == std::string::npos) end = s.size();
	std::string label = s.substr(begin, end-begin);

	// Check if this voice already exists, and create it if it doesn't
	size_t index;
	if (voiceIndices.contains(label)) {
		index = voiceIndices[label];
	}
	else {
		index = numVoices++;
		voices.push_back({});
		voiceIndices[label] = index;
	}
	ABCVoice &voice = voices[index];
	currentVoice = index;

	size_t eq = 0;
	while ((eq = s.find('=', eq+1)) != std::string::npos) {
		end = s.find(' ', eq);
		if (end == std::string::npos) end = s.size();
		std::string option = s.substr(eq+1, end-eq-1);

		if (eq >= 12 && s.compare(eq-12, 12, "jaw-waveform") == 0) {
			if (option == "sine") voice.waveform = sinf;
			else if (option == "square") voice.waveform = square;
			else if (option == "triangle") voice.waveform = triangle;
			else if (option == "sawtooth") voice.waveform = sawtooth;
			else JAW_DBGPRINT("Malformed abc: unrecognized waveform option " << option);
		}
		else if (eq >= 13 && s.compare(eq-13, 13, "jaw-amplitude") == 0) {
			int a = std::stol(option);
			voice.amplitude = (int16_t)std::clamp(a, 0, 32767);
		}
		else JAW_DBGPRINT("Malformed abc: unsupported V property; ignored.");
	}
}

// Returns the data from file header info fields (if applicable)
// Leaves line holding the first line of the tune header, and the dataStream pointing to the next
static bool parseFileHeader(std::stringstream &dataStream, std::string &line, InfoFields *fields) {
	unsigned fieldCount = 0;

	do {
		// Remove all whitespace
		line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

		// Blank line marks the end, return what we've got thus far
		if (line.empty()) {
			// Consume the blank line
			std::getline(dataStream, line);
			return true;
		}

		// Ignore comments/directives
		if (line[0] == '%') continue;

		// Confirm this line contains a field
		if (line.size() < 2 || line[1] != ':') {
			JAW_DBGPRINT("Malformed abc: only comments/directives and fields are allowed in the file header");
			return false;
		}

		// Parse the field
		switch (line[0]) {
			// If we encounter X: it's either:
			// 1) The first field in the file, in which case the file does not contain a file header, or
			// 2) Incorrectly placed.
			// In the case of incorrect placement, this parser assumes that X: marks the beginning of the tune header.
			// However, it outputs a warning as standard ABC requires a blank line to terminate the file header.
		case 'X':
			if (fieldCount != 0) {
				JAW_DBGPRINT("Malformed abc: the file header contains an X: field. "
					"Assuming there should've been a blank line and this begins the first tune header");
			}
			return true;

			// These fields are explicitly not allowed in the file header.
		case 'K':
		case 'P':
		case 'Q':
		case 's':
		case 'T':
		case 'V':
		case 'W':
		case 'w':
			JAW_DBGPRINT("Malformed abc: " << line[0] << " is not allowed in the file header; ignored.");
			break;

		case 'L':
			fields->L = parseL(line);
			break;

		case 'M':
			fields->M = parseM(line);
			break;

		default:
			// Don't care about other file header fields, such as C or O
			break;
		}

		fieldCount++;
	} while (std::getline(dataStream, line));

	JAW_DBGPRINT("Malformed abc: unexpected eof in file header");
	return false;
}

// Returns the data from tune header info fields
// Leaves line holding the first line of the tune body, and the dataStream pointing to the next
static bool parseTuneHeader(std::stringstream &dataStream, std::string &line, InfoFields *fields) {
	unsigned fieldCount = 0;

	do {
		// Remove all whitespace
		std::string origLine = line;
		line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

		// Ignore comments/directives
		if (line[0] == '%') continue;

		// Confirm this line contains a field
		if (line.size() < 2 || line[1] != ':') {
			JAW_DBGPRINT("Malformed abc: only comments/directives and fields are allowed in the tune header");
			return false;
		}
		
		// Parse the field
		switch (line[0]) {
			// X must be the first field in the tune header
		case 'X':
			if (fieldCount != 0) {
				JAW_DBGPRINT("Malformed abc: the tune header's first field must be X:");
				return false;
			}
			break;

			// T must be the second field
		case 'T':
			if (fieldCount != 1) {
				JAW_DBGPRINT("Malformed abc: the tune header's second field must be T:");
				return false;
			}
			fields->T = parseT(line);
			break;

			// K is the final field, so parsing the tune header ends here
		case 'K':
			fields->K = parseK(line);
			std::getline(dataStream, line);	// Consume the K: field to point to the first line of the tune body
			return true;

			// These fields are explicitly not allowed in the tune header
		case 's':
		case 'w':
			JAW_DBGPRINT("Malformed abc: " << line[0] << " is not allowed in the tune header; ignored");
			break;

		case 'M':
			fields->M = parseM(line);
			break;

		case 'L':
			fields->L = parseL(line);
			break;

		case 'Q':
			fields->Q = parseQ(line);
			break;

		case 'V':
			parseV(origLine);	// use the line still with whitespace
			break;

		default:
			break;
		}

		fieldCount++;
	} while (std::getline(dataStream, line));

	JAW_DBGPRINT("Malformed abc: unexpected eof in tune header");
	return false;
}

static void completeHeader(InfoFields *fields) {
	// M defaults to zero in the struct, which indicates free meter; correct
	// K defaults to all naturals in the struct (C Major); correct

	// If L was not specified, compute it based on M as per the standard
	if (fields->L == 0) {
		if (fields->M.num == 0.f)
			fields->L = 8;
		else if (fields->M.div() < 0.75f)
			fields->L = 16;
		else
			fields->L = 8;
	}

	// If Q was not specified, default to 100 default note lengths per minute
	if (fields->Q.base == 0 && fields->Q.bpm == 0) {
		fields->Q.base = fields->L;
		fields->Q.bpm = 100;
	}
}

static float getFreq(char note, float octave, InfoFields::Key &key, Accidental accidental, const sound::ABCOptions &opt) {
	// Increase octave if lowercase, and convert to uppercase
	octave *= toupper(note) == note ? 1 : 2;
	note = toupper(note);

	if (note == 'Z' || note == 'X') return 0; // 0 indicates silence
	if (note < 'A' || note > 'G') {
		JAW_DBGPRINT("UNREACHABLE");
		return 0;
	}

	constexpr int NOTE_LUT[7] = {
		9, 11, 0, 2, 4, 5, 7
	};
	int freqIndex = NOTE_LUT[note - 'A'];

	Accidental acc;
	if (accidental == NAT)
		acc = key.get(note);
	else if (accidental == EXPLICIT_NAT)
		acc = NAT;
	else
		acc = accidental;

	freqIndex += acc;
	while (freqIndex < 0) {
		freqIndex += 12;
		octave /= 2.f;
	}
	while (freqIndex >= 12) {
		freqIndex -= 12;
		octave *= 2.f;
	}

	return opt.freqs[freqIndex] * octave;
}

enum BarType {
	NORMAL, OPEN_REPEAT, CLOSE_REPEAT, DBL_REPEAT
};
static BarType classifyBar(std::string &line, size_t begin, size_t end) {
	if (end-begin == 1) return NORMAL;

	if (line[begin] == ':' && line[end-1] == ':') {
		return DBL_REPEAT;
	}
	else if (line[begin] == ':') {
		return CLOSE_REPEAT;
	}
	else if (line[end-1] == ':') {
		return OPEN_REPEAT;
	}
	else return NORMAL;
}

// Currently don't support fields within the tune body
static bool parseTuneBody(std::stringstream &dataStream, std::string &line, InfoFields *fields, const sound::ABCOptions &opt) {
	static const char *NOTES = "ABCDEFGabcdefgZzx";
	static const char *BARLINES = ":[]|";
	static const char *PREFIXES = ".LH^=_";
	static const char *POSTFIXES = ",\'";

	const auto processNote = [&](size_t index) -> float {
		// Here we have a note to synthesize:
		float spb = 1.f / (fields->Q.bpm/60.f);
		float len = spb * ((float)fields->Q.base / fields->L);
		float octave = 1;
		bool staccato = false;
		float amp = 1;
		Accidental acc = NAT;

		// Process any prefixes
		size_t prefix = line.find_last_not_of(PREFIXES, index-1);
		if (prefix == std::string::npos) prefix = -1;
		for (size_t i = prefix+1; i < index; i++) {
			switch (line[i]) {
			case '.':
				staccato = true;
				break;

			case 'L':
				amp = 1.2f;	// Accent makes the note 20% louder
				break;

			case 'H':
				len *= 2;	// Fermata doubles note length
				break;

			case '^':
				if (acc == EXPLICIT_NAT) break;
				acc = (Accidental)(acc+1);
				if (acc > DBLSHARP) acc = DBLSHARP;
				break;

			case '_':
				if (acc == EXPLICIT_NAT) break;
				acc = (Accidental)(acc-1);
				if (acc < DBLFLAT) acc = DBLFLAT;
				break;

			case '=':
				acc = EXPLICIT_NAT;
				break;

			default:
				JAW_DBGPRINT("UNREACHABLE");
				break;
			}
		}

		// Process any postfixes
		size_t postfix = line.find_first_not_of(POSTFIXES, index+1);
		if (postfix == std::string::npos) postfix = line.size();
		for (size_t i = index+1; i < postfix; i++) {
			switch (line[i]) {
			case ',':
				octave /= 2;
				break;

			case '\'':
				octave *= 2;
				break;

			default:
				JAW_DBGPRINT("UNREACHABLE");
				break;
			}
		}

		// Process length modifier
		unsigned numerator = 1;
		unsigned denominator = 1;
		unsigned numSlashes = 0;
		int numExists = sscanf(line.c_str()+postfix, "%u", &numerator);	// Read the numerator

		size_t nextNote = line.find_first_of(NOTES, postfix);	// Find the next note
		if (nextNote == std::string::npos) nextNote = line.size();

		size_t firstSlash = line.find_first_of('/', postfix);	// Find the next slash
		if (firstSlash == std::string::npos) firstSlash = line.size();

		if (firstSlash < nextNote) {	// If a slash occurs before the next note, count how many
			for (size_t i = firstSlash; i < line.size() && line[i] == '/'; i++)
				numSlashes++;
		}

		int denomExists = 0;
		if (numSlashes == 1) {
			// Attempt to read the denominator only if there's one slash
			denomExists = sscanf(line.c_str()+firstSlash+1, "%u", &denominator);
		}

		// Possible length modifier cases
		float numUnits = 0;
		if (numSlashes == 0) {	// Simple length multiplier, (could be one, ie not there)
			numUnits = (float)numerator;
		}
		else if (numSlashes > 0 && !numExists && !denomExists) { // Slash division shorthand
			numUnits = (float)1 / (1 << numSlashes);
		}
		else if (numSlashes == 1 && numExists && denomExists) {	// Arbitrary fraction
			numUnits = (float)numerator/denominator;
		}
		else {
			JAW_DBGPRINT("Malformed abc: failed to parse note length");
		}
		len *= numUnits;

		// Synthesize the sound
		ABCVoice &v = voices[currentVoice];
		float freq = getFreq(line[index], octave, fields->K, acc, opt);
		
		if (staccato) {	// Staccato halves note length and fills the rest with silence
			writeSample(&v.samples, len/2, freq, (int16_t)(amp*v.amplitude), v.waveform);
			writeSample(&v.samples, len/2, 0, 0, nullptr);
		}
		else {
			writeSample(&v.samples, len, freq, (int16_t)(amp*v.amplitude), v.waveform);
		}

		// Return the length of the note in unit beats
		return numUnits;
	};

	const auto processBar = [&](size_t begin) {
		size_t end = line.find_first_not_of(BARLINES, begin+1);
		if (end == std::string::npos) end = line.size();
		BarType type = classifyBar(line, begin, end);
		switch (type) {
		case NORMAL:
			break;

		case OPEN_REPEAT:
			pushRepeat();
			break;

		case CLOSE_REPEAT:
			popRepeat();
			break;

		case DBL_REPEAT:
			popRepeat();
			pushRepeat();
			break;

		default:
			JAW_DBGPRINT("UNREACHABLE");
		}
	};
 
	do {
		// Remove all whitespace
		std::string origLine = line;
		line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

		// Blank line marks the end
		if (line.empty()) {
			return true;
		}

		// Ignore comments/directives
		if (line[0] == '%') continue;

		// Check if this line has an information field
		// Extra check to exclude lines that begin with a barline
		if (line.size() >= 2 && line[1] == ':' && line[0] != '|') {
			switch (line[0]) {
			case 'K':
				fields->K = parseK(line);
				break;

			case 'L':
				fields->L = parseL(line);
				break;

			case 'M':
				fields->M = parseM(line);
				break;

			case 'Q':
				fields->Q = parseQ(line);
				break;

			case 'V':
				parseV(origLine);
				break;

				// These are explicitly not allowed in the tune body
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'F':
			case 'G':
			case 'H':
			case 'O':
			case 'S':
			case 'X':
			case 'Z':
				JAW_DBGPRINT("Malformed abc: " << line[0] << " is not allowed in the tune body; ignored");
				break;

			default:
				break;
			}

			// Finished processing info field, move to next line
			continue;
		}

		// Parsing a line of music code
		size_t noteIndex = line.find_first_of(NOTES);
		size_t barIndex = line.find_first_of(BARLINES);
		float noteCount = 0;

		while (noteIndex != std::string::npos) {
			// Process notes up to the next barline
			if (noteIndex < barIndex) {
				noteCount += processNote(noteIndex);
				noteIndex = line.find_first_of(NOTES, noteIndex+1);
				continue;
			}

			// End of bar, make sure we have the right number if we aren't in free meter
			if (noteCount != 0 && fields->M.denom != 0 && fields->M.div() != noteCount / fields->L) {	
				JAW_DBGPRINT("Malformed abc: incorrect number of beats (" << noteCount << ") in bar: " << line);
				return false;
			}

			processBar(barIndex);

			barIndex = line.find_first_of(BARLINES, noteIndex+1);
			noteCount = 0;
		}

		// Handle trailing barlines
		if (barIndex != std::string::npos) processBar(barIndex);
	} while (std::getline(dataStream, line));

	// Okay if we reach EOF here, EOF can mark the end of the tune body
	return true;
}

static int16_t *parseABC(const char *abcData, size_t *numSamples, const sound::ABCOptions &opt) {
	// set up voices (the default voice is used if no other is specified with a V: field)
	numVoices = 0;
	voices.clear();
	voiceIndices.clear();
	size_t n = numVoices++;
	voices.push_back({ .waveform = square, .amplitude = 10000 });
	voiceIndices["default"] = n;
	
	std::stringstream dataStream(abcData);
	std::string line;

	// The first line of an abc file should begin "%abc"
	std::getline(dataStream, line);
	if (line.size() < 4 || line.compare(0, 4, "%abc") != 0) {
		JAW_DBGPRINT("Missing %abc at top of file. Is this abc data?");
		return nullptr;
	}

	InfoFields fileHeader{};
	bool success = parseFileHeader(dataStream, line, &fileHeader);
	if (!success) return nullptr;

	// At this point we have the file header data parsed,
	// and the "line" string points to the first line of the tune header, which should be the X field

	// The file header provides default options, but the tune header can override them
	InfoFields tuneHeader = fileHeader;
	success = parseTuneHeader(dataStream, line, &tuneHeader);
	if (!success) return nullptr;

	// We're done parsing headers, but the data might be incomplete thus far
	// For example, if the L field was not specified, it is implied based on M
	completeHeader(&tuneHeader);

	// At this point we're done with the headers, and we're on to the tune body
	success = parseTuneBody(dataStream, line, &tuneHeader, opt);
	if (!success) return nullptr;

	// Now each of the voices are filled with sample data
	// Mix them and return the result
	return mix(&voices[0], numVoices, numSamples, opt.lowpass, opt.masterGain);
}

jaw::soundid sound::abc(const char *abcData, const sound::ABCOptions &opt) {
	size_t numSamples;
	int16_t *samples = parseABC(abcData, &numSamples, opt);
	if (!samples) return jaw::INVALID_ID;
	jaw::soundid id = sound::create();
	if (!sound::write(id, samples, numSamples, opt.loop)) return jaw::INVALID_ID;
	else return id;
}

void sound::deinitSynth() {
	for (void *x : allocations) free(x);
	allocations.clear();
}