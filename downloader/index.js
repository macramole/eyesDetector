var xv = require("xvideos");
var exec = require("child_process").exec

xv.search({ k : "pov"}, function(err, results) {
	console.log(err);
	console.log(results);

	xv.details(results.videos[0].url, function(err, result) {
		console.log(err);
		console.log("wget \"" + result.flv + "\"")
		exec("wget \"" + result.flv + "\"", function(err, stdout, stderr) {
			console.log(err);
		});
	});
});
