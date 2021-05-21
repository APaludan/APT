const validFreqs = [880, 1320, 1760, 2200, 2640, 3080, 3520, 3960, 4400, 4840, 5280, 5720, 6160, 6600]; //skal matche de freq vi sender ved (14 toner). Separator tone behøver IKKE inkluderes
const bitCombinations = [ "000", "001", "010", "100", "011", "101", "110", "111",
                          "00", "01", "10", "11",
                          "0", "1"]; //skal matche arrayet fra Tx

let case1 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case2 = [362, 362, 362, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case3 = [440, 440, 362, 362, 362, 362, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case4 = [440, 440, 2200, 2200, 2200, 2200, 362, 362, 362, 362, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case5 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 362, 362, 362, 362, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case6 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 362, 362, 362, 362, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case8 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 362, 362, 362, 362, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 440];
let case9 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 362, 362, 362, 362, 4400, 4400, 4400, 4400, 440];
let case10 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 362, 362, 362, 362, 440];
let case11 = [440, 440, 2200, 2200, 2200, 2200, 440, 440, 440, 440, 1320, 1320, 1320, 1320, 440, 440, 440, 440, 3520, 3520, 3520, 3520, 440, 440, 440, 440, 4400, 4400, 4400, 4400, 362, 362, 362, 362,];
let case12 = [362, 362, 362, 362, 1511, 1511, 1511, 1511, 2411, 2411, 2411, 2411, 1511, 1511, 1511, 1511, 362, 362, 362, 362, 131, 131, 131, 131, 362, 362, 362, 362, 2411, 2411, 2411, 2411, 362, 362, 362, 362];





if (!recording) {
    let x = 0, current = 0;
    let seperator = false;
    console.log(spec);
    while (!((compare(spec[x], 400, 480) && compare(spec[x + 1], 400, 480))) && x < spec.length) { //sort out background noise before signal has started
      //while the frequency we are looking at, and the next frequency aren't just noise, skip and continue
      x++;
    }
    console.log("x: ", x);
    while (x < spec.length) { 
      while (!isValidFreq(spec[x]) && x < spec.length && !compare(spec[x], 400, 480)) {
        x++;
        if (!isValidFreq(spec[x]) && x < spec.length && !compare(spec[x], 400, 480)) {
          //x = spec.length; //if x and x+1 is not valid, skip all.
        } //hvad sker der her, hvorfor er der både while og if haha
      }
      while (compare(spec[x], 400, 480) && x < spec.length) { 
        seperator = true; //while spec[x] is between 400 and 480 then it is a seperatortone
        x++;
        //console.log(x);
      }
      while (!seperator && compare(spec[x], current - 40, current + 40) && x < spec.length) {
        //If the tone is not the seperation tone, and the frequency is the same as the prev, just skip
        x++;
      }

      current = spec[x];
      if (isValidFreq(spec[x]) && seperator) { //if the freq is valid an the seperator tone has been there
        seperator = false;
        identifiedFreqs.push(isValidFreq(spec[x])); //push to identified freqs
        bitstring += freqToBits(identifiedFreqs[identifiedFreqs.length - 1]);
        //console.log("x: " + x + "freq: " + identifiedFreqs[identifiedFreqs.length-1]);
        while (!compare(spec[x], 400, 480) && x < spec.length) { //hvad sker der her
          x++;
        }
      }
    }
}