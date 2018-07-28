(function(args) {
	if(args[0]) {
		var resolved = shell.resolve(args[1]);
		if(args[0] === "-r" && os.exists(resolved) === "directory") { //go through the dir
		
			var recurse_dirs = function(dir) {
				var dir_contents = os.listdir(dir); //returns an array of files
				dir_contents.some(function(e) {
					var current = dir + "/" + e;
					var type = os.exists(current);
					
					switch(type) {
						case "directory":
							recurse_dirs(current);
							return false;
						case "file":
							if(!os.remove(current)) {
								os.print("Couldn't remove: '" + args[0] + "'");
								return true;
							}
							return false;
						default: //kill if undefined
							return true;
					}
				});
			}
			
			recurse_dirs(resolved);
			os.rmdir(resolved);
			
		} else {
			if(!os.remove(resolved)) os.print("Couldn't remove: '" + args[0] + "'");
		}
	}
})