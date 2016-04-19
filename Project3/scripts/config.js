requirejs.config({
	"paths" : {
		"jqueryMin" : "jqueryMin",
		"init" : "init"
	},
	"shim" : {
		"init" 		: ["jqueryMin"],
		"JSONModel" : ["init", "VEC3"]
	}
});

require(["main"]);
