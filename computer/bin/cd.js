(function(args) {
	var folder = args.join(" ");
	if(!shell.chdir(folder)) {
		os.print("'" + folder + "' is not a directory");
	}
})