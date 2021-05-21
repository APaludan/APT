const validFreqs = [880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400, 4840, 5280, 5720, 6160, 6600]; //skal matche de freq vi sender ved (14 toner). Separator tone behøver IKKE inkluderes
const bitCombinations = [ "000", "001", "010", "100", "011", "101", "110", "111",
                          "00", "01", "10", "11",
                          "0", "1"]; //skal matche arrayet fra Tx

//case 1(perf), sep,val,sep,val.....                          
//case 2(no wakeuptone), val,sep,val,sep.....
//case 3(missing seperator) sep,val,val,sep.....
//case 4(single valid noise) sep,valN,val,sep....
//case 5(3 * valid in row), sep, val, val, val, sep.....
//case 6(valN before sep), sep,val,valN,sep....
let case1 = [440, 440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case2 = [2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case3 = [440, 440, 440, 2200, 2200, 2200, 2200, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case4 = [440, 440, 440, 880, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case5 = [440, 440, 440, 2200, 2200, 2200, 2200, 1320, 1320, 1320, 1320, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case6 = [440, 440, 440, 2200, 2200, 2200, 2200, 880, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];


function compare(x, target) { 
    return x > target - 40 && x < target + 40;
  }
  
  //This function checks whether the detected sound has a valid frequency
  //Should be within the validFreqs array (with a +/-40 margin)
  //Returns false or the valid frequency
  function isValidFreq(freq) {
    for (let i = 0; i < validFreqs.length; i++) {
      if (compare(freq, validFreqs[i])) {
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
        return bitCombinations[i];
      }
    }
    return 'XXX';
  }
  
  //Record/stop -> identify the different frequencies 
  
  document.getElementById("startpause").addEventListener("click", () => {
    recording = !recording;
    console.log("recording: " + recording); //console.log is recording true or false
    console.log(spec); //console.log the spec array
  
    //empty array and string for holding the collected freqs and converting to bits
    let identifiedFreqs = [];
    let bitstring = '';
  
    //spec[] is now filled up with loudest freqs 
    //if not recording start by comparing spec[0]
    if (!recording) {
      let x = 0, current = 0;
      let separator = false;
      console.log(spec);
      while (!((compare(spec[x], 440) && compare(spec[x + 1], 440))) && x < spec.length) { //sort out background noise before signal has started
        //while the frequency we are looking at, and the next frequency aren't just noise, skip and continue
        x++;
      }
      console.log("x: ", x);
      while (x < spec.length) { 
        while (compare(spec[x], 440) && x < spec.length) { 
          if (compare(spec[x+1], 440))
            separator = true; //while spec[x] is between 400 and 480 then it is a separatortone
          x++;
        }
        if (!separator)
          x++;
  
        current = spec[x];
        if (isValidFreq(spec[x]) && separator) { //if the freq is valid an the separator tone has been there
          separator = false;
          identifiedFreqs.push(isValidFreq(spec[x])); //push to identified freqs
          bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
          //console.log("x: " + x + "freq: " + identifiedFreqs[identifiedFreqs.length-1]);
          while (!compare(spec[x], 440) && x < spec.length) { //spring til næste separator
            x++;
          }
        }
      }
      console.log(identifiedFreqs);
      console.log("bitstring: " + bitstring);
      showImage(bitstring);
    }
  });
  //when reset button clicked, the spec.length and j will be set to 0
  document.getElementById("reset").addEventListener("click", () => {
    spec.length = 0;
    j = 0;
    x = 0;
    recording = false;
    console.log("reset!");
  });
  
  
  //-----------------
  //Functions for drawing and analyzing the sound
  //Source p5: https://p5js.org/
  //https://p5js.org/examples/sound-frequency-spectrum.html 
  //-----------------
  
  //a p5.js function, that runs only once (when the page loads)
  function setup() {
    createCanvas(200,200); //creates canvas, dimensions: 1000x600 pixels
    noFill();                //disables filling geometry
  
    //Gets audio from fx. computer's microphone
    //mic.start() starts the recording
    mic = new p5.AudioIn(); 
    mic.start();
  
    //FFT (fast fourier transform) isolates individual audio frequencies within a waveform 
    fft = new p5.FFT(0.0, 2048);
    fft.setInput(mic); //Sets the input source for the FFT analysis
  }
  
  //This function draws the "spectrogram",
  //it runs every frame (framerate = 60 or 144 times per sec)
  function draw() {
    background(0);  //background color
    stroke(255,255,255);
  
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
    //calculates the loudest frequency once per frame (e.g., 60 or 144 times pr second)
    //Samplerate returns the samples per sec, samplerate = sampling frequency
    let loudestFreq = largestBin * (sampleRate() / numberOfBins) / 2; 
    //we take the largestBin which contains the index with the larges value,
    //so to find this value u take the sample rate and divide it with 2 to get the frequencies humans can hear. 
    //Then we divide it with the number of bins to get the frequencies in the loudestBin.
   
    if (recording == true && (isValidFreq(loudestFreq) || compare(loudestFreq, 440))) {
      //if recording is true and the freq is either valid or a separation tone then add to spec[j]
      spec[j] = loudestFreq;
      j++;
    }
  }
  
  //This function draws what the fft has analyzed
  function drawSpectrum(spectrum) {
    //begin- and endshape begins and stops recording vertices for a shape
    beginShape();
    for (i = 0; i < spectrum.length; i++) {
      vertex(i*(width/spectrum.length), map(spectrum[i], 0, 255, height, 0)); //i = x-coordinate of vertex, map(..) = y-coordinate 
      //map(value, start1_cur, stop1_cur, start2_targ, stop2_targ) it re-maps a number from one range to another
    }
    endShape();
  }
  
  
  
  //------------
  //For converting the binary data back to an image
  //-------------
  function showImage(bitstring) {
    let encodedData = btoa(binaryToString(bitstring.replace(/(.{8})/g, "$1 ")));
    //Btoa() is a method that encodes a string in base-64.
    console.log(encodedData);
  
    document.getElementById("img").src = "data:image/bmp;base64," + encodedData;
  
    //Before btoa can be used, the binary data needs to be converted to a string
    function binaryToString(str) {
      let newBin = str.split(" ");
      let binCode = [];
  
      for (i = 0; i < newBin.length; i++) {
        binCode.push(String.fromCharCode(parseInt(newBin[i], 2)));
      }
      return binCode.join("");
    }
  }
