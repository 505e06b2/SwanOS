(function(args) {
	if(args[0]) {
		var resolved = shell.resolve(args[1]);
		if(args[0] === "-r" && os.exists(resolved) === "directory") { //go through the dir
			var dir_contents = os.listdir(resolved); //returns an array of files
			dir_contents.some(function(e) {
				if(!os.remove(resolved + "/" + e)) {
					os.print("Couldn't remove: '" + resolved + "/" + e + "'");
					return true;
				}
			});
			os.rmdir(resolved);
			
		} else {
			if(!os.remove(resolved)) os.print("Couldn't remove: '" + args[0] + "'");
		}
	}
})