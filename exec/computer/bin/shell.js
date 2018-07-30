(function(args) {
	const self = this;
	
	self.run = true;
	self.current_dir = "/";
	self.path = "/bin";
	self.startup_time = Date.now();
	
	self.exit = function() {
		self.run = false;
	}
	
	self.resolve = function(path) {
		if(path[0] == "/") return path;
		return this.current_dir + path;
	}
	
	self.chdir = function(path) {
		var result = os.chdir(self.resolve(path));
		if(result) self.current_dir = result;
		return result;
	}
	
	self.uptime = function() {
		return Math.round((Date.now() - self.startup_time)/1000);
	}
	
	self.exec = function() {
		while(this.run) {
			var line = os.stdin(self.current_dir.replace(/^\/+|\/+$/gm, "") + "> "); //regex for removing excess '/'
			if(line === "") continue;
			
			line = line.trim().match(/(?=\S)[^"\s]*(?:"[^\\"]*(?:\\[\s\S][^\\"]*)*"[^"\s]*)*/g); // stolen from https://stackoverflow.com/a/40120309
			for(var i = 0; i < line.length; i++) if(line[i][0] === "\"" && line[i][line[i].length-1] === "\"") line[i] = line[i].slice(1, -1);
			
			if(line[0] === "exit") {
					self.run = false;
					continue;
			}
			
			var r;
			var args = line.slice(1);
			var filename = line[0];
			if(filename.slice(-3) !== ".js") filename = filename + ".js";
			filename = "/" + filename;
			
			var paths = self.path.split(":");
			paths = [self.current_dir].concat(paths); //Add current dir to start of paths
			
			for(var i = 0; i < paths.length; i++) {
				r = os.run(paths[i] + filename);
				if(r) {
					if(typeof(r) === "function") {
						try {
							r(args);
						} catch(err) {
							os.print(">>> [" + paths[i] + filename + "] => " + err);
						}
					} else os.print("\"Executables\" are functions wrapped in brackets, like (function(args) {})... Here's the contents of your file: " + r);
					break;
				}
			}
			
			if(r) continue;
			os.print("'" + filename + "' is not a command");
		}
		
		return "Shell Terminated";
	}
	
	if(args != false) self.exec();
})