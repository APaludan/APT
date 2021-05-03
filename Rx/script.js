let mic, fft;
let spec = [];
let j = 0;
var recording = false;

document.getElementById("startpause").addEventListener("click", () => {
    recording = !recording;
    console.log(recording);
    console.log(spec);
});
document.getElementById("reset").addEventListener("click", () => {
    spec.length = 0;
    j = 0;
    console.log(spec);
});

// runs once on page load
function setup() {
    createCanvas(1000, 600);
    noFill();

    mic = new p5.AudioIn();
    mic.start();
    fft = new p5.FFT(0.01, 8192);
    fft.setInput(mic);
    frameRate(1000);
}

// runs every frame - 60 times per second
function draw() {
    background(200);
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

    var loudestFreq = largestBin * (sampleRate() / numberOfBins);
    if (recording == true) {
        spec[j] = loudestFreq;
        j++;
    }
    drawSpectrum();
}
function drawSpectrum(){
    let spectrum = fft.analyze();

    beginShape();
    for (i = 0; i < spectrum.length; i++) {
      vertex(i, map(spectrum[i], 0, 255, height, 0));
    }
    endShape();
}