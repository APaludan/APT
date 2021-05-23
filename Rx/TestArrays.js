const validFreqs = [880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400, 4840, 5280, 5720, 6160, 6600]; //skal matche de freq vi sender ved (14 toner). Separator tone behøver IKKE inkluderes
const bitCombinations = ["000", "001", "010", "100", "011", "101", "110", "111",
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

test(case1);

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


function test(spec) {
	//empty array and string for holding the collected freqs and converting to bits
	let identifiedFreqs = [];
	let bitstring = '';

	//spec[] is now filled up with loudest freqs 
	//if not recording start by comparing spec[0]
	let x = 0, current = 0;
	let separator = false; //this boolean controls whether there has been a separator tone yet
	console.log(spec);

	//step 1: skip all tones registered before the first separator tones have been registered multiple times:
	while (!((compare(spec[x], 440) && compare(spec[x + 1], 440))) && x < spec.length) {
		x++;
	}

	//step 2: identify information carrying frequencies in the "spec" array:
	while (x < spec.length) {
		//identify consecutive separator tones:
		while (compare(spec[x], 440) && x < spec.length) {
			if (compare(spec[x + 1], 440))
				separator = true; //if two separator tones are identified in a row, separator boolean is set to true
			x++;
		}

		if (!separator) //if we reach this point, and separator is false, we skip the frequency
			x++;

		current = spec[x];

		// if the current frequency is valid and there has been a separator tone, we add the frequency to 
		// the array of identified (info carrying) frequencies. Then we skip all the frequencies similar to this freq:
		if (isValidFreq(spec[x]) && separator) { //if the freq is valid an the separator tone has been there
			separator = false;
			identifiedFreqs.push(isValidFreq(spec[x])); //push to identified freqs
			bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
			while (!compare(spec[x], 440) && x < spec.length) { //spring til næste separator
				x++;
			}
		}
	}
	console.log(identifiedFreqs);
	console.log("bitstring: " + bitstring);
}