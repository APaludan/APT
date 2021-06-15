//Global variables ooops 
let mic = false, spec = [],
	j = 0,
	recording = false;
const sepFreq = 440; //separator tone
const validFreqs = [880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400, 4840, 5280, 5720, 6160, 6600, 7040, 7480]; //skal matche de freq vi sender ved (14 toner). Separator tone behøver IKKE inkluderes
const bitCombinations = ["0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
	"1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"]; //skal matche arrayet fra Tx


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
		document.getElementById("spinspin").style.display = "none"; //if not recording dont show the spin thing
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
		document.getElementById("spinspin").style.display = "inline-block"; //if recording the spin thing is shown
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
//GetMedia runs when the mic is recording!
//The web audio api is used to do this (instead of p5)
//--------------------
function getMedia() {
	navigator.mediaDevices.getUserMedia({ audio: true }) //get access to users audio, takes an object as parameter (audio true and/or video true)
	.then(function (stream) { //if promise is resolved then execute line 121 to 154, stream is a result of the promise getUserMedia
		//gør tingene klar
		let audioContext = new AudioContext({ sampleRate: 384000 }); //make new audio context with the given sample rate 
		let input = audioContext.createMediaStreamSource(stream); //creates audio stream representing the mic input audio
		let analyser = audioContext.createAnalyser(); //gør det muligt at analysere lyden
		let scriptProcessor = audioContext.createScriptProcessor(256, 1, 1); //the buffer - amplitudeværdierne smides i bufferen
		analyser.smoothingTimeConstant = 0.0; //når den næste buffer skal fyldes kommer den lige efter den der var før 
		analyser.fftSize = 16384; //window size of fft in number of samples - analyserer så mange samples af gangen

		//sammensætter tingene 
		input.connect(analyser); //connects input to the analyser
		analyser.connect(scriptProcessor); //connects analyser to scriptProcessor (bufferen)
		scriptProcessor.connect(audioContext.destination); //connects scriptProcessor to the mic as its input buffer

		function onAudio() { 
			let spectrum = new Uint8Array(analyser.frequencyBinCount); //an integer half that of the .fftSize - window size deles i bins.
			void analyser.getByteFrequencyData(spectrum); //fft analysen er lavet og amplituden af frekvenserne smides i spectrum arrayet, indekserne er bins
			//void because is does not return anything
			let loudestBin = 0;
      		let specFrequency = 0;

			for (let i = 0; i < analyser.frequencyBinCount; i++) { //loops through spectrum array with the apmlitudes referred to as bins
				if (spectrum[i] > spectrum[loudestBin] && spectrum[i] > 120) { //finds the bin with the loudest amplitude
          			specFrequency = i * (audioContext.sampleRate / analyser.fftSize); 
          			if(specFrequency >= 400 && specFrequency <= 7600){
            			loudestBin = i;
          			}
				}
			}
			let frequency = loudestBin * (audioContext.sampleRate / analyser.fftSize); //regner frekvensen ud, ud fra den bin der havde den højeste amplitude
			
			document.getElementById("spectogram").style.width = `${loudestBin/500*50}vw`; 
			//if recording is true and the freq is either valid or a separation tone then push to spec
			if (recording && (isValidFreq(frequency) || compare(frequency, sepFreq))) {
				spec.push(frequency);
			}
		};
		scriptProcessor.addEventListener('audioprocess', onAudio);
	}).catch (error => { //error handling
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
	document.getElementById("img").style.imageRendering = "pixelated"; //makes the images "clearer"

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
