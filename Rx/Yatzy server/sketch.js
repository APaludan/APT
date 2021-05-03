let mic, fft;

function preload(){
    mic = loadSound('out.flac');
}

function setup() {
  createCanvas(1000, 600);
  noFill();

  //mic = new p5.AudioIn();
  //mic.start();
  
  mic.play();
  fft = new p5.FFT();
  fft.setInput(mic);
}

function draw() {
  background(200);

  waveform = fft.waveform();
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

function drawWaveform() {
    stroke(240);
    strokeWeight(4);
    beginShape();
    for (let i = 0; i < waveform.length; i++) {
      let x = map(i, 0, waveform.length, 0, width);
      let y = map(waveform[i], -1, 1, -height / 2, height / 2);
      vertex(x, y + height / 2);
    }
    endShape();
  }

function mousePressed() {
    if (mic.isPlaying()) {
      // .isPlaying() returns a boolean
      mic.pause(); // .play() will resume from .pause() position
    } else {
      mic.play();
    }
  }
