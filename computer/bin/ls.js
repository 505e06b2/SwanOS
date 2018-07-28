(function(args) {
	var folder = shell.resolve(args.join(" "));
	os.listdir(folder).forEach(function(e) {
		os.print(e);
	});
})