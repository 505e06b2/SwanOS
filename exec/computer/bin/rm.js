(function(args) {
	if(args[0]) {
		if(args[0] === "-r") {
			var resolved = shell.resolve(args[1]);
			if(os.exists(resolved) === "directory") { //go through the dir
		
				var recurse_dirs = function(dir) {
					var dir_contents = os.listdir(dir); //returns an array of files
					dir_contents.some(function(e) {
						var current = dir + "/" + e;
						var type = os.exists(current);
					
						switch(type) {
							case "directory":
								recurse_dirs(current);
								os.rmdir(current);
								return false;
							case "file":
								if(!os.remove(current)) {
									throw current + " couldn't be removed";
									return true;
								}
								return false;
							default: //kill if undefined
								return true;
						}
					});
				}
			}
			
			recurse_dirs(resolved);
			os.rmdir(resolved);
			
		} else {
			//os.print(shell.resolve(args[0]));
			if(!os.remove(shell.resolve(args[0]))) throw args[0] + " couldn't be removed";
		}
	}
})