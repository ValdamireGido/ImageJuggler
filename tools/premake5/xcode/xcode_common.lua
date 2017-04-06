--
-- xcode_common.lua
-- Functions to generate the different sections of an Xcode project.
-- Copyright (c) 2009-2011 Jason Perkins and the Premake project
--

	local xcode = premake.xcode
	local tree  = premake.tree
    local solution = premake.solution
	local project = premake.project
    local config = premake.config 	
	local fileconfig = premake.fileconfig

	xcode.buildcategories = {}
	
	
	function xcode.getbuildbuildcategories(kind)
		local categories = {
			[".a"] = "Frameworks",
			[".c"] = "Sources",
			[".cc"] = "Sources",
			[".cpp"] = "Sources",
			[".cxx"] = "Sources",
			[".dylib"] = "Frameworks",
			[".framework"] = "Frameworks",
			[".m"] = "Sources",
			[".mm"] = "Sources",
			[".strings"] = "Resources",
			[".nib"] = "Resources",
			[".xib"] = "Resources",
			[".icns"] = "Resources",
			[".s"] = "Sources",
			[".S"] = "Sources",
			[".swift"] = "Sources",
			[".storyboard"] = "Resources",
			[".entitlements"] = "Resources",
			[".appex"] = "Embed App Extensions",
			[".xcdatamodeld"] = "Sources",
			[".xcassets"] = "Resources",
--			[".xcconfig"] = "Resources", -- xconfig files should not be put in resources
			[".vsh"] = "Resources",
			[".fsh"] = "Resources",
			[".app"] = "Resources",
		}
		
		if kind == "Framework" then
			categories[".h"] = "Headers"
		end
		
		return categories
	end


--
-- Return the Xcode build category for a given file, based on the file extension.
--
-- @param node
--    The node to identify.
-- @returns
--    An Xcode build category, one of "Sources", "Resources", "Frameworks", or nil.
--

	function xcode.getbuildcategory(node)
		if node.isResource then
			return "Resources"
		end
		local categories = xcode.buildcategories
		return categories[path.getextension(node.name)]
	end


	function xcode.appendbulidsettings(settings, config)
		if config then
			for key, values in pairs(config) do
				settings = settings .. key .. " = "
				valuesCount = table.getn(values)
				if valuesCount > 1 then
					settings = settings .. "("
				end
				for index, value in ipairs(values) do
					if index == valuesCount then
						settings = settings .. value
					else
						settings = settings .. value .. ", "
					end
				end
				if valuesCount > 1 then
					settings = settings .. ")"
				end
				settings = settings .. "; "
			end
		end
		
		return settings
	end

	function xcode.getbuildfilesettings(node, cfg)
		
		local xcodebuildsettings
		
		if node.configs then
			local filecfg = fileconfig.getconfig(node, cfg)
			if filecfg then
				if filecfg.xcodebuildsettings then
					xcodebuildsettings = filecfg.xcodebuildsettings
				end
			end
		end	
		
		if not node.buildfilesettings and not xcodebuildsettings then 
			return "" 
		end
		
		local settings = ""
		settings = xcode.appendbulidsettings(settings, node.buildfilesettings)
		settings = xcode.appendbulidsettings(settings, xcodebuildsettings)
		
		if settings == "" then
			return ""
		else
			settings = "settings = {" .. settings .. "}; "
		end
		
		return settings
	end


	function xcode.isItemResource(project, node)
	
		local res;
			
		if project and project.xcodebuildresources then
			if type(project.xcodebuildresources) == "table" then								
				res = project.xcodebuildresources			
			end							
		end	
			
		local function checkItemInList(item, list)
			if item then
				if list then
					if type(list) == "table" then	
						for _,v in pairs(list) do												
							if string.find(item, v) then								
								return true
							end						  
						end
					end
				end	
			end
			return false
		end
		
		--print (node.path, node.buildid, node.cfg, res)
		if (checkItemInList(node.path, res)) then			
			return true
		end
		
		return false
	end		
--
-- Return the displayed name for a build configuration, taking into account the
-- configuration and platform, i.e. "Debug 32-bit Universal".
--
-- @param cfg
--    The configuration being identified.
-- @returns
--    A build configuration name.
--

	function xcode.getconfigname(cfg)
		local name = cfg.buildcfg
		if #cfg.project.solution.platforms > 1 then
			name = name .. " " .. premake.action.current().valid_platforms[cfg.platform]
		end
		return name
	end


--
-- Return the Xcode type for a given file, based on the file extension.
--
-- @param fname
--    The file name to identify.
-- @returns
--    An Xcode file type, string.
--

	function xcode.getfiletype(node, cfg)
		
		if node.configs then
			local filecfg = fileconfig.getconfig(node, cfg)
			if filecfg then
				if filecfg.language == "ObjC" then 
					return "sourcecode.c.objc"
				elseif 	filecfg.language == "ObjCpp" then
					return "sourcecode.cpp.objcpp"
				end	
			end
		end
		
		local types = {
			[".c"]            = "sourcecode.c.c",
			[".cc"]           = "sourcecode.cpp.cpp",
			[".cpp"]          = "sourcecode.cpp.cpp",
			[".css"]          = "text.css",
			[".cxx"]          = "sourcecode.cpp.cpp",
			[".S"]            = "sourcecode.asm.asm",
			[".framework"]    = "wrapper.framework",
			[".gif"]          = "image.gif",
			[".h"]            = "sourcecode.c.h",
			[".html"]         = "text.html",
			[".lua"]          = "sourcecode.lua",
			[".m"]            = "sourcecode.c.objc",
			[".mm"]           = "sourcecode.cpp.objc",
			[".nib"]          = "wrapper.nib",
			[".pch"]          = "sourcecode.c.h",
			[".plist"]        = "text.plist.xml",
			[".strings"]      = "text.plist.strings",
			[".xib"]          = "file.xib",
			[".icns"]         = "image.icns",
			[".s"]            = "sourcecode.asm",
			[".bmp"]          = "image.bmp",
			[".wav"]          = "audio.wav",
			[".xcassets"]     = "folder.assetcatalog",
			[".swift"]        = "sourcecode.swift",
			[".playground"]   = "file.playground",
			[".app"]          = "wrapper.application",
			[".storyboard"]   = "file.storyboard",
			[".appex"]        = "wrapper.app-extension",
			[".entitlements"] = "text.xml",
			[".xcdatamodeld"] = "sourcecode.xcdatamodeld",
			[".xcconfig"]     = "text.xcconfig",
		}
		return types[path.getextension(node.path)] or "text"
	end

