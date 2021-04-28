let mic, fft;
let spec = [];
let i = 0;

function setup() {
    mic = new p5.AudioIn();
    mic.start();
    fft = new p5.FFT(0.01, 8192);
    fft.setInput(mic);
    frameRate(240);
}

function draw() {
    var nyquist = sampleRate();
    var spectrum = fft.analyze(); // array of amplitudes in bins
    var numberOfBins = spectrum.length;
    var maxAmp = 0;
    var largestBin;
    
    for (var i = 0; i < numberOfBins; i++) {
        var thisAmp = spectrum[i]; // amplitude of current bin
        if (thisAmp > maxAmp) {
            maxAmp = thisAmp;
            largestBin = i;
        }
    }
    
    var loudestFreq = largestBin * (nyquist / numberOfBins);
    spec[i] = loudestFreq;
    console.log(loudestFreq);
}