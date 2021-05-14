//Global variables ooops 
let mic, fft, spec = [],  j = 0, recording = false;
const validFreqs = [440, 880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400]; //skal matche de freq vi sender ved (14 eller 15)
const bits = ['', '010', '000', '100', '001', '110', '101', '', '011', '111']; //skal matche arrayet fra Tx


//-----------------
//Functions for comparing frequencies 
//and for converting frequencies to bits 
//-----------------

//This function compares the x to the min and the max value
//it returns true if x i within the limit, else false
function compare(x, min, max) {
  if (x > min && x < max) {
    return true;
  }
  else
    return false;
}

//This function checks whether the detected sound has a valid frequency
//Should be within the validFreqs array (with a +/-40 margin)
//Returns false or the valid frequency
function isValidFreq(freq) {
  for (let i = 0; i < validFreqs.length; i++) {
    if (compare(freq, validFreqs[i] - 40, validFreqs[i] + 40)) {
      return validFreqs[i];
    }
  }
  return false;
}

//This function uses the validfreqs and bits constant
//to convert the frequency into bits (1 bit combination corresponds to a frequency)
//returns the bit if true
function freqToBits(freq) {
  for (let i = 0; i < validFreqs.length; i++) {
    if (validFreqs[i] === freq) {
      return bits[i];
    }
  }
  return 'nej';
}

//MANGLER 
document.getElementById("startpause").addEventListener("click", () => {
  recording = !recording;
  console.log(recording); //console.log is recording true or false
  console.log(spec); //console.log the spec array

  //empty array and string for holding the collected freqs and converting to bits
  let identifiedFreqs = [];
  let bitstring = '';
  
  //spec[] is now filled up with loudest freqs 
  //if not recording start by comparing spec[0] **
  if (!recording) {
    while (!compare(spec[0], 400, 480)) { //sort out background noise before signal has started
      spec.shift();
    }
    while (spec.length > 1) {
      while (!isValidFreq(spec[0])) {
        spec.shift();
      }
      while (compare(spec[0], 400, 480)) {
        spec.shift();
      }

      let prev = spec[0]; //prev burde hedde current.. og er den første tone efter separation tone
      if (isValidFreq(spec[0])) {
        identifiedFreqs.push(isValidFreq(spec[0]));
        bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
      }
      while (compare(spec[0], prev - 40, prev + 40)) {
        spec.shift();
      }
    }
    console.log(identifiedFreqs);
    console.log(bitstring);
  }
});
//when reset button is being clicked on, the spec.length and j will be set to 0
document.getElementById("reset").addEventListener("click", () => {
  spec.length = 0;
  j = 0;
  console.log("reset!");
});


//-----------------
//Functions for drawing and analyzing the sound
//Source p5: https://p5js.org/
//https://p5js.org/examples/sound-frequency-spectrum.html 
//-----------------

//a p5.js function, that runs only once (when the page loads)
function setup() {
  createCanvas(1000, 600); //creates canvas, dimensions: 1000x600 pixels
  noFill();                //disables filling geometry

  //Gets audio from fx. computer's microphone
  //mic.start() starts the recording
  mic = new p5.AudioIn(); 
  mic.start();

  //FFT (fast fourier transform) isolates individual audio frequencies within a waveform 
  fft = new p5.FFT();
  fft.setInput(mic); //Sets the input source for the FFT analysis
}

//This function draws the "spectrogram",
//it runs every frame (framerate = 60 times per sec)
function draw() {
  background(200);  //background color

  let spectrum = fft.analyze(); //returns array of amplitudes referred to as bins
  drawSpectrum(spectrum);       
  let numberOfBins = spectrum.length;  //length of spectrum array is the number of bins in total
  let maxAmp = 0;
  let largestBin;

  //finds the bin that holds the max amplitude
  for (let i = 0; i < numberOfBins; i++) {
    let thisAmp = spectrum[i]; //amplitude of current bin
    if (thisAmp > maxAmp) {
      maxAmp = thisAmp; 
      largestBin = i;
    }
  }
  //calculates the loudest frequency
  //Samplerate returns the samples per sec, samplerate = sampling frequency
  let loudestFreq = largestBin * (sampleRate() / numberOfBins) / 2;  
  if (recording == true) {
    spec[j] = loudestFreq;
    j++;
  }
}

//This function draws what the fft has analyzed
function drawSpectrum(spectrum) {
  //begin- and endshape begins and stops recording vertices for a shape
  beginShape();
  for (i = 0; i < spectrum.length; i++) {
    vertex(i, map(spectrum[i], 0, 255, height, 0)); //i = x-coordinate of vertex, map(..) = y-coordinate 
    //map(value, start1_cur, stop1_cur, start2_targ, stop2_targ) it re-maps a number from one range to another
  }
  endShape();
}



//------------
//For converting the binary data back to an image
//-------------

//MANGLER
let test = "";

let encodedData = btoa(binaryToString(test.replace(/(.{8})/g, "$1 ")));
console.log(encodedData);

document.getElementById("img").src = "data:image/gif;base64," + encodedData;

function binaryToString(str) {
  let newBin = str.split(" ");
  let binCode = [];

  for (i = 0; i < newBin.length; i++) {
    binCode.push(String.fromCharCode(parseInt(newBin[i], 2)));
  }
  return binCode.join("");
}