(function(args) {
	var folder = shell.resolve(args.join(" "));
	os.list(folder).forEach(function(e) {
		os.print(e);
	});
})