let mic, fft;
let spec = [];
let j = 0;
var recording = false;
const validFreqs = [440, 880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400];
const bits = ['', '010', '000', '100', '001', '110', '101', '', '011', '111'];

function compare(x, min, max) {
    if (x > min && x < max) {
        return true;
    }
    else
        return false;
}

function isValidIndex(freq) {
    for (let i = 0; i < validFreqs.length; i++) {
        if (compare(freq, validFreqs[i] - 40, validFreqs[i] + 40)) {
            return validFreqs[i];
        }
    }
    return false;
}

function freqToBits (freq) {
    for (let i = 0; i < validFreqs.length; i++) {
        if (validFreqs[i] === freq) {
            return bits[i];
        }
    }
    return 'nej';
}


document.getElementById("startpause").addEventListener("click", () => {
    recording = !recording;
    console.log(recording);
    console.log(spec);
    let identifiedFreqs = [];
    let bitstring = '';
    if (!recording) {
        while (spec.length > 0) {
            console.log(spec);
            console.log(identifiedFreqs);
            console.log("0: " + spec.length);
            while (!compare(spec[0], 400, 480)) {
                console.log("1: " + spec.length);
                spec.shift();
            }
            while (compare(spec[0], 400, 480)) {
                console.log("2: " + spec.length + " spec0: " + spec[0] + " spec1: " + spec[1]);
                spec.shift();
            }
            if (isValidIndex(spec[0])) {
                console.log("3: " + spec.length);
                identifiedFreqs.push(isValidIndex(spec[0]));
                bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
            }
        }
        console.log(identifiedFreqs);
        console.log(bitstring);
    }
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
    fft = new p5.FFT();
    fft.setInput(mic);
}
44000
0 - 22000

// runs every frame - 60 times per second
function draw() {
    background(200);
    var spectrum = fft.analyze(); // array of amplitudes in bins
    drawSpectrum(spectrum);
    var numberOfBins = spectrum.length;
    var maxAmp = 0;
    var largestBin;

    for (let i = 0; i < numberOfBins; i++) {
        let thisAmp = spectrum[i]; // amplitude of current bin
        if (thisAmp > maxAmp) {
            maxAmp = thisAmp;
            largestBin = i;
        }
    }

    var loudestFreq = largestBin * (sampleRate() / numberOfBins) / 2;
    if (recording == true) {
        spec[j] = loudestFreq;
        j++;
    }

}
function drawSpectrum(spectrum) {
    //let spectrum = fft.analyze();

    beginShape();
    for (i = 0; i < spectrum.length; i++) {
        vertex(i, map(spectrum[i], 0, 255, height, 0));
    }
    endShape();
}