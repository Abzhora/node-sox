var sox = require('./build/Release/format');
var Format = sox.Format;
var Signal = sox.Signal;

var fmt = new Format;
fmt.readFile('test.wav');
var sig = new Signal(fmt.getSignal());
var wf = new Format;
wf.play(sig, 'alsa');
