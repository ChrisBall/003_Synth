#define MIDIMAX 127
#define MIDIFraction 0.007874015748031496 //=1/127

byte voiceIndex = 0;

float logLookup[128] = {
  0.0, 0.0016164734, 0.0032457253, 0.004887959, 0.006543384, 0.008212213, 0.009894666, 0.011590966,
  0.013301344, 0.015026034, 0.016765278, 0.018519325, 0.02028843, 0.02207285, 0.023872854, 0.025688719,
  0.027520726, 0.02936916, 0.031234328, 0.033116523, 0.03501607, 0.036933288, 0.038868506, 0.04082207,
  0.042794324, 0.04478564, 0.04679638, 0.04882693, 0.050877687, 0.052949052, 0.05504145, 0.057155307,
  0.059291072, 0.0614492, 0.06363016, 0.065834455, 0.06806257, 0.07031505, 0.072592415, 0.074895225,
  0.07722405, 0.0795795, 0.081962176, 0.08437272, 0.086811796, 0.089280084, 0.091778286, 0.09430714,
  0.09686741, 0.099459894, 0.1020854, 0.10474478, 0.10743892, 0.110168755, 0.11293523, 0.115739346,
  0.118582144, 0.1214647, 0.124388136, 0.12735364, 0.13036245, 0.13341582, 0.13651513, 0.13966174,
  0.14285715, 0.14610288, 0.14940053, 0.1527518, 0.15615849, 0.15962242, 0.16314557, 0.16672999,
  0.17037787, 0.17409146, 0.17787321, 0.18172565, 0.18565147, 0.18965353, 0.19373482, 0.1978986,
  0.20214821, 0.20648731, 0.21091972, 0.21544954, 0.22008118, 0.22481933, 0.22966895, 0.23463544,
  0.23972455, 0.24494253, 0.2502961, 0.25579238, 0.26143926, 0.2672453, 0.2732196, 0.27937227,
  0.2857143, 0.29225767, 0.2990156, 0.3060027, 0.31323498, 0.32073036, 0.32850862, 0.33659196,
  0.34500536, 0.35377687, 0.3629383, 0.37252608, 0.38258168, 0.39315322, 0.40429643, 0.41607675,
  0.42857143, 0.44187278, 0.45609215, 0.47136578, 0.4878625, 0.5057955, 0.52543885, 0.5471536,
  0.5714286, 0.5989493, 0.63071966, 0.668296, 0.71428573, 0.7735768, 0.85714287, 1.0
};

void onNoteOn(byte channel, byte note, byte velocity) {
  Serial.print(channel);
  Serial.print('\t');
  Serial.print(note);
  Serial.print('\t');
  Serial.println(velocity);
  vm.onNoteOn(note, velocity);
}

void onNoteOff(byte channel, byte note, byte velocity) {
  Serial.print(channel);
  Serial.print('\t');
  Serial.print(note);
  Serial.print('\t');
  Serial.println(velocity);
  vm.onNoteOff(note, velocity);
}

void onProgramChange(byte channel, byte program) {
  Serial.print(channel);
  Serial.print('\t');
  Serial.println(program);

  if (channel == 3) { //load
    loadPreset(program);
  } else if (channel == 16) { //save
    savePreset(program);
  }
}

