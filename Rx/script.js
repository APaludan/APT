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
  for (let p = 0; p < spec.length; p++) {
    if (!(isValidFreq(spec[j]) || compare(loudestFreq, 440)))
    spec.splice(p, 1);
  }

  //spec[] is now filled up with loudest freqs 
  //if not recording start by comparing spec[0]
  let x = 0,
    current = 0;
  let separator = false;
  console.log(spec);
  while (!((compare(spec[x], 440) && compare(spec[x + 1], 440))) && x < spec.length) { //sort out background noise before signal has started
    //while the frequency we are looking at, and the next frequency aren't just noise, skip and continue
    x++;
  }
  console.log("x: ", x);
  while (x < spec.length) {
    while (compare(spec[x], 440) && x < spec.length) {
      if (compare(spec[x + 1], 440))
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