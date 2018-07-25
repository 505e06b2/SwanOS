function(args, current_dir) {
	var filename = args.join(" ");
	var contents = os.read(shell.resolve(filename));
	if(contents) os.print(contents);
	else os.print("'" + filename + "' can't be read");
}