var sox = require('./build/Release/sox');
var s = sox.hello({
    onend : function () { console.log('__END__') },
});
console.log(s);
