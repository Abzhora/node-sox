var sox = require('./build/Release/format');
var Format = sox.Format;
var Signal = sox.Signal;
var Effect = sox.Effect;

var fmt = new Format;
fmt.readFile('test.wav');
var sig = new Signal(fmt.getSignal());
var wf = new Format;
var e = new Effect('input', ['test.wav']);
//wf.play(sig, 'alsa');
wf.play(sig);
