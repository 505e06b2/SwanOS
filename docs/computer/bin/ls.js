(function(args) {
	var folder = shell.resolve(args.join(" "));
	var result = os.listdir(folder);
	if(typeof(result) === "undefined") throw folder + " is not a directory";
	result.forEach(function(e) {
		os.print(e);
	});
})