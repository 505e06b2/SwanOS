(function(args) {
	if(args.length >= 2) {
		var contents = os.read(shell.resolve(args[0]))
		if(contents) {
			var destination = shell.resolve(args[1])
			if(os.exists(destination) === "directory") destination += "/" + args[0].split("/")[-1];
			if(!os.write(destination, contents)) throw destination + " can't be written to";
			
		} else throw filename + " can't be read";
		
	} else throw "Not enough arguments";
})