void onControlChange(byte channel, byte control, byte value) {
  Serial.print(channel);
  Serial.print('\t');
  Serial.print(control);
  Serial.print('\t');
  Serial.println(value);

  if (control == 64) {
    //accept sustain messages on all channels
    vm.setSustain(value);
    return;
  }

  if (channel != 3 || control >= NUM_CONTROLS || value > MIDIMAX) { //OOR value handling
    return;
  }

  controlValues[control] = value; //store value

  switch (control) {
    case 0:
      //glide time in mono mode
      vm.setGlideTime(logLookup[value] * 4000);
      break;
    case 1:
      //poly/mono/mont cont. mode
      vm.setMonoMode(value / 43);
      break;
    case 2:
      //simple waveform type
      vm.setWaveform1Type(value >> 5);
      break;
    case 3:
      //simple waveform type
      vm.setWaveform2Type(value >> 5);
      break;

    case 4:
      //arp mode
      vm.setArpMode(value / 22);
      break;
    case 5:
      //arp rate
      vm.setArpRate(0.25 + float(value) * MIDIFraction * 15.75);
      break;
    case 6:
      //det osc1
      vm.setWaveform1Detune(0.125 * pow(-1 + 2 * float(value) * MIDIFraction, 3));
      break;
    case 7:
      //det osc2
      vm.setWaveform2Detune(0.125 * pow(-1 + 2 * float(value) * MIDIFraction, 3));
      break;

    case 8:
      //LFO/vib amp
      vm.setLFOAmplitude(0.125 * pow(logLookup[value], 2));
      break;
    case 9:
      //LFO/vib freq
      vm.setLFOFrequency(40 * pow(logLookup[value], 2));
      break;
    case 10:
      //osc1 gain
      vm.setWaveform1Gain(float(value) * MIDIFraction * .5);
      break;
    case 11:
      //osc2 gain
      vm.setWaveform2Gain(float(value) * MIDIFraction * .5);
      break;

    case 12:
      //VCF TYPE
      vm.setFilterMix(2 * float(value) * MIDIFraction);
      break;
    case 13:
      //VCF Q
      vm.setFilterQ(0.7315 + 4 * float(value) * MIDIFraction);
      break;
    case 14:
      //VCF MAX
      vm.setFilterMax(logLookup[value]);
      break;
    case 15:
      //VCF amount
      vm.setFilterAmount(float(value) * MIDIFraction);
      break;

    case 16:
      //voice ADSR attack
      vm.setADSRAttack(5 + logLookup[value] * 3000); //5 to 3005 ms
      break;
    case 17:
      //voice ADSR decay
      vm.setADSRDecay(5 + logLookup[value] * 3000); //5 to 3005
      break;
    case 18:
      //voice ADSR sustain
      vm.setADSRSustain(float(value) * MIDIFraction);
      break;
    case 19:
      //voice ADSR release
      vm.setADSRRelease(5 + logLookup[value] * 3000); //5 to 3005
      break;

    case 20:
      //ext input to fx chain
      fx.input.gain(1, float(value) * MIDIFraction);
      break;
    case 21:
      //synth input to fx chain
      fx.input.gain(0, float(value) * MIDIFraction);
      break;
    case 22:
      fx.tremoloLFO.frequency(float(value) * MIDIFraction * 20);
      break;
    case 23:
      fx.tremoloLFO.amplitude(0.001 + float(value) * MIDIFraction * 0.499); //avoiding amplitude of zero (or the module switches off)
      fx.tremoloLFO.offset(1 - float(value) * MIDIFraction * 0.5);
      break;

    case 24:
      //dx dist
      fx.waveshaper1.shape(waveShapes[value], WAVESHAPE_SIZE);
      break;
    case 25:
      //dx lvl
      fx.waveshaper1Gain.gain(1 * float(value) * MIDIFraction);
      break;
    case 26:
      //bitcrusher bits
      fx.bitCrusher1.bits(map(value, MIDIMAX, 0, 1, 16));
      break;
    case 27:
      //bitcrusher sample rate
      fx.bitCrusher1.sampleRate(map(value, MIDIMAX, 0, 1, 44100));
      break;

    case 28:
      //granular speed/pitchshift
      fx.granular1.beginFreeze(map(value, 0, MIDIMAX, 1, 1470));
      fx.granular1.beginPitchShift(map(value, 0, MIDIMAX, 1, 1470));
      fx.granular1.setSpeed(float(value) * MIDIFraction);
      break;
    case 29:
      //granular mix
      fx.granularMixer1.gain(0, 1 - float(value) * MIDIFraction);
      fx.granularMixer1.gain(1, float(value) * MIDIFraction);
      break;
    case 30:
      //ring mod freq
      fx.ringSine.frequency(logLookup[value] * 4000);
      break;
    case 31:
      //ring mod mix
      fx.ringMixer.gain(0, float(value) * MIDIFraction);
      fx.ringMixer.gain(1, 1 - float(value) * MIDIFraction);
      break;

    case 32:
      //tape delay mix
      fx.tapeInMix.gain(float(value) * MIDIFraction);
      fx.tapeBypassMix.gain(1 - float(value) * MIDIFraction);
      break;
    case 33:
      //tape delay LP cutoff
      fx.tapeDelayFilter1.setLowpass(0, 400 + 19600 * logLookup[value], 0.707);
      //fx.tapeDelayFilter1.frequency(50+float(value)*100); //need to log scale, also filter may be better as FIR
      //fx.tapeDelayFilter1.begin(filterCoeffs[value],8);
      break;
    case 34:
      //tape delay time
      fx.tapeDelay1.setSpeed(logLookup[value] * 16);
      break;
    case 35:
      //tape delay feedback
      fx.tapeDelayMixer1.gain(1, 1.25 * float(value) * MIDIFraction);
      break;

    case 36:
      //filter type
      if (2 * float(value) * MIDIFraction <= 1) {
        fx.filterTypeMixer.gain(0, 1 - 2 * float(value) * MIDIFraction);
        fx.filterTypeMixer.gain(1, 2 * float(value) * MIDIFraction);
        fx.filterTypeMixer.gain(2, 0);
      } else if (2 * float(value) * MIDIFraction <= 2) {
        fx.filterTypeMixer.gain(0, 0);
        fx.filterTypeMixer.gain(1, 2 - 2 * float(value) * MIDIFraction);
        fx.filterTypeMixer.gain(2, 2 * float(value) * MIDIFraction - 1);
      }
      break;
    case 37:
      //filter Q
      fx.filter.resonance(0.7 + 4 * float(value) * MIDIFraction);
      break;
    case 38:
      //filter cutoff
      fx.filter.frequency(10 + 15000 * logLookup[value]);
      break;
    case 39:
      //filter mix
      fx.filterMixer.gain(0, 1 - float(value) * MIDIFraction);
      fx.filterMixer.gain(1, float(value) * MIDIFraction);
      break;

    case 40:
      //dx2 dist
      fx.waveshaper2.shape(waveShapes[value], WAVESHAPE_SIZE);
      break;
    case 41:
      //dx2 lvl
      fx.waveshaper2Gain.gain(1 * float(value) * MIDIFraction);
      break;
    case 42:
      //reverb length
      fx.reverb.size(float(value) * MIDIFraction);
      fx.reverb.lodamp(1 - float(value) * MIDIFraction);
      fx.reverb.hidamp(1 - float(value) * MIDIFraction);
      break;
    case 43:
      //reverb mix
      fx.reverbMixer.gain(0, 1 - float(value) * MIDIFraction);
      fx.reverbMixer.gain(1, float(value) * MIDIFraction);
      break;

    case 44:
      //master out
      masterLevel.amplitude(float(value) * MIDIFraction,50); 
      break;
    case 45:
      //clean ext
      outputMixer.gain(1, float(value) * MIDIFraction);
      break;
    case 46:
      //clean synth
      outputMixer.gain(2, float(value) * MIDIFraction);
      break;
    case 47:
      //fx synth/ext
      outputMixer.gain(3, float(value) * MIDIFraction);
      break;
  }
}
