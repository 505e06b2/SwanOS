(function(args) {
	var folder = args.join(" ");
	if(!shell.chdir(folder)) throw folder + " can't be cd'd into";
})