--
-- Print user configuration references contained in xcodeconfigreferences
-- @param offset
--    offset used by function _p
-- @param cfg
--    configuration
--

	local function xcodePrintUserConfigReferences(offset, cfg, tr, kind)
		local referenceName
		if kind == "project" then
			referenceName = cfg.xcodeconfigreferenceproject
		elseif kind == "target" then
			referenceName = cfg.xcodeconfigreferencetarget
		end
		tree.traverse(tr, {
			onleaf = function(node)
				filename = node.name
				if node.id and path.getextension(filename) == ".xcconfig" then
					if filename == referenceName then
						_p(offset, 'baseConfigurationReference = %s /* %s */;', node.id, filename)
						return	        
					end
				end
			end
		}, false)
	end


	
	local escapeSpecialChars = {
		['\n'] = '\\n',
		['\r'] = '\\r',
		['\t'] = '\\t',
	}
	
	local function escapeChar(c)
		return escapeSpecialChars[c] or '\\'..c
	end
	
	local function escapeArg(value)
		value = value:gsub('[\'"\\\n\r\t ]', escapeChar)
		return value
	end
	
	local function escapeSetting(value)
		value = value:gsub('["\\\n\r\t]', escapeChar)
		return value
	end
	
	local function stringifySetting(value)
		value = value..''
		if not value:match('^[%a%d_./]+$') then
			value = '"'..escapeSetting(value)..'"'
		end
		return value
	end

	local function customStringifySetting(value)
		value = value..''
		
		local test = value:match('^[%a%d_./%+]+$')		
		if test then		
			value = '"'..escapeSetting(value)..'"'
		end
		return value
	end
	
	local function printSetting(level, name, value)
		if type(value) == 'function' then
			value(level, name)
		elseif type(value) ~= 'table' then
			_p(level, '%s = %s;', stringifySetting(name), stringifySetting(value))
		elseif #value == 1 then
			_p(level, '%s = %s;', stringifySetting(name), stringifySetting(value[1]))
		elseif (#value >= 1) then
			_p(level, '%s = (', stringifySetting(name))
			for _, item in ipairs(value) do
				_p(level + 1, '%s,', stringifySetting(item))
			end
			_p(level, ');')	
		elseif (type(value) == "table") then
			local count = 0

			for key, item in pairs(value) do
				count = count + 1
			end
			if count > 0 then	
				_p(level, '%s = {', stringifySetting(name))
				for key, item in pairs(value) do
					if type(item) == "table" then
						printSetting(level + 1, key, item)
					else
						_p(level + 1, '%s = %s;', key, item)
					end	
				end
				_p(level, '};')	
			end	
		end
	end
	
	local function printSettingsTable(level, settings)
		-- Maintain alphabetic order to be consistent
		local keys = table.keys(settings)
		table.sort(keys)
		for _, k in ipairs(keys) do
			printSetting(level, k, settings[k])
		end
	end
	
	------ //	

	-- helper function
	function customPrintSetting(level, name, value)
	
		if type(value) == 'function' then
			value(level, name)
		elseif type(value) ~= 'table' then
			_p(level, '%s = %s', stringifySetting(name), value)
		elseif #value == 1 then
			_p(level, '%s = %s', stringifySetting(name), value[1])
		elseif #value >= 1 then
			io.write(stringifySetting(name) .. ' = ')
			for _, item in ipairs(value) do
				io.write(item .. ' ')
			end
			io.write('\n')
		end
	end
	
	-- custom print settings function for outputing settings in .xcconfig files
	function customPrintSettingsTable(level, settings)
		-- Maintain alphabetic order to be consistent
		local keys = table.keys(settings)
		table.sort(keys)
		for _, k in ipairs(keys) do
			customPrintSetting(level, k, settings[k])
		end
	end
	
----- //
	
	local function overrideSettings(settings, overrides)
		if type(overrides) == 'table' then
			for name, value in pairs(overrides) do
				-- Allow an override to remove a value by using false
				settings[name] = iif(value ~= false, value, nil)
			end
		end
	end

--
-- Return the Xcode product type, based target kind.
--
-- @param node
--    The product node to identify.
-- @returns
--    An Xcode product type, string.
--

	function xcode.getproducttype(node)
		local types = {
			ConsoleApp   = "com.apple.product-type.tool",
			WindowedApp  = "com.apple.product-type.application",
			StaticLib    = "com.apple.product-type.library.static",
			SharedLib    = "com.apple.product-type.library.dynamic",
			AppExtension = "com.apple.product-type.app-extension",
			Framework    = "com.apple.product-type.framework",
			
			WatchApp    = "com.apple.product-type.application.watchapp",
			WatchKitExtension    = "com.apple.product-type.watchkit-extension",
		}
		return types[node.cfg.kind]
	end


--
-- Return the Xcode target type, based on the target file extension.
--
-- @param node
--    The product node to identify.
-- @returns
--    An Xcode target type, string.
--

	function xcode.gettargettype(kind)
		local types = {
			ConsoleApp   = "\"compiled.mach-o.executable\"",
			WindowedApp  = "wrapper.application",
			StaticLib    = "archive.ar",
			SharedLib    = "\"compiled.mach-o.dylib\"",
			AppExtension = "\"wrapper.app-extension\"",
			Framework    = "wrapper.framework",
			WatchApp     = "wrapper.application",
			WatchKitExtension = "\"wrapper.app-extension\"",
		}
		return types[kind]
	end

--
-- Return a unique file name for a project. Since Xcode uses .xcodeproj's to 
-- represent both solutions and projects there is a likely change of a name
-- collision. Tack on a number to differentiate them.
--
-- @param prj
--    The project being queried.
-- @returns
--    A uniqued file name
--

	function xcode.getxcodeprojname(prj)
		-- if there is a solution with matching name, then use "projectname1.xcodeproj"
		-- just get something working for now
		local fname = premake.filename(prj, ".xcodeproj")
		return fname
	end


--
-- Returns true if the file name represents a framework.
--
-- @param fname
--    The name of the file to test.
--

	function xcode.isframework(fname)
		return (path.getextension(fname) == ".framework")
	end


--
-- Retrieves a unique 12 byte ID for an object.
-- This function accepts an array of parameters that will be used to generate the id.
--
-- @returns
--    A 24-character string representing the 12 byte ID.
--

	function xcode.newid(...)
		local name = ''
		for i, v in pairs(arg) do
			name = name..v..'****'
		end
			
		local a = ("%08X"):format(name:hash(16777619)):sub(1,8)
		local b = ("%08X"):format(name:hash(11352893)):sub(1,8)
		local c = ("%08X"):format(name:hash(46577619)):sub(1,8)
		return a..b..c
	end


--
-- 
--

-- added 'cfgname' and 'cfgtype' parameters:
-- cfgname - can be 'Release' or 'Debug'
-- cfgtype - can be 'project' or 'target'
-- removed 'cfg' parameter
function xcode.getbaseconfigurationreference(tr, cfgname, cfgtype)
	local function getxcconfig(tr, cfgname, cfgtype)
		local xcconfig
		tree.traverse(tr, {
			onnode = function(node)
				if node and string.find(node.name, '.xcconfig') and (string.find(node.name, string.lower(cfgname)) and (string.find(node.name, cfgtype) or cfgtype == 'common')) then					
					xcconfig = node
				end
			end
		})
		return xcconfig
	end
	-- we are no longer restricted to using only one xcconfig file !
	local node = getxcconfig(tr, cfgname, cfgtype)
	if node then
		return node.id, node.name
	end	
		
end


--
-- Create a product tree node and all projects in a solution; assigning IDs 
-- that are needed for inter-project dependencies.
--
-- @param sln
--    The solution to prepare.
--

	function xcode.preparesolution(sln)
		-- create and cache a list of supported platforms
		sln.xcode = { }
	
		for prj in premake.solution.eachproject(sln) do
			-- need a configuration to get the target information
			local cfg = project.findClosestMatch(prj,  prj.configurations[1], prj.platforms[1])
			
			-- build the product tree node
			local bundlepath = cfg.buildtarget.bundlename ~= "" and cfg.buildtarget.bundlename or cfg.buildtarget.name;
			if (prj.external) or prj.kind == premake.STATICLIB then
				bundlepath = cfg.project.name
			end 

			local node = premake.tree.new(path.getname(bundlepath))
				
			node.cfg = cfg
			node.id = premake.xcode.newid(node.name, "product")
			node.targetid = premake.xcode.newid(node.name, "target")
			
			-- attach it to the project
			prj.xcode = {}
			prj.xcode.projectnode = node
		end
	end


---------------------------------------------------------------------------
-- Section generator functions, in the same order in which they appear
-- in the .pbxproj file
---------------------------------------------------------------------------

	function xcode.Header()
		_p('// !$*UTF8*$!')
		_p('{')
		_p(1,'archiveVersion = 1;')
		_p(1,'classes = {')
		_p(1,'};')
		_p(1,'objectVersion = 46;')
		_p(1,'objects = {')
		_p('')
	end


	function xcode.PBXBuildFile(tr)
		local cfg = project.getfirstconfig(tr.project)
		local settings = {};
		tree.traverse(tr, {
			onnode = function(node)
				if node.buildid then
					settings[node.buildid] = function(level)
						_p(level,'%s /* %s in %s */ = {isa = PBXBuildFile; fileRef = %s /* %s */; %s};', 
							node.buildid, node.name, xcode.getbuildcategory(node), node.id, node.name, xcode.getbuildfilesettings(node, cfg))
					end
				end
			end
		})
		
		if not table.isempty(settings) then
			_p('/* Begin PBXBuildFile section */')
			printSettingsTable(2, settings);
			_p('/* End PBXBuildFile section */')
			_p('')
		end
	end


	function xcode.PBXContainerItemProxy(tr)
		local settings = {}				
		
		for _, node in ipairs(tr.products.children) do			
			if node.containerProxyId then
				settings[node.containerProxyId] = function()
					_p(2,'%s /* PBXContainerItemProxy */ = {', node.containerProxyId)
					_p(3,'isa = PBXContainerItemProxy;')
					_p(3,'containerPortal = %s /* %s */;', "08FB7793FE84155DC02AAC07", "Project object") 
					_p(3,'proxyType = 1;')
					_p(3,'remoteGlobalIDString = %s;', node.prj.xcode.projectnode.targetid)
					_p(3,'remoteInfo = %s;', stringifySetting(node.prj.targetname or node.prj.name))
					_p(2,'};')
				end
			end	
		end	
			
		for _, node in ipairs(tr.projects.children) do				
			if node.productproxyid then
				settings[node.productproxyid] = function()
					_p(2,'%s /* PBXContainerItemProxy */ = {', node.productproxyid)
					_p(3,'isa = PBXContainerItemProxy;')
					_p(3,'containerPortal = %s /* %s */;', node.id, path.getname(xcode.getxcodeprojname(node.prj))) --path.getrelative(node.parent.parent.project.location, node.path))
					_p(3,'proxyType = 2;')
					_p(3,'remoteGlobalIDString = %s;', node.prj.xcode.projectnode.id)
					_p(3,'remoteInfo = %s;', stringifySetting(node.prj.targetname or node.prj.name))
					_p(2,'};')
				end
			end
			if node.targetproxyid then			
				settings[node.targetproxyid] = function()
					_p(2,'%s /* PBXContainerItemProxy */ = {', node.targetproxyid)
					_p(3,'isa = PBXContainerItemProxy;')
					_p(3,'containerPortal = %s /* %s */;', node.id, path.getname(xcode.getxcodeprojname(node.prj))) --path.getrelative(node.parent.parent.project.location, node.path))
					_p(3,'proxyType = 1;')
					_p(3,'remoteGlobalIDString = %s;', node.prj.xcode.projectnode.targetid)
					_p(3,'remoteInfo = %s;', stringifySetting(node.prj.targetname or node.prj.name))
					_p(2,'};')
				end
			end	
			
		end

		if not table.isempty(settings) then
			_p('/* Begin PBXContainerItemProxy section */')
			printSettingsTable(2, settings);
			_p('/* End PBXContainerItemProxy section */')
			_p('')
		end
	end


	function xcode.PBXCopyFilesBuildPhase(tr)
		if #tr.binaries.children == 0 then
			return
		end

		local function hasEmbeddedBinaryOfType(target , which)
			local result = false
			if target.prj and target.prj.kind and target.prj.kind == which then 
				result = true 
			end
			return result
		end

		_p('/* Begin PBXCopyFilesBuildPhase section */')
		
		for _, target in ipairs(tr.products.children) do

			--[[
			if hasEmbeddedBinaryOfType(premake.FRAMEWORK) then
				_p(2,'%s /* Embed Frameworks */ = {', target.embfwkid)
				_p(3,'isa = PBXCopyFilesBuildPhase;')
				_p(3,'buildActionMask = 2147483647;')
				_p(3,'dstPath = "";')
				_p(3,'dstSubfolderSpec = 10;')
				_p(3,'files = (')
				tree.traverse(tr.binaries, {
					onnode = function(node)
						if node.project and node.project.kind and node.project.kind == premake.FRAMEWORK then
							_p(4,'%s /* %s in %s */,', node.buildid, node.name, xcode.getbuildcategory(node))
						end
					end
				})
				_p(3,');')
				_p(3,'name = "Embed Frameworks";')
				_p(3,'runOnlyForDeploymentPostprocessing = 0;')
				_p(2,'};')
			end
			]]
			if  target.embextid ~= nil then
				_p(2,'%s /* Embed App Extensions */ = {', target.embextid)
				_p(3,'isa = PBXCopyFilesBuildPhase;')
				_p(3,'buildActionMask = 2147483647;')
				_p(3,'dstPath = "";')
				_p(3,'dstSubfolderSpec = 13;')
				_p(3,'files = (')
				tree.traverse(tr.binaries, {
					onnode = function(node)
						if node.prj and node.prj.kind and (node.prj.kind == premake.APPEX or  node.prj.kind == premake.WATCHEX) then
							_p(4,'%s /* %s in %s */,', node.buildid, node.name, xcode.getbuildcategory(node))
						end
					end
				})
				_p(3,');')
				_p(3,'name = "Embed App Extensions";')
				_p(3,'runOnlyForDeploymentPostprocessing = 0;')
				_p(2,'};')
			end
		end
		
		_p('/* End PBXCopyFilesBuildPhase section */')
		_p('')
	end
	
	
	function xcode.PBXFileReference(tr)
		local cfg = project.getfirstconfig(tr.project)
		local settings = {}
		
		tree.traverse(tr, {
			onleaf = function(node)
				-- I'm only listing files here, so ignore anything without a path
				if not node.path then
					return
				end
				
				--if node.isBinary ~= nil and node.isBinary
				-- is this the product node, describing the output target?
				if node.kind == "product" then
					if node.id then
						settings[node.id] = function(level)					
							_p(level,'%s /* %s */ = {isa = PBXFileReference; explicitFileType = %s; includeInIndex = 0; name = %s; path = %s; sourceTree = BUILT_PRODUCTS_DIR; };',
								node.id, node.name, xcode.gettargettype(node.cfg.kind), stringifySetting(node.name), stringifySetting(path.getname(node.cfg.buildtarget.bundlename ~= "" and node.cfg.buildtarget.bundlename or node.cfg.buildtarget.relpath)))
						end
					end	
				--elseif node.parent.parent == tr then
				-- ignore products
				
				-- is this a project dependency?
				elseif node.parent.parent == tr.projects then
					if not node.cfg or (node.cfg.kind ~= premake.APPEX and node.cfg.kind ~= premake.WATCHEX) then 
						settings[node.parent.id] = function(level)
							-- ms Is there something wrong with path is relative ?
							-- if we have a and b without slashes get relative should assume the same parent folder and return ../
							-- this works if we put it like below
							local relpath = path.getrelative(path.getabsolute(tr.project.location), path.getabsolute(node.parent.prj.location))
							_p(level,'%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = "wrapper.pb-project"; name = %s; path = %s; sourceTree = SOURCE_ROOT; };',
								node.parent.id, node.name, customStringifySetting(node.parent.name), stringifySetting(path.join(relpath, node.parent.name)))
						end
					end
				-- something else
				else
					settings[node.id] = function(level)
					local pth, src
					if xcode.isframework(node.path) then
						--respect user supplied paths
						-- look for special variable-starting paths for different sources
						local nodePath = node.path
						local _, matchEnd, variable = string.find(nodePath, "^%$%((.+)%)/")
						if variable then
							-- by skipping the last '/' we support the same absolute/relative
							-- paths as before
							nodePath = string.sub(nodePath, matchEnd + 1)
						end
						if string.find(nodePath,'/')  then
							if string.find(nodePath,'^%.')then
								--error('relative paths are not currently supported for frameworks')
								pth = path.getrelative(tr.project.location, node.path)
								--print(tr.project.location, node.path , pth)
								src = "SOURCE_ROOT"
								variable = src
							else
								pth = nodePath
								src = "<absolute>"	
							end							
						end
						-- if it starts with a variable, use that as the src instead
						if variable then
							src = variable
							-- if we are using a different source tree, it has to be relative
							-- to that source tree, so get rid of any leading '/'
							if string.find(pth, '^/') then
								pth = string.sub(pth, 2)
							end
						else
							pth = "System/Library/Frameworks/" .. node.path
							src = "SDKROOT"
						end
					else
						-- something else; probably a source code file
						src = "<group>"
						
						if node.abspath then
							pth = path.getrelative(tr.project.location, node.abspath)
						else
							pth = node.path
						end 
						--end
						end

						local srcString = ""
						local pathString = ""
						if xcode.getfiletype(node, cfg) == "wrapper.app-extension" then
							srcString = "BUILT_PRODUCTS_DIR"
							pathString = node.name							
						else
							srcString = stringifySetting(src)
							pathString = stringifySetting(pth)
						end
						
						-- // the xcconfig files folder should not be referenced in PBXFileReference section !
						if node.name ~= 'xcconfigs' then
							_p(level,'%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = %s; name = %s; path = %s; sourceTree = %s; };',
							node.id, node.name, xcode.getfiletype(node, cfg), stringifySetting(node.name), pathString, srcString)
						end
						-- //
					end
				end
			end
		})
		
		if not table.isempty(settings) then
			_p('/* Begin PBXFileReference section */')
			printSettingsTable(2, settings)
			_p('/* End PBXFileReference section */')
			_p('')
		end
	end


	function xcode.PBXFrameworksBuildPhase(tr)				
		_p('/* Begin PBXFrameworksBuildPhase section */')
			for _, target in ipairs(tr.products.children) do			
				_p(2,'%s /* Frameworks */ = {', target.fxstageid)
				_p(3,'isa = PBXFrameworksBuildPhase;')
				_p(3,'buildActionMask = 2147483647;')
				_p(3,'files = (')
				
				-- write out library dependencies
				tree.traverse(tr.frameworks, {
					onleaf = function(node)
						if node.buildid and node.prjids[target.prjid] then
							_p(4,'%s /* %s in Frameworks */,', node.buildid, node.name)
						end
					end
				})
				
				-- write out project dependencies
				tree.traverse(tr.projects, {
					onleaf = function(node)
						if node.buildid and node.prjids[target.prjid] then				
							_p(4,'%s /* %s in Frameworks */,', node.buildid, node.name)
						end	
					end
				})
				
				_p(3,');')
				_p(3,'runOnlyForDeploymentPostprocessing = 0;')
				_p(2,'};')		
			end	
		_p('/* End PBXFrameworksBuildPhase section */')
		_p('')	
	end


	function xcode.PBXGroup(tr)
		local settings = {}

		tree.traverse(tr, {
			onnode = function(node)
				-- Skip over anything that isn't a proper group
				if (node.path and #node.children == 0) or node.kind == "vgroup" then
					return
				end
				
				if node.productgroupid or node.id then
					settings[node.productgroupid or node.id] = function()
						-- project references get special treatment
						if node.parent == tr.projects then
							_p(2,'%s /* Products */ = {', node.productgroupid)
						else
							_p(2,'%s /* %s */ = {', node.id, node.name)
						end
					
						_p(3,'isa = PBXGroup;')
						_p(3,'children = (')
						local size = #node.children
						
						if size > 0 then
							for _, childnode in ipairs(node.children) do
								if childnode and childnode.id then
									_p(4,'%s /* %s */,', childnode.id, childnode.name)
								end	
							end
						end	
						_p(3,');')
					
						if node.parent == tr.projects then
							_p(3,'name = Products;')
						else
							_p(3,'name = %s;', stringifySetting(node.name))
						
							local vpath = project.getvpath(tr.project, node.name)
						
							if node.path and node.name ~= vpath then
								local p = node.path
								if node.parent.path then
									p = path.getrelative(node.parent.path, node.path)
								end
								_p(3,'path = %s;', stringifySetting(p))
							end
						end
					
						_p(3,'sourceTree = "<group>";')
						_p(2,'};')
					end
				end
			end
		}, true)
		
		if not table.isempty(settings) then
			_p('/* Begin PBXGroup section */')
			printSettingsTable(2, settings)
			_p('/* End PBXGroup section */')
			_p('')
		end
	end	


	function xcode.PBXHeadersBuildPhase(tr)
		_p('/* Begin PBXHeadersBuildPhase section */')			
		for _, target in ipairs(tr.products.children) do
			if target.headers then
				_p(2,'%s /* Headers */ = {', target.headers)
				_p(3,'isa = PBXHeadersBuildPhase;')
				_p(3,'buildActionMask = 2147483647;')
				_p(3,'files = (')
				tree.traverse(tr, {
					onnode = function(node)
						if node.buildid and path.iscppheader(node.name) then
							_p(4,'%s /* %s in Headers */,', node.buildid, node.name)
						end
					end
				})
				_p(3,');')
				_p(3,'runOnlyForDeploymentPostprocessing = 0;')
				_p(2,'};')
			end
		end
		
		_p('/* End PBXHeadersBuildPhase section */')
		_p('')
	end


	function xcode.PBXNativeTarget(tr)
		_p('/* Begin PBXNativeTarget section */')
		for _, node in ipairs(tr.products.children) do
			local name = node.name
			
			-- This function checks whether there are build commands of a specific
			-- type to be executed; they will be generated correctly, but the project
			-- commands will not contain any per-configuration commands, so the logic
			-- has to be extended a bit to account for that.
			local function hasBuildCommands(which)
				-- standard check...this is what existed before
				if #tr.project[which] > 0 then
					return true
				end
				-- what if there are no project-level commands? check configs...
				for _, cfg in ipairs(node.configs) do
					if #cfg[which] > 0 then
						return true
					end
				end
			end
			
			local function hasEmbeddedBinaryOfType(which)
				local result = false
				tree.traverse(tr.binaries, {
					onnode = function(node)
						if node.prj and node.prj.kind and node.prj.kind == which then result = true end
					end
				})
				return result
			end
			
			_p(2,'%s /* %s */ = {', node.targetid, name)
			_p(3,'isa = PBXNativeTarget;')
			_p(3,'buildConfigurationList = %s /* Build configuration list for PBXNativeTarget "%s" */;', node.cfgsection, escapeSetting(name))
			_p(3,'buildPhases = (')
			if hasBuildCommands('prebuildcommands') then
				_p(4,'9607AE1010C857E500CD1376 /* Prebuild */,')
			end
			_p(4,'%s /* Resources */,', node.resstageid)
			_p(4,'%s /* Sources */,', node.sourcesid)
			if hasBuildCommands('prelinkcommands') then
				_p(4,'9607AE3510C85E7E00CD1376 /* Prelink */,')
			end
			_p(4,'%s /* Frameworks */,', node.fxstageid)
			if node.headers then
				_p(4,'%s /* Headers */,', node.headers)
			end
			if hasBuildCommands('postbuildcommands') then
				_p(4,'9607AE3710C85E8F00CD1376 /* Postbuild */,')
			end
			if node.embfwkid then
				_p(4,'%s /* Embed Frameworks */,', node.embfwkid)
			end
			if node.embextid then
				_p(4,'%s /* Embed App Extensions */,', node.embextid)
			end
			_p(3,');')
			_p(3,'buildRules = (')
			_p(3,');')
			
			_p(3,'dependencies = (')
			
			for _, child in ipairs(tr.projects.children) do
				for _, deps in ipairs(child.children) do
					if deps.prjids[node.prjid] then 
						_p(4,'%s /* PBXTargetDependency */,', child.targetdependid)
					end	
				end
			end
			
			-- put the stuff from xcodeTargetDependencies
			if node.prj and node.prj.xcodeTargetDependency then
				for _, deps in ipairs(node.prj.xcodeTargetDependency) do
				-- search other product with this name and put it targetProxy
					for _, otherProduct in ipairs(tr.products.children) do
						if deps == otherProduct.prj.name then
							_p(4,'%s /* PBXTargetDependency */,', otherProduct.targetProxyId)
						end
					end
					-- search other projects with this name and put it targetProxy
					--for _, otherProjects in ipairs(tr.projects.children) do	
					--	if(otherProjects.prj and otherProjects.prj.xcodeTarget) then															
					--		for k,v in pairs(otherProjects.prj.xcodeTarget) do
					--			print(k,v)
					--			if deps == v then
					--				_p(4,'%s /* PBXTargetDependency */,', otherProduct.targetProxyId)
					--			end														
					--		end	
					--	end							
					--end
				end
			end
			
			_p(3,');')
			
			_p(3,'name = %s;', stringifySetting(name))
			
			local p
			if node.cfg.kind == "ConsoleApp" then
				p = "$(HOME)/bin"
			elseif node.cfg.kind == "WindowedApp" then
				p = "$(HOME)/Applications"
			end
			if p then
				_p(3,'productInstallPath = %s;', stringifySetting(p))
			end
			
			_p(3,'productName = %s;', stringifySetting(name))
			_p(3,'productReference = %s /* %s */;', node.id, node.name)
			_p(3,'productType = %s;', stringifySetting(xcode.getproducttype(node)))
			_p(2,'};')
		end
		_p('/* End PBXNativeTarget section */')
		_p('')
	end

	
		
	function xcode.PBXProject(tr)
		_p('/* Begin PBXProject section */')
		_p(2,'08FB7793FE84155DC02AAC07 /* Project object */ = {')
		_p(3,'isa = PBXProject;')
		_p(3,'attributes = {')
		if #tr.products.children then
			_p(4,'TargetAttributes = {')
				for _, target in ipairs(tr.products.children) do
					if target and target.prj and target.prj.xcodeTargetAttributes then
						-- print attributes 
						_p(5,'%s = {', target.targetid)		
							local level = 6
							for key, value in pairs(target.prj.xcodeTargetAttributes) do
								--printKeyValue(level, key, value)
								printSetting(level, key, value)
							end
						_p(5,'};')		
					end	
				end
			_p(4,'};')
		end	
		_p(3,'};')
		
		_p(3,'buildConfigurationList = 1DEB928908733DD80010E9CD /* Build configuration list for PBXProject "%s" */;', tr.name)
		_p(3,'compatibilityVersion = "Xcode 3.2";')		
		_p(3,'developmentRegion = English;')
		_p(3,'hasScannedForEncodings = 1;')
		_p(3,'knownRegions  = (')
		_p(4,'	en,')
		_p(3,');')
		
		_p(3,'mainGroup = %s /* %s */;', tr.id, tr.name)
		_p(3,'productRefGroup = %s /* %s */;', tr.mainProject.children[1].productgroupid, "Products")
		_p(3,'projectDirPath = "";')
				
		local settings = {}
		
		if #tr.projects.children > 0 then
			_p(3,'projectReferences = (')
			for _, node in ipairs(tr.projects.children) do
				local name = path.getname(node.path)
				settings[name] = function()
					_p(4,'{')
						_p(5,'ProductGroup = %s /* Products */;', node.productgroupid)
						_p(5,'ProjectRef = %s /* %s */;', node.id, name)
					_p(4,'},')
				end			
			end
			printSettingsTable(4, settings)
			_p(3,');')
		end
		
		_p(3,'projectRoot = "";')
		_p(3,'targets = (')
		for _, node in ipairs(tr.products.children) do
			_p(4,'%s /* %s */,', node.targetid, node.name)
		end
		_p(3,');')
		_p(2,'};')
		_p('/* End PBXProject section */')
		_p('')
	end


	function xcode.PBXReferenceProxy(tr)
		local settings = {}
		
		tree.traverse(tr.projects, {
			onleaf = function(node)
				if node.productproxyid or node.parent.productproxyid then
					settings[node.id] = function()
						_p(2,'%s /* %s */ = {', node.id, node.name)
						_p(3,'isa = PBXReferenceProxy;')
						_p(3,'fileType = %s;', xcode.gettargettype(node.parent.prj.kind))
						_p(3,'path = %s;', stringifySetting(node.name))
						_p(3,'remoteRef = %s /* PBXContainerItemProxy */;', node.productproxyid or node.parent.productproxyid)
						_p(3,'sourceTree = BUILT_PRODUCTS_DIR;')
						_p(2,'};')
					end
				end
			end
		})
		
		if not table.isempty(settings) then
			_p('/* Begin PBXReferenceProxy section */')
			printSettingsTable(2, settings)
			_p('/* End PBXReferenceProxy section */')
			_p('')
		end
	end
	

	function xcode.PBXResourcesBuildPhase(tr)
		_p('/* Begin PBXResourcesBuildPhase section */')
		for _, target in ipairs(tr.products.children) do
			_p(2,'%s /* Resources */ = {', target.resstageid)
			_p(3,'isa = PBXResourcesBuildPhase;')
			_p(3,'buildActionMask = 2147483647;')
			_p(3,'files = (')
			tree.traverse(tr, {
				onnode = function(node)
					if target.prjid == node.prjid then
					if xcode.getbuildcategory(node) == "Resources" then
						_p(4,'%s /* %s in Resources */,', node.buildid, node.name)
					end
					end
				end
			})
			_p(3,');')
			_p(3,'runOnlyForDeploymentPostprocessing = 0;')
			_p(2,'};')
		end
		_p('/* End PBXResourcesBuildPhase section */')
		_p('')
	end
	
	function xcode.PBXShellScriptBuildPhase(tr)
		local wrapperWritten = false

		local function doblock(id, name, which)
			-- start with the project-level commands (most common)
			local prjcmds = tr.project[which]
			local commands = table.join(prjcmds, {})

			-- see if there are any config-specific commands to add
			for _, cfg in ipairs(tr.configs) do
				local cfgcmds = cfg[which]
				if #cfgcmds > #prjcmds then
					table.insert(commands, 'if [ "${CONFIGURATION}" = "' .. xcode.getconfigname(cfg) .. '" ]; then')
					for i = #prjcmds + 1, #cfgcmds do
						table.insert(commands, cfgcmds[i])
					end
					table.insert(commands, 'fi')
				end
			end
			
			if #commands > 0 then
				if not wrapperWritten then
					_p('/* Begin PBXShellScriptBuildPhase section */')
					wrapperWritten = true
				end
				_p(2,'%s /* %s */ = {', id, name)
				_p(3,'isa = PBXShellScriptBuildPhase;')
				_p(3,'buildActionMask = 2147483647;')
				_p(3,'files = (')
				_p(3,');')
				_p(3,'inputPaths = (');
				_p(3,');');
				_p(3,'name = %s;', name);
				_p(3,'outputPaths = (');
				_p(3,');');
				_p(3,'runOnlyForDeploymentPostprocessing = 0;');
				_p(3,'shellPath = /bin/sh;');
				_p(3,'shellScript = %s;', stringifySetting(table.concat(commands, '\n')))
				_p(2,'};')
			end
		end
				
		doblock("9607AE1010C857E500CD1376", "Prebuild", "prebuildcommands")
		doblock("9607AE3510C85E7E00CD1376", "Prelink", "prelinkcommands")
		doblock("9607AE3710C85E8F00CD1376", "Postbuild", "postbuildcommands")
		
		if wrapperWritten then
			_p('/* End PBXShellScriptBuildPhase section */')
		end
	end
	
	
	function xcode.PBXSourcesBuildPhase(tr)
		_p('/* Begin PBXSourcesBuildPhase section */')
		for _, target in ipairs(tr.products.children) do
			_p(2,'%s /* Sources */ = {', target.sourcesid)
			_p(3,'isa = PBXSourcesBuildPhase;')
			_p(3,'buildActionMask = 2147483647;')
			_p(3,'files = (')
			tree.traverse(tr, {
				onleaf = function(node)
					if target.prjid == node.prjid then
						if xcode.getbuildcategory(node) == "Sources" then
							_p(4,'%s /* %s in Sources */,', node.buildid, node.name)
						end
					end	
				end
			})
			_p(3,');')
			_p(3,'runOnlyForDeploymentPostprocessing = 0;')
			_p(2,'};')
		end
		_p('/* End PBXSourcesBuildPhase section */')
		_p('')
	end


	function xcode.PBXVariantGroup(tr)
		local settings = {}
		tree.traverse(tr, {
			onbranch = function(node)
				if node.id then
					settings[node.id] = function()
						if node.kind == "vgroup" then
							_p(2,'%s /* %s */ = {', node.id, node.name)
							_p(3,'isa = PBXVariantGroup;')
							_p(3,'children = (')
							for _, lang in ipairs(node.children) do
								_p(4,'%s /* %s */,', lang.id, lang.name)
							end
							_p(3,');')
							_p(3,'name = %s;', node.name)
							_p(3,'sourceTree = "<group>";')
							_p(2,'};')
						end
					end
				end	
			end
		})
		
		if not table.isempty(settings) then
			_p('/* Begin PBXVariantGroup section */')
			printSettingsTable(2, settings)
			_p('/* End PBXVariantGroup section */')
			_p('')
		end
	end


	function xcode.PBXTargetDependency(tr)
		local settings = {}
		
		local onleafFunc = function(node)
				local dependid = node.parent.targetdependid
				if dependid then
					settings[dependid] = function()
						_p(2,'%s /* PBXTargetDependency */ = {', dependid)
						_p(3,'isa = PBXTargetDependency;')
						_p(3,'name = %s;', stringifySetting(path.getbasename(node.parent.name)))
						_p(3,'targetProxy = %s /* PBXContainerItemProxy */;', node.parent.targetproxyid)
						_p(2,'};')
					end	
				end
			end
			
		for _, node in ipairs(tr.products.children) do
			if node.targetProxyId then
				settings[node.targetProxyId] = function()
						_p(2,'%s /* PBXTargetDependency */ = {', node.targetProxyId)
						_p(3,'isa = PBXTargetDependency;')
						_p(3,'target = %s /*%s */;', node.targetid, stringifySetting(node.name))
						_p(3,'targetProxy = %s /* PBXContainerItemProxy */;', node.containerProxyId)
						_p(2,'};')
				end	
			end	
		end
		
		tree.traverse(tr.mainProject, {
			onleaf = onleafFunc
		})
		
		tree.traverse(tr.projects, {
			onleaf = onleafFunc
		})

		if not table.isempty(settings) then
			_p('/* Begin PBXTargetDependency section */')
			printSettingsTable(2, settings)
			_p('/* End PBXTargetDependency section */')
			_p('')
		end
	end
	
	-- // this function generates all the all the default settings
	-- 'target' parameter if nil means 'project', otherwise it's 'target'
	function xcode.DefaultSettings(tr, cfg, target)
		local settings = {}

		-- // COMMON SETTINGS
		-- Architectures
		local archs = {
			Native = "$(NATIVE_ARCH_ACTUAL)",
			x32    = "i386",
			x64    = "x86_64",
			Universal32 = "$(ARCHS_STANDARD_32_BIT)",
			Universal64 = "$(ARCHS_STANDARD_64_BIT)",
			Universal = "$(ARCHS_STANDARD_32_64_BIT)",
		}
		
		settings['ARCHS'] = archs[cfg.platform or "Native"] -- also in addCommonXcodeBuildSettings()			 
		settings['SDKROOT'] = 'iphoneos' -- also in addCommonXcodeBuildSettings()
	
		settings['ONLY_ACTIVE_ARCH'] = iif(premake.config.isDebugBuild(cfg), 'YES', 'NO') -- also in addCommonXcodeBuildSettings()											
--		settings['VALID_ARCHS'] = 'armv7 armv7s arm64' -- also in addCommonXcodeBuildSettings()
--		settings['TARGETED_DEVICE_FAMILY'] = '1,2' -- also in addCommonXcodeBuildSettings()
--		settings['IPHONEOS_DEPLOYMENT_TARGET'] = '6.0' -- also in addCommonXcodeBuildSettings()
		
		-- Build Locations
		local targetdir = path.getdirectory(cfg.buildtarget.relpath)
		if targetdir ~= "." then
			settings['SYMROOT'] = path.getrelative(tr.project.location, targetdir)
		end
		local objDir = path.getrelative(tr.project.location, cfg.objdir) 
		settings['OBJROOT'] = objDir

		if target then
			local outdir = path.getrelative(target.prj.location, targetdir)
			if outdir ~= "." then
				settings['CONFIGURATION_BUILD_DIR'] = outdir
			end
		end
		if targetdir ~= "." then
			settings['CONFIGURATION_BUILD_DIR'] = '$(SYMROOT)'
		end
		settings['CONFIGURATION_TEMP_DIR'] = '$(OBJROOT)'
		
		-- Build Options
--		settings['GCC_VERSION'] = 'com.apple.compilers.llvm.clang.1_0' -- also in addCommonXcodeBuildSettings()
		if not cfg.flags.Symbols then
--				settings['GCC_GENERATE_DEBUGGING_SYMBOLS'] = 'YES' -- NEW
--				settings['GCC_DEBUGGING_SYMBOLS']          = 'full' -- NEW
			settings['DEBUG_INFORMATION_FORMAT'] = 'dwarf-with-dsym' -- maybe we should put 'dwarf' on 'Release'
		end
		
		-- Code Signing
		settings['CODE_SIGN_IDENTITY'] = 'iPhone Developer: Valeri Vuchov (WDTMDP2J2J)'
		
		-- Deployment
--		settings['DSTROOT'] = '/tmp/$(PROJECT_NAME).dst' -- NEW
		installpaths = {
			ConsoleApp = '/usr/local/bin',
			WindowedApp = '"$(HOME)/Applications"',
			SharedLib = '/usr/local/lib',
			StaticLib = '/usr/local/lib',
		}
		settings['INSTALL_PATH'] = installpaths[cfg.kind]
		if cfg.flags.Symbols then
			settings['COPY_PHASE_STRIP'] = 'NO'
--			settings['STRIP_INSTALLED_PRODUCT'] = 'NO' -- NEW
		end
		
		-- this must be set to YES for all librarires and to NO for APPLICATIONS
		settings["SKIP_INSTALL"] = 'YES' -- also in addCommonXcodeBuildSettings()
		
		-- Linking
--		settings['PREBINDING'] = 'NO' -- NEW
		settings['GCC_SYMBOLS_PRIVATE_EXTERN'] = 'NO' -- also in addCommonXcodeBuildSettings()
		if cfg.flags.StaticRuntime then
			settings['STANDARD_C_PLUS_PLUS_LIBRARY_TYPE'] = 'static'
		end
	
		-- // TARGET ONLY
		if target then
			-- Packaging
			local fileNameList = {}
			local file_tree = project.getsourcetree(target.prj) 
			tree.traverse(tr, {
					onnode = function(node)
						if node.buildid and not node.isResource and node.abspath then
							-- ms this seems to work on visual studio !!!
							-- why not in xcode ??
							local filecfg = fileconfig.getconfig(node, cfg)
							if filecfg and filecfg.flags.ExcludeFromBuild then
							--fileNameList = fileNameList .. " " ..filecfg.name
								table.insert(fileNameList, escapeArg(node.name))
							end
							
							--ms new way 
							-- if the file is not in this config file list excluded it from build !!!
							--if not cfg.files[node.abspath] then
							--	table.insert(fileNameList, escapeArg(node.name))
							--end
						end						
					end
				}) 
				
			if not table.isempty(fileNameList) then
				settings['EXCLUDED_SOURCE_FILE_NAMES'] = fileNameList
			end
			
			if target.infoplist then
				settings['INFOPLIST_FILE'] = config.findfile(cfg, path.getextension(target.infoplist.name))
			end
			
			settings['PRODUCT_NAME'] = cfg.buildtarget.basename
		end
		-- //
	
		-- Search Paths			
		for i,v in ipairs(cfg.includedirs) do
			cfg.includedirs[i] = premake.project.getrelative(cfg.project, cfg.includedirs[i])
		end
	
		settings['COMMON_HEADER_SEARCH_PATHS'] = cfg.includedirs

		for i,v in ipairs(cfg.libdirs) do
		cfg.libdirs[i] = premake.project.getrelative(cfg.project, cfg.libdirs[i])
		end
		
		settings['LIBRARY_SEARCH_PATHS'] = cfg.libdirs
		if cfg.frameworkdirs then
			for i,v in ipairs(cfg.frameworkdirs) do
				cfg.frameworkdirs[i] = premake.project.getrelative(cfg.project, cfg.frameworkdirs[i])
			end
		end
		
		local function tableSplit(flags, array)					
			if (type(array) == "table") then
				for k,v in ipairs(array) do			
					for _k, _v in string.gmatch(v, "%S+") do
						table.insert(flags, _k)
					end	
				end
			end	
			return flags
		end	
		
		local fd = tableSplit({}, cfg.frameworkdirs)
		settings['FRAMEWORK_SEARCH_PATHS'] = fd	
	
		settings['ALWAYS_SEARCH_USER_PATHS'] = 'NO'
		settings['HEADER_SEARCH_PATHS'] = '$(COMMON_HEADER_SEARCH_PATHS)'
		
		-- Code Generation
		local optimizeMap = { On = 3, Size = 's', Speed = 3, Full = 'fast', Debug = 1 }
		settings['GCC_OPTIMIZATION_LEVEL'] = optimizeMap[cfg.optimize] or 0
		
		if cfg.flags.Symbols then
--			settings['GCC_AUTO_VECTORIZATION'] = 'NO' -- NEW
			settings['GCC_DYNAMIC_NO_PIC'] = 'NO' -- also in addCommonXcodeBuildSettings()
			if not cfg.flags.NoEditAndContinue then
				settings['GCC_ENABLE_FIX_AND_CONTINUE'] = 'YES'
			end
--			settings['GCC_ENABLE_KERNEL_DEVELOPMENT'] = 'NO' -- NEW
--			settings['GCC_ENABLE_OBJC_GC'] = 'unsupported' -- NEW
--			settings['GCC_ENABLE_SSE3_EXTENSIONS'] = 'NO' -- NEW
--			settings['GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS'] = 'NO' -- NEW
--			settings['GCC_ENABLE_SYMBOL_SEPARATION'] = 'YES' -- NEW
--			settings['GCC_FAST_MATH'] = 'NO' -- NEW
--			settings['GCC_FAST_OBJC_DISPATCH'] = 'YES' -- NEW
--			settings['GCC_FEEDBACK_DIRECTED_OPTIMIZATION'] = 'Off' -- NEW
--			settings['GCC_GENERATE_TEST_COVERAGE_FILES'] = 'NO' -- NEW
--			settings['GCC_INLINES_ARE_PRIVATE_EXTERN'] = 'YES' -- NEW
--			settings['GCC_INSTRUMENT_PROGRAM_FLOW_ARCS'] = 'NO' -- NEW
--			settings['GCC_MODEL_PPC64'] = 'NO' -- NEW
			settings['GCC_MODEL_TUNING'] = 'G5'
--			settings['GCC_NO_COMMON_BLOCKS'] = 'NO' -- NEW
--			settings['GCC_OBJC_CALL_CXX_CDTORS'] = 'NO' -- NEW
--			settings['GCC_REUSE_STRINGS'] = 'YES' -- NEW
--			settings['GCC_STRICT_ALIASING'] = 'NO' -- NEW
--			settings['GCC_THREADSAFE_STATICS'] = 'YES' -- NEW
--			settings['GCC_THUMB_SUPPORT'] = 'NO' -- NEW
--			settings['GCC_UNROLL_LOOPS'] = 'NO' -- NEW
		end
		
		-- Custom Compiler / other c++ Flags
		local checks = {
			["-ffast-math"]          = cfg.flags.FloatFast,
			["-ffloat-store"]        = cfg.flags.FloatStrict,
			["-fomit-frame-pointer"] = cfg.flags.NoFramePointer,
		}
			
		local flags = { }
		for flag, check in pairs(checks) do
			if check then
				table.insert(flags, flag)
			end
		end
		
		settings['OTHER_CFLAGS'] = tableSplit(flags, cfg.buildoptions)
		
		-- build list of "other" linked flags. All libraries that aren't frameworks
		-- are listed here, so I don't have to try and figure out if they are ".a"
		-- or ".dylib", which Xcode requires to list in the Frameworks section
		flags = { }
		for _, lib in ipairs(config.getlinks(cfg, "system")) do
			if not xcode.isframework(lib) then
				table.insert(flags, "-l" .. lib)
			end
		end
		
		--ms this step is for reference projects only
		for _, lib in ipairs(config.getlinks(cfg, "dependencies", "object")) do	
			if (lib.external) then
				if not xcode.isframework(lib.linktarget.basename) then
					table.insert(flags, "-l" .. escapeArg(lib.linktarget.basename))
				end
			end 
		end
						
		settings['OTHER_LDFLAGS'] = tableSplit(flags, cfg.linkoptions) 
		
		-- Language
--		settings['GCC_ALTIVEC_EXTENSIONS'] = 'NO' -- NEW
--		settings['GCC_CHAR_IS_UNSIGNED_CHAR'] = 'NO' -- NEW
--		settings['GCC_CHECK_RETURN_VALUE_OF_OPERATOR_NEW'] = 'NO' -- NEW
--		settings['GCC_CW_ASM_SYNTAX'] = 'YES' -- NEW
		settings['GCC_C_LANGUAGE_STANDARD'] = 'gnu99' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_ENABLE_ASM_KEYWORD'] = 'YES' -- NEW
--		settings['GCC_ENABLE_BUILTIN_FUNCTIONS'] = 'YES' -- NEW
		if cfg.flags.NoExceptions then
			settings['GCC_ENABLE_CPP_EXCEPTIONS'] = 'NO'
			settings['GCC_ENABLE_OBJC_EXCEPTIONS'] = 'NO'
		end
		if cfg.flags.NoRTTI then
			settings['GCC_ENABLE_CPP_RTTI'] = 'NO'
		end
--		settings['GCC_ENABLE_FLOATING_POINT_LIBRARY_CALLS'] = 'NO' -- NEW
--		settings['GCC_ENABLE_PASCAL_STRINGS'] = 'YES' -- NEW
--		settings['GCC_ENABLE_TRIGRAPHS'] = 'NO' -- NEW
		if cfg.xcodepchheader and not cfg.flags.NoPCH then
			settings['GCC_PRECOMPILE_PREFIX_HEADER'] = 'YES'
			settings['GCC_PREFIX_HEADER'] = cfg.xcodepchheader
		end
--		settings['GCC_INCREASE_PRECOMPILED_HEADER_SHARING'] = 'NO' -- NEW
--		settings['GCC_FORCE_CPU_SUBTYPE_ALL'] = 'NO' -- NEW
--		settings['GCC_INPUT_FILETYPE'] = 'automatic' -- NEW
--		settings['GCC_LINK_WITH_DYNAMIC_LIBRARIES'] = 'YES' -- NEW
--		settings['GCC_ONE_BYTE_BOOL'] = 'YES' -- NEW
--		settings['GCC_PFE_FILE_C_DIALECTS'] = 'c objective-c c++ objective-c++' -- NEW
--		settings['GCC_SHORT_ENUMS'] = 'NO' -- NEW
--		settings['GCC_USE_GCC3_PFE_SUPPORT'] = 'YES' -- NEW
--		settings['GCC_USE_INDIRECT_FUNCTION_CALLS'] = 'NO' -- NEW
--		settings['GCC_USE_REGISTER_FUNCTION_CALLS'] = 'NO' -- NEW
--		settings['GCC_USE_STANDARD_INCLUDE_SEARCHING'] = 'YES' -- NEW

		-- Language C++
--		settings['CLANG_CXX_LANGUAGE_STANDARD'] = 'c++11' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_CXX_LIBRARY'] = 'libc++' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN_BOOL_CONVERSION'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN_CONSTANT_CONVERSION'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN_EMPTY_BODY'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN_ENUM_CONVERSION'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN_INT_CONVERSION'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN_UNREACHABLE_CODE'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['CLANG_WARN__DUPLICATE_METHOD_MATCH'] = 'YES' -- also in addCommonXcodeBuildSettings()
		
		-- Preprocessing
--		settings['ENABLE_STRICT_OBJC_MSGSEND'] = 'YES' -- also in addCommonXcodeBuildSettings()
		
		-- Preprocesor Definitions
		settings['GCC_PREPROCESSOR_DEFINITIONS'] = cfg.defines
		
		-- Executable
		if cfg.kind ~= "StaticLib" and cfg.buildtarget.prefix ~= '' then
			settings['EXECUTABLE_PREFIX'] = cfg.buildtarget.prefix
		end
		
		--[[if cfg.targetextension then
			local ext = cfg.targetextension
			ext = iif(ext:startswith('.'), ext:sub(2), ext)
			settings['EXECUTABLE_EXTENSION'] = ext
		end]]
		
		-- Warnings
--		settings['GCC_TREAT_IMPLICIT_FUNCTION_DECLARATIONS_AS_ERRORS'] = 'YES' -- NEW
--		settings['GCC_TREAT_NONCONFORMANT_CODE_ERRORS_AS_WARNINGS'] = 'NO' -- NEW
		if cfg.flags.FatalWarnings then
			settings['GCC_TREAT_WARNINGS_AS_ERRORS'] = 'YES'
		end
--		settings['GCC_WARN_64_TO_32_BIT_CONVERSION'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO'] = 'YES' -- NEW
--		settings['GCC_WARN_ABOUT_MISSING_NEWLINE'] = 'NO' -- NEW
--		settings['GCC_WARN_ABOUT_MISSING_PROTOTYPES'] = 'NO' -- NEW
--		settings['GCC_WARN_ABOUT_POINTER_SIGNEDNESS'] = 'YES' -- NEW
		settings['GCC_WARN_ABOUT_RETURN_TYPE'] = 'YES'
--		settings['GCC_WARN_ALLOW_INCOMPLETE_PROTOCOL'] = 'YES' -- NEW
--		settings['GCC_WARN_CHECK_SWITCH_STATEMENTS'] = 'NO' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_WARN_EFFECTIVE_CPLUSPLUS_VIOLATIONS'] = 'NO' -- NEW
--		settings['GCC_WARN_FOUR_CHARACTER_CONSTANTS'] = 'NO' -- NEW
--		settings['GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS'] = 'NO' -- NEW	
--		settings['GCC_WARN_INHIBIT_ALL_WARNINGS'] = 'NO' -- NEW
--		settings['GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED'] = 'NO' -- NEW
--		settings['GCC_WARN_MISSING_PARENTHESES'] = 'NO' -- NEW
--		settings['GCC_WARN_NON_VIRTUAL_DESTRUCTOR'] = 'YES' -- NEW
--		settings['GCC_WARN_PEDANTIC'] = 'NO' -- NEW
--		settings['GCC_WARN_PROTOTYPE_CONVERSION']= 'NO' -- NEW
--		settings['GCC_WARN_SHADOW'] = 'NO' -- NEW
--		settings['GCC_WARN_SIGN_COMPARE'] = 'YES' -- NEW
--		settings['GCC_WARN_TYPECHECK_CALLS_TO_PRINTF'] = 'YES' -- NEW
--		settings['GCC_WARN_UNDECLARED_SELECTOR'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_WARN_UNINITIALIZED_AUTOS'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_WARN_UNKNOWN_PRAGMAS'] = 'NO' -- NEW
--		settings['GCC_WARN_UNUSED_FUNCTION'] = 'YES' -- also in addCommonXcodeBuildSettings()
--		settings['GCC_WARN_UNUSED_LABEL'] = 'YES' -- NEW
--		settings['GCC_WARN_UNUSED_PARAMETER'] = 'NO' -- NEW
--		settings['GCC_WARN_UNUSED_VALUE'] = 'NO' -- NEW
		settings['GCC_WARN_UNUSED_VARIABLE'] = 'YES' -- also in addCommonXcodeBuildSettings()
		if cfg.warnings == "Extra" then
			settings['WARNING_CFLAGS'] = '-Wall'
		end
		
		return settings
	end
-- //	
	
	function xcode.XCBuildConfiguration_Target(tr, target, cfg)
		local settings = {}

---------- these settings are no longer used !!! ---------------------
--[=====[
		settings['ALWAYS_SEARCH_USER_PATHS'] = 'NO'
		
		if not cfg.flags.Symbols then
			settings['DEBUG_INFORMATION_FORMAT'] = 'dwarf-with-dsym'
		end
		
		if cfg.kind ~= "StaticLib" and cfg.buildtarget.prefix ~= '' then
			settings['EXECUTABLE_PREFIX'] = cfg.buildtarget.prefix
		end
		
		--[[if cfg.targetextension then
			local ext = cfg.targetextension
			ext = iif(ext:startswith('.'), ext:sub(2), ext)
			settings['EXECUTABLE_EXTENSION'] = ext
		end]]
		
		local outdir = path.getrelative(target.prj.location, path.getdirectory(cfg.buildtarget.relpath))
		if outdir ~= "." then
			settings['CONFIGURATION_BUILD_DIR'] = outdir
		end

		settings['GCC_DYNAMIC_NO_PIC'] = 'NO'
		settings['GCC_MODEL_TUNING'] = 'G5'

		if target.infoplist then
			settings['INFOPLIST_FILE'] = config.findfile(cfg, path.getextension(target.infoplist.name))
		end
					
		installpaths = {
			ConsoleApp = '/usr/local/bin',
			WindowedApp = '"$(HOME)/Applications"',
			SharedLib = '/usr/local/lib',
			StaticLib = '/usr/local/lib',
		}
		settings['INSTALL_PATH'] = installpaths[cfg.kind]
		
		local fileNameList = {}
		local file_tree = project.getsourcetree(target.prj) 
		tree.traverse(tr, {
				onnode = function(node)
					if node.buildid and not node.isResource and node.abspath then
						-- ms this seems to work on visual studio !!!
						-- why not in xcode ??
						local filecfg = fileconfig.getconfig(node, cfg)
						if filecfg and filecfg.flags.ExcludeFromBuild then
						--fileNameList = fileNameList .. " " ..filecfg.name
							table.insert(fileNameList, escapeArg(node.name))
						end
						
						--ms new way 
						-- if the file is not in this config file list excluded it from build !!!
						--if not cfg.files[node.abspath] then
						--	table.insert(fileNameList, escapeArg(node.name))
						--end
					end						
				end
			}) 
		
		if not table.isempty(fileNameList) then
			settings['EXCLUDED_SOURCE_FILE_NAMES'] = fileNameList
		end
		settings['PRODUCT_NAME'] = cfg.buildtarget.basename
		
		--ms not by default ...add it manually if you need it 
		--settings['COMBINE_HIDPI_IMAGES'] = 'YES'
		
		local archs = {
			Native = "$(NATIVE_ARCH_ACTUAL)",
			x32    = "i386",
			x64    = "x86_64",
			Universal32 = "$(ARCHS_STANDARD_32_BIT)",
			Universal64 = "$(ARCHS_STANDARD_64_BIT)",
			Universal = "$(ARCHS_STANDARD_32_64_BIT)",
		}
		
		settings['ARCHS'] = archs[cfg.platform or "Native"]
		
		--ms This is the default so don;t write it		
		--settings['SDKROOT'] = 'macosx'
				
		local targetdir = path.getdirectory(cfg.buildtarget.relpath)
		if targetdir ~= "." then
			settings['CONFIGURATION_BUILD_DIR'] = '$(SYMROOT)'
		end
		
		settings['CONFIGURATION_TEMP_DIR'] = '$(OBJROOT)'

		if cfg.flags.Symbols then
			settings['COPY_PHASE_STRIP'] = 'NO'
		end
		
		settings['GCC_C_LANGUAGE_STANDARD'] = 'gnu99'
		
		if cfg.flags.NoExceptions then
			settings['GCC_ENABLE_CPP_EXCEPTIONS'] = 'NO'
		end
		
		if cfg.flags.NoRTTI then
			settings['GCC_ENABLE_CPP_RTTI'] = 'NO'
		end
		
		if cfg.flags.Symbols and not cfg.flags.NoEditAndContinue then
			settings['GCC_ENABLE_FIX_AND_CONTINUE'] = 'YES'
		end
		
		if cfg.flags.NoExceptions then
			settings['GCC_ENABLE_OBJC_EXCEPTIONS'] = 'NO'
		end
		
		local optimizeMap = { On = 3, Size = 's', Speed = 3, Full = 'fast', Debug = 1 }
		settings['GCC_OPTIMIZATION_LEVEL'] = optimizeMap[cfg.optimize] or 0
			
		if cfg.xcodepchheader and not cfg.flags.NoPCH then
			settings['GCC_PRECOMPILE_PREFIX_HEADER'] = 'YES'
			settings['GCC_PREFIX_HEADER'] = cfg.xcodepchheader
		end
		
		settings['GCC_PREPROCESSOR_DEFINITIONS'] = cfg.defines

		settings["GCC_SYMBOLS_PRIVATE_EXTERN"] = 'NO' 
		
		if cfg.flags.FatalWarnings then
			settings['GCC_TREAT_WARNINGS_AS_ERRORS'] = 'YES'
		end
		
		settings['GCC_WARN_ABOUT_RETURN_TYPE'] = 'YES'
		settings['GCC_WARN_UNUSED_VARIABLE'] = 'YES'

		for i,v in ipairs(cfg.includedirs) do
			cfg.includedirs[i] = premake.project.getrelative(cfg.project, cfg.includedirs[i])
		end
		
		settings['HEADER_SEARCH_PATHS'] = cfg.includedirs
		
		for i,v in ipairs(cfg.libdirs) do
			cfg.libdirs[i] = premake.project.getrelative(cfg.project, cfg.libdirs[i])
		end
		settings['LIBRARY_SEARCH_PATHS'] = cfg.libdirs
		if cfg.frameworkdirs then
			for i,v in ipairs(cfg.frameworkdirs) do
				cfg.frameworkdirs[i] = premake.project.getrelative(cfg.project, cfg.frameworkdirs[i])
			end
		end	
		
		local function tableSplit(flags, array)					
			if (type(array) == "table") then
				for k,v in ipairs(array) do			
					for _k, _v in string.gmatch(v, "%S+") do
						table.insert(flags, _k)
					end	
				end
			end	
			return flags
		end	
		
		local fd = tableSplit({}, cfg.frameworkdirs)
		settings['FRAMEWORK_SEARCH_PATHS'] = fd

		local objDir = path.getrelative(tr.project.location, cfg.objdir) 
		settings['OBJROOT'] = objDir

		settings['ONLY_ACTIVE_ARCH'] = iif(premake.config.isDebugBuild(cfg), 'YES', 'NO')
		
		-- build list of "other" C/C++ flags
		local checks = {
			["-ffast-math"]          = cfg.flags.FloatFast,
			["-ffloat-store"]        = cfg.flags.FloatStrict,
			["-fomit-frame-pointer"] = cfg.flags.NoFramePointer,
		}
			
		local flags = { }
		for flag, check in pairs(checks) do
			if check then
				table.insert(flags, flag)
			end
		end
		
		settings['OTHER_CFLAGS'] = tableSplit(flags, cfg.buildoptions) -- table.join(flags, bo)

		-- build list of "other" linked flags. All libraries that aren't frameworks
		-- are listed here, so I don't have to try and figure out if they are ".a"
		-- or ".dylib", which Xcode requires to list in the Frameworks section
		flags = { }
		for _, lib in ipairs(config.getlinks(cfg, "system")) do
			if not xcode.isframework(lib) then
				table.insert(flags, "-l" .. lib)
			end
		end
		
		--ms this step is for reference projects only
		for _, lib in ipairs(config.getlinks(cfg, "dependencies", "object")) do	
			if (lib.external) then
				if not xcode.isframework(lib.linktarget.basename) then
					table.insert(flags, "-l" .. escapeArg(lib.linktarget.basename))
				end
			end 
		end
						
		settings['OTHER_LDFLAGS'] = tableSplit(flags, cfg.linkoptions) 
		
		if cfg.flags.StaticRuntime then
			settings['STANDARD_C_PLUS_PLUS_LIBRARY_TYPE'] = 'static'
		end
		
		if targetdir ~= "." then
			settings['SYMROOT'] = path.getrelative(tr.project.location, targetdir)
		end
		
		if cfg.warnings == "Extra" then
			settings['WARNING_CFLAGS'] = '-Wall'
		end
		
		--overrideSettings(settings, cfg.xcodebuildsettings)
--]=====]		
		-- // 
		-- generate settings for 'target'
		settings = xcode.DefaultSettings(tr, cfg, target)
		

		-- keep the possibility to override settings
		overrideSettings(settings, cfg.xcodebuildsettings)
		
		local fname, ufname, commonfname
		
		-- create .xcconfig file path
		if cfg.generatexcconfigs == 'YES' then
			local configdir = 'xcconfigs_' .. cfg.project.name
			local fpath = tr.project.project.location or tr.project.project.basedir
			fname = path.join(fpath, configdir)
			ufname = path.join(fpath, configdir .. '/user')
		end
		-- //
			
		local cfgname = xcode.getconfigname(cfg)
		_p(2,'%s /* %s */ = {', cfg.xcode.targetid, cfgname)
		_p(3,'isa = XCBuildConfiguration;')
		-- // added xcconfigs files to Target also
		if cfg.generatexcconfigs == 'YES' then
			local fileid, filename = xcode.getbaseconfigurationreference(tr, cfgname, 'target')
			if fileid and filename then
				_p(3,'baseConfigurationReference = %s /* %s */;', fileid, filename)
				-- // generate the .xcconfig full file names	 
				fname = path.join(fname, filename)	
				ufname = path.join(ufname, 'user-' .. filename)
				-- //
			end
		elseif cfg.configurationfiles then -- support for 'common.xcconfig' 
			local fileid, filename = xcode.getbaseconfigurationreference(tr, cfgname, 'common') -- support for old 'common.xcconfig'
			if fileid and filename then
				_p(3,'baseConfigurationReference = %s /* %s */;', fileid, filename)
				-- // generate the .xcconfig full file name	 
				commonfname = filename --path.join(commonfname, filename)
				-- //
			end
		end
		-- //
		
		_p(3,'buildSettings = {')		
		if cfg.generatexcconfigs == 'NO' or cfg.generatexcconfigs == nil then -- // if using config files, all the settings will be in these files and no longer in the .pbxproj file !
			printSettingsTable(4, settings)
		end -- //
		_p(3,'};')
		printSetting(3, 'name', cfgname);
		_p(2,'};')
		
		-- //
		if cfg.generatexcconfigs == 'YES' then
			-- save the current output descriptor
			local temp = io.output()
			
			-- create the 'default' target .xcconfig file for the current configuration
			printf("Generating %s...", path.getrelative(os.getcwd(), fname)) -- print message to console
			local file = io.open(fname, "w")
			io.output(file)

			-- write the generated settings
			customPrintSettingsTable(0, settings)
	
			-- write the path to 'user' target .xcconfig file
			if cfg.configurationfiles then
				for _, filename in ipairs(cfg.configurationfiles) do
					if string.find(path.getbasename(filename), 'target') and string.find(path.getbasename(filename), string.lower(cfgname)) and string.find(filename, '.xcconfig') then						
						io.write('#include "' .. premake.project.getrelative(cfg.project, filename) .. '"')
						break
					end
				end	
			end
			io.close(file)
			
			-- create the empty 'user' target .xcconfig file for the current configuration
			printf("Generating %s...", path.getrelative(os.getcwd(), ufname)) -- print message to console
			local ufile = io.open(ufname, "w")
			io.output(ufile)
			io.close(ufile)
			
			-- load the previous saved output descriptor
			io.output(temp)			
		-- support for old 'common.xcconfig'	
		elseif cfg.configurationfiles and commonfname and string.find(commonfname, '.xcconfig') then
			local configdir = 'xcconfig'
			local fpath = tr.project.project.location or tr.project.project.basedir
			commonfname = path.join(path.join(fpath, configdir), commonfname)
		
			-- save the current output descriptor
			local temp = io.output()
		
			-- create the empty 'common user' target .xcconfig file
			printf("Generating %s...", path.getrelative(os.getcwd(), commonfname)) -- print message to console
			local ufile = io.open(commonfname, "w")
			io.output(ufile)
			io.close(ufile)
			
			-- load the previous saved output descriptor
			io.output(temp)			
		end
		-- //
		
	end
	
	
	function xcode.XCBuildConfiguration_Project(tr, cfg)
		local settings = {}
		
		-- // 
		-- generate setings for 'project'
		settings = xcode.DefaultSettings(tr, cfg, nil)
		
		-- keep the possibility to override settings
		overrideSettings(settings, cfg.xcodebuildsettings)
		
		local fname, ufname
		
		-- create .xcconfig file path
		if cfg.generatexcconfigs == 'YES' then
			local configdir = 'xcconfigs_' .. cfg.project.name
			local fpath = tr.project.project.location or tr.project.project.basedir
			fname = path.join(fpath, configdir)
			ufname = path.join(fpath, configdir .. '/user')
		end
		-- //
		
		local cfgname = xcode.getconfigname(cfg)
		_p(2,'%s /* %s */ = {', cfg.xcode.projectid, cfgname)
		_p(3,'isa = XCBuildConfiguration;')
		if cfg.generatexcconfigs == 'YES' then	
			local fileid, filename = xcode.getbaseconfigurationreference(tr, cfgname, 'project')
			if fileid and filename then
				_p(3,'baseConfigurationReference = %s /* %s */;', fileid, filename)
				 -- // generate the .xcconfig full file names
				fname = path.join(fname, filename)
				ufname = path.join(ufname, 'user-' .. filename)
				-- //
			end	
		end
		
		_p(3,'buildSettings = {')
		if cfg.generatexcconfigs == 'NO' and (cfg.xcodeNoProjectSettings == false or cfg.xcodeNoProjectSettings == nil) then -- // if using config files, all the settings will be in these files and no longer in the .pbxproj file !		
			printSettingsTable(4, settings)
		end -- //
		_p(3,'};')
		printSetting(3, 'name', cfgname);
		_p(2,'};')
		
		-- //
		if cfg.generatexcconfigs == 'YES' then
			-- save the current output descriptor
			local temp = io.output()
			
			-- create the default project .xcconfig file for the current configuration
			printf("Generating %s...", path.getrelative(os.getcwd(), fname)) -- print message to console
			local file = io.open(fname, "w")
			io.output(file)

			-- write the generated settings
			customPrintSettingsTable(0, settings)

			-- write the path to 'user' project .xcconfig file
			if cfg.configurationfiles then
				for _, filename in ipairs(cfg.configurationfiles) do
					if string.find(path.getbasename(filename), 'project') and string.find(path.getbasename(filename), string.lower(cfgname)) and string.find(filename, '.xcconfig') then
						io.write('#include "' .. premake.project.getrelative(cfg.project, filename) .. '"')
						break
					end
				end	
			end
			io.close(file)
			
			-- create the empty 'user' project .xcconfig file for the current configuration
			printf("Generating %s...", path.getrelative(os.getcwd(), ufname)) -- print message to console
			local ufile = io.open(ufname, "w")
			io.output(ufile)
			io.close(ufile)
			
			-- load the previous saved output descriptor
			io.output(temp)
		end
		-- //
	end


	function xcode.XCBuildConfiguration(tr)
		local settings = {}
		
		for _, target in ipairs(tr.products.children) do
			for _, cfg in ipairs(target.configs) do
				settings[cfg.xcode.targetid] = function()
					xcode.XCBuildConfiguration_Target(tr, target, cfg)
				end
			end
		end
		
		for _, cfg in ipairs(tr.configs) do
			settings[cfg.xcode.projectid] = function()
				xcode.XCBuildConfiguration_Project(tr, cfg)
			end
		end
		
		if not table.isempty(settings) then
			_p('/* Begin XCBuildConfiguration section */')
			printSettingsTable(0, settings)
			_p('/* End XCBuildConfiguration section */')
			_p('')
		end
	end


	function xcode.XCBuildConfigurationList(tr)
		local sln = tr.project.solution
		local defaultCfgName = stringifySetting(xcode.getconfigname(tr.configs[1]))
		local settings = {}
		
		for _, target in ipairs(tr.products.children) do
			settings[target.cfgsection] = function()
				_p(2,'%s /* Build configuration list for PBXNativeTarget "%s" */ = {', target.cfgsection, target.name)
				_p(3,'isa = XCConfigurationList;')
				_p(3,'buildConfigurations = (')
				for _, cfg in ipairs(target.configs) do
					_p(4,'%s /* %s */,', cfg.xcode.targetid, xcode.getconfigname(cfg))
				end
				_p(3,');')
				_p(3,'defaultConfigurationIsVisible = 0;')
				_p(3,'defaultConfigurationName = %s;', defaultCfgName)
				_p(2,'};')
			end
		end
		settings['1DEB928908733DD80010E9CD'] = function()
			_p(2,'1DEB928908733DD80010E9CD /* Build configuration list for PBXProject "%s" */ = {', tr.name)
			_p(3,'isa = XCConfigurationList;')
			_p(3,'buildConfigurations = (')
			for _, cfg in ipairs(tr.configs) do
				_p(4,'%s /* %s */,', cfg.xcode.projectid, xcode.getconfigname(cfg))
			end
			_p(3,');')
			_p(3,'defaultConfigurationIsVisible = 0;')
			_p(3,'defaultConfigurationName = %s;', defaultCfgName)
			_p(2,'};')
		end
		
		_p('/* Begin XCConfigurationList section */')
		printSettingsTable(2, settings)
		_p('/* End XCConfigurationList section */')
	end


	function xcode.Footer()
		_p(1,'};')
		_p('\trootObject = 08FB7793FE84155DC02AAC07 /* Project object */;')
		_p('}')
	end
