//Global variables ooops 
let mic = false, spec = [],
	j = 0,
	recording = false;
const sepFreq = 440; //separator tone
const validFreqs = [880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400, 4840, 5280, 5720, 6160, 6600]; //skal matche de freq vi sender ved (14 toner). Separator tone behøver IKKE inkluderes
const bitCombinations = ["000", "001", "010", "100", "011", "101", "110", "111",
	"00", "01", "10", "11",
	"0", "1"
]; //skal matche arrayet fra Tx

//-----------------
//Functions for comparing frequencies 
//and for converting frequencies to bits 
//-----------------

//This function compares the x to the min and the max value
//it returns true if x i within the limit, else false
function compare(x, target) {
	return x > target - 70 && x < target + 70;
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
	if (!mic) { //if start button is clicked, and mic was false before -> set mic to true
		getMedia(); //here spec will be filled with loudest freqs and audio is analyzed
		mic = true; 
	}
	recording = !recording;
	console.log("recording: " + recording); //console.log is recording true or false
	
	//empty array and string for holding the collected freqs and converting to bits
	let identifiedFreqs = [];
	let bitstring = '';
	
	//spec[] is now filled up with loudest freqs 
	//if not recording start by comparing spec[0]
	if (!recording) {
		console.log(spec); //console.log the spec array
		document.getElementById("spinspin").style.display = "none"; 
		let x = 0,
			current = 0,
			separator = false; //this boolean controls whether there has been a separator tone yet

		//step 1: skip all tones registered before the first separator tones have been registered multiple times:
		while (!((compare(spec[x], sepFreq) && compare(spec[x + 1], sepFreq))) && x < spec.length) {
			x++;
		}

		//step 2: identify information carrying frequencies in the "spec" array:
		while (x < spec.length) {
			//identify consecutive separator tones:
			while (compare(spec[x], sepFreq) && x < spec.length) {
				if (compare(spec[x + 1], sepFreq))
					separator = true; //if two separator tones are identified in a row, separator boolean is set to true
				x++;
			}

			if (!separator && x < spec.length) //if we reach this point, and separator is false, we skip the frequency
				x++;

			current = spec[x];

			// if the current frequency is valid and there has been a separator tone, we add the frequency to 
			// the array of identified (info carrying) frequencies. Then we skip all the frequencies similar to this freq:
			if (isValidFreq(current) && separator) { //if the freq is valid an the separator tone has been there
				separator = false;
				identifiedFreqs.push(isValidFreq(current)); //push to identified freqs
				bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
				while (!compare(spec[x], sepFreq) && x < spec.length) { //spring til næste separator
					x++;
				}
			}
		}
		console.log(identifiedFreqs);
		console.log("bitstring: " + bitstring);
		showImage(bitstring);
	}
	else
		document.getElementById("spinspin").style.display = "inline-block"; 
});
//when reset button clicked, the spec.length and j will be set to 0
document.getElementById("reset").addEventListener("click", () => {
	spec.length = 0;
	j = 0;
	x = 0;
	recording = false;
	console.log("reset!");
});

//--------------------
//FORKLAR ALT DET DER GETMEDIA
//--------------------
function getMedia() {
	navigator.mediaDevices.getUserMedia({ audio: true })
	.then(function (stream) {
		let audioContext = new AudioContext({ sampleRate: 384000 });
		let input = audioContext.createMediaStreamSource(stream);
		let analyser = audioContext.createAnalyser();
		let scriptProcessor = audioContext.createScriptProcessor(256, 1, 1);
		analyser.smoothingTimeConstant = 0.0;
		analyser.fftSize = 16384;

		input.connect(analyser);
		analyser.connect(scriptProcessor);
		scriptProcessor.connect(audioContext.destination);

		function onAudio() {
			let spectrum = new Uint8Array(analyser.frequencyBinCount);
			void analyser.getByteFrequencyData(spectrum);
			//console.log(spectrum);
			let loudestBin = 0;
      let specFrequency = 0;
			for (let i = 0; i < analyser.frequencyBinCount; i++) {
				if (spectrum[i] > spectrum[loudestBin]) {
          specFrequency = i * (audioContext.sampleRate / analyser.fftSize);
          if(specFrequency >= 400 && specFrequency <= 7000){
            loudestBin = i;
          }
				}
			}
			let frequency = loudestBin * (audioContext.sampleRate / analyser.fftSize);
			
			document.getElementById("spectogram").style.width = `${loudestBin/500*50}vw`;
			//if recording is true and the freq is either valid or a separation tone then push to spec
			if (recording && (isValidFreq(frequency) || compare(frequency, sepFreq))) {
				spec.push(frequency);
			}
		};
		scriptProcessor.addEventListener('audioprocess', onAudio);
	}).catch (error => {
		console.error(error.message, "- check if your browser allows access to a microphone");
	});
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
//Source:
//https://stackoverflow.com/questions/21354235/converting-binary-to-text-using-javascript
//https://stackoverflow.com/questions/1772941/how-can-i-insert-a-character-after-every-n-characters-in-javascript
/*

let binary = "";

let encodedData = btoa(binaryToString(binary.replace(/(.{8})/g, "$1 "))); //inside replace, it puts a space between every 8th character
//Btoa() is a method that encodes a string in base-64.
console.log(encodedData);

document.getElementById("img").src = "data:image/gif;base64," + encodedData;

//Before btoa can be used, the binary data needs to be converted to a string
function binaryToString(str) {
  let newBin = str.split(" ");
  let binCode = [];

  for (i = 0; i < newBin.length; i++) {
	binCode.push(String.fromCharCode(parseInt(newBin[i], 2)));
  }
  return binCode.join("");
}*/

//----------------------
/* document.getElementById("startpause").addEventListener("click", () => {
  recording = !recording;
  console.log(recording); //console.log is recording true or false
  console.log(spec); //console.log the spec array

  //empty array and string for holding the collected freqs and converting to bits
  let identifiedFreqs = [];
  let bitstring = '';

  //spec[] is now filled up with loudest freqs
  //if not recording start by comparing spec[0]
  if (!recording) {
	while (!compare(spec[0], 440)) { //sort out background noise before signal has started
	  spec.shift();
	  if (!(compare(spec[1],400,480) && compare(spec[2],400,480))) //if not the two following are separator tone:
		spec[1] = 0; //spec[1] will be set to 0, to avoid a false registration of separator tone if noise is equal to separator tone
	}
	while (spec.length > 1) {
	  while (!isValidFreq(spec[0])) {
		spec.shift();
	  }

	  let prev = spec[0]; //prev burde hedde current.. og er den første tone efter separation tone
	  if (isValidFreq(spec[0])) {
		identifiedFreqs.push(isValidFreq(spec[0]));
		bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
	  }
	  while (compare(spec[0], isValidFreq(prev) - 40, isValidFreq(prev) + 40)) {
		spec.shift();
	  }
	}
	console.log(identifiedFreqs);
	console.log(bitstring);
  }
});
//when reset button clicked, the spec.length and j will be set to 0
document.getElementById("reset").addEventListener("click", () => {
  spec.length = 0;
  j = 0;
  console.log("reset!");
});
*/