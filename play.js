var sox = require('./build/Release/sox');
var Format = sox.Format;
var Signal = sox.Signal;
var Effect = sox.Effect;
var Chain = sox.Chain;

var rf = new Format;
rf.readFile('test.wav');

var sig = new Signal(rf.getSignal());
var wf = new Format;
//wf.play(sig, 'alsa');
wf.play(sig);

var c = new Chain(rf, wf);
c.addEffect(new Effect('input', ['test.wav']), rf, rf);
c.addEffect(new Effect('rate', []), rf, wf);
c.addEffect(new Effect('channels', []), rf, wf);
c.addEffect(new Effect('output', ['rawk.wav']), rf, wf);
c.flow();
