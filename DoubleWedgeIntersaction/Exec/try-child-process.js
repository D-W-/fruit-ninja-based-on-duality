const execFile = require('child_process').execFile;
const child = execFile('.\\DoubleWedgeIntersaction-argc.exe', ['-0.5 0 -1 0 0 1 0 -1'], (error, stdout, stderr) => {
	if (error) {
		throw error;
	}
	console.log(stdout);
});
