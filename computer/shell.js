shell = {
	run: true,
	current_dir: "/",
	path: ".:/bin",
	startup_time: Date.now(),
	
	exec: function() {
		while(this.run) {
			var line = os.stdin(this.current_dir.replace(/^\/+|\/+$/gm, "") + "> "); //regex for removing excess '/'
			if(line === "") continue;
			
			line = line.trim().match(/(?=\S)[^"\s]*(?:"[^\\"]*(?:\\[\s\S][^\\"]*)*"[^"\s]*)*/g); // stolen from https://stackoverflow.com/a/40120309
			for(var i = 0; i < line.length; i++) if(line[i][0] === "\"" && line[i][line[i].length-1] === "\"") line[i] = line[i].slice(1, -1);
			
			if(line[0] === "exit") {
					this.run = false;
					continue;
			}
			
			var r;
			var args = line.slice(1);
			var filename = line[0];
			if(filename.slice(-3) !== ".js") filename = filename + ".js";
			filename = "/" + filename;
			
			var paths = this.path.split(":");
			for(var i = 0; i < paths.length; i++) {
				r = os.run(paths[i] + filename);
				if(r) {
					if(typeof(r) === "function") {
						r(args);
					} else os.print(r);
					break;
				}
			}
			
			if(r) continue;
			os.print("'" + filename + "' is not a command");
		}
	},
	
	exit: function() {
		this.run = false;
	},
	
	resolve: function(path) {
		if(path[0] == "/") return path;
		return this.current_dir + path;
	},
	
	chdir: function(path) {
		var result = os.chdir(this.resolve(path));
		if(result) this.current_dir = result;
		return result;
	},
	
	uptime: function() {
		return Math.round((Date.now() - this.startup_time)/1000);
	}
};