--
-- _xcode.lua
-- Define the Apple XCode action and support functions.
-- Copyright (c) 2009 Jason Perkins and the Premake project
--

premake.APPEX       = "AppExtension"
premake.WATCHAPP    = "WatchApp"
premake.WATCHEX     = "WatchKitExtension"
premake.FRAMEWORK   = "Framework"

premake.api.addAllowed("kind", "AppExtension")
premake.api.addAllowed("kind", "Framework")
premake.api.addAllowed("kind", "WatchApp")
premake.api.addAllowed("kind", "WatchKitExtension")

	premake.xcode = { }
	
	local api = premake.api

---
---

	
	api.register {
		name = "xcodebuildsettings",
		scope = "config",
		kind = "key-array",
	}
	-- doesn;t print the settings in the project 
	-- usefull if you have multiple targets and you don't want the settings in the main project to interact with the ones in other targets  
	api.register {
		name = "xcodeNoProjectSettings",
		scope = "project",
		kind = "boolean",
		default = false,
	}
	api.register {
		name = "xcodebuildresources",
		scope = "config",
		kind = "list",
	}
		
	api.register {
		name = "xcodebuildfilesettings",
		scope = "project",
		kind = "key-array",
	}
 	
 	api.register {
		name = "xcodeTarget",
		scope = "config",
		kind = "list",
	}
 	
	api.register {
		name = "frameworkdirs",
		scope = "config",
		kind = "list:directory",
		tokens = true,
    }
        
	--api.register {
	--	name = "xcconfigfile",
	--	scope = "config",
	--	kind = "path",
	--	tokens = true,
    --}
    api.register {
		name = "xcodepchheader",
		scope = "config",
		kind = "string",
		tokens = true,
    }
    
	api.register {
		name = "xcodeTargetDependency",
		scope = "project",
		kind = "list:string",
		tokens = true,
    }

    api.register {
		name = "xcodeTargetAttributes",
		scope = "project",
		kind = "key-array",
		tokens = false,
    }
---
---

	function isExtension(kind)
		return kind == premake.APPEX or kind == premake.WATCHAPP or kind == premake.WATCHEX
	end
			
		
	dofile("xcode_common.lua")
	dofile("xcode4_workspace.lua")
	dofile("xcode_project.lua")
	

	newaction 
	{
		trigger         = "xcode4",
		shortname       = "Xcode 4",
		description     = "Generate Apple Xcode 4 project files",
		os              = "macosx",

		valid_kinds     = { "ConsoleApp", "WindowedApp", "AppExtension", "Framework", "SharedLib", "StaticLib", "Makefile", "None" },
		
		valid_languages = { "C", "C++", "Objective-C", "Swift" },
		
		valid_tools     = 
		{
			cc     = { "gcc" , "clang"},
		},

		valid_platforms = 
		{ 
			Native = "Native", 
			x32 = "Native 32-bit", 
			x64 = "Native 64-bit", 
			Universal32 = "32-bit Universal", 
			Universal64 = "64-bit Universal", 
			Universal = "Universal",			
		},
		
		default_platform = "Universal",
		
		onsolution = function(sln)
			premake.generate(sln, ".xcworkspace/contents.xcworkspacedata", premake.xcode4.workspace_generate)
		end,
		
		onproject = function(prj)
			if not isExtension(prj.kind) then
				premake.generate(prj, ".xcodeproj/project.pbxproj", premake.xcode.project)
			end	
		end,
		
		oncleanproject = function(prj)
			--premake.clean.directory(prj, "%.xcodeproj")
			--premake.clean.directory(prj, "%.xcworkspace")
		end,

		oncleansolution = function(sln)

		end,
		
		oncleantarget   = function()
		
		end,
 
		
		oncheckproject = function(prj)
			-- Xcode can't mix target kinds within a project
			local last
			for cfg in project.eachconfig(prj) do
				if last and last ~= cfg.kind then
					error("Project '" .. prj.name .. "' uses more than one target kind; not supported by Xcode", 0)
				end
				last = cfg.kind
			end
		end,
	}