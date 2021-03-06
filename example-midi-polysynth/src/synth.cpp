
#include "synth.h"

void PolySynth::setup(int numVoices ){
    
    voices.resize( numVoices );
    
    for( Voice & voice : voices){
        voice.setup( *this );
    }   
    
    ui.setName("synth parameters");
    ui.add(pw.set("pulse width", 0.5f, 0.0f, 1.0f) );
    ui.add(pwm.set("pwm amount", 0.3f, 0.0f, 0.5f) );
    ui.add(pwmSpeed.set("pwm speed (hz)", 0.5f, 0.005f, 1.5f));
    cutoff.enableSmoothing(200.0f);
    ui.add(cutoff.set("cutoff pitch", 82, 20, 136));
    ui.add(reso.set("resonance", 0.0f, 0.0f, 1.0f) );
    ui.add(modAttack.set("mod env attack", 400, 0, 1200) );
    ui.add(modRelease.set("mod env release", 900, 0, 1200));    
    
}


void PolySynth::Voice::setup( PolySynth & ui ){

    addModuleInput("trig", voiceTrigger);
    addModuleInput("pitch", voicePitch);
    addModuleOutput("signal", voiceAmp);

    // SIGNAL PATH
    oscillator.out("pulse") * 4.0f >> saturator >> filter >> voiceAmp;
    
    // MODULATIONS AND CONTROL
                ui.pwm >> lfoAmt.in_mod();
    lfo.out_triangle() >> lfoAmt >> oscillator.in_pw();
                           ui.pw >> oscillator.in_pw();
                      voicePitch >> oscillator.in_pitch();
                                                             
                                                              ui.cutoff >> p2f;
     voiceTrigger >> (modEnv.set(400.0f, 600.0f, 1.0f, 800.0f) * 48.0f) >> p2f >> filter.in_cutoff(); 
                                                                       ui.reso >> filter.in_reso();
    
    (voiceTrigger >> (ampEnv.set(200.0f, 400.0f, 1.0f, 600.0f ) * 0.1f)) >> voiceAmp.in_mod();

       ui.modAttack >> modEnv.in_attack();
      ui.modRelease >> modEnv.in_release();
        ui.pwmSpeed >> lfo.in_freq();    
}


float PolySynth::Voice::meter_mod_env() const{
    return modEnv.meter_output();
}

float PolySynth::Voice::meter_pitch() const{
    return oscillator.meter_pitch();
}
