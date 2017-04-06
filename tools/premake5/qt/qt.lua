
--
-- define the qt extension
--
premake.extensions.qt = {
	--
	-- The available modules' definitions.
	-- Use the keys with qtmodule. For instance `qtmodule { "core", "gui" }` to use QtCore and QtGui
	--
	modules = {
		core = {
			name = "Core",
			include = "QtCore",
			defines = { "QT_CORE_LIB" }
		},
		gui = {
			name = "Gui",
			include = "QtGui",
			links = { "imm32", "winmm", "ws2_32" },
			defines = { "QT_GUI_LIB" }
		},
		network = {
			name = "Network",
			include = "QtNetwork",
			defines = { "QT_NETWORK_LIB" }
		},
		opengl = {
			name = "OpenGL",
			include = "QtOpenGL",
			links = { "OpenGL32" },
			defines = { "QT_OPENGL_LIB" }
		},
		qml = {
			name = "Qml",
			include = "QtQml",
			defines = { "QT_QML_LIB" }
		},
		quick = {
			name = "Quick",
			include = "QtQuick",
			qtlinks = { 
				"qmldbg_tcp", 
				"qmldbg_tcp_qtdeclarative",
				"qtquick2plugin"
			}
		},
		widgets = {
			name = "Widgets",
			include = "QtWidgets",
			defines = { "QT_WIDGET_LIB" }
		},
		webkit = {
			name = "Webkit",
			include = "QtWebkit"
		},

		multimediawidgets = {
			name = "Multimediawidgets",
			include = "QtMultimediaWidgets"
		},

		multimedia = {
			name = "Multimedia",
			include = "QtMultimedia"
		},

		printsupport = {
			name = "Printsupport",
			include = "QtPrintsupport"
		},
		webkitwidgets = {
			name = "Webkitwidgets",
			include = "QtWebkitwidgets"
		},
		sensors = {
			name = "Sensors",
			include = "QtSensors"
		},
		sql = {
			name = "Sql",
			include = "QtSql"
		},
		svg = {
			name = "Svg",
			include = "QtSvg"
		},
		winextras = {
			name = "WinExtras",
			include = "QtWinExtras",
		},
		test = {
			name = "Test",
			include = "QtTest",
		},
	},
	
	--
	-- these are private, do not touch
	--
	enabled = false,
	defaultPath = os.getenv("QTDIR"),
	binPath = os.getenv("QTDIR") or '' .. '/bin',
	libPath = os.getenv("QTDIR") or '' .. '/lib',
	includePath = os.getenv("QTDIR") or '' .. '/include',
	dynamic = false,
}



--
-- register our custom API
--

--
-- Set the path where Qt is installed
--
premake.api.register {
	name = "qtpath",
	scope = "config",
	kind = "path"
}

--
-- Set the path where Qt is installed (bin)
--
premake.api.register {
	name = "qtbinpath",
	scope = "config",
	kind = "path"
}

--
-- Set the path where Qt is installed (libs)
--
premake.api.register {
	name = "qtlibpath",
	scope = "config",
	kind = "path"
}

--
-- Set the path where Qt is installed (includes)
--
premake.api.register {
	name = "qtincludepath",
	scope = "config",
	kind = "path"
}

--
-- Set the prefix of the libraries ("Qt4" or "Qt5" usually)
--
premake.api.register {
	name = "qtprefix",
	scope = "config",
	kind = "string"
}

--
-- Set a suffix for the libraries ("d" usually when using Debug Qt libs)
--
premake.api.register {
	name = "qtsuffix",
	scope = "config",
	kind = "string"
}

--
-- Specify the modules to use (will handle include paths, links, etc.)
-- See premake.extensions.qt.modules for a list of available modules.
--
premake.api.register {
	name = "qtmodules",
	scope = "config",
	kind = "string-list"
}

--
-- Specify the path, relative to the current script, where the files generated
-- by Qt will be created. If this command is not used, the default behavior
-- is to generate those files in the objdir.
--
premake.api.register {
	name = "qtgeneratedDir",
	scope = "config",
	kind = "path"
}

--
-- Private use only : used by the addon to know if qt has already been enabled or not
--
premake.api.register {
	name = "qtenabled",
	scope = "project",
	kind = "string"
}

premake.api.register {
	name = "qtLocalSources",
	scope = "project",
	kind = "path"
}

premake.api.register {
	name = "qtDynamic",
	scope = "project",
	kind = "string"
}
--
-- Enable Qt for a project. This must be called before calling any of the qt* api
-- functions previously registered.
--
function premake.extensions.qt.enable()

 	local qt = premake.extensions.qt
 	
 	-- enable Qt for the current config
 	qtenabled "true"

	-- setup our overrides if not already done
	if qt.enabled == false then
		qt.enabled = true
		premake.override(premake.oven, "bakeFiles", qt.customBakeFiles)
		premake.override(premake.oven, "bakeConfig", qt.customBakeConfig)
		premake.override(premake.fileconfig, "addconfig",  qt.customAddFileConfig)
	end

end

--
-- A small function which will get the generated directory for a given config.
-- If objdir was specified, it will be used. Else, it's the project's location +
-- obj + configuration + platform
--
-- @param
--		cfg The input configuration
--
function premake.extensions.qt.getGeneratedDir(cfg)

	-- check if the user specified a qtgenerateddir
	if cfg.qtgeneratedDir == nil then
		error("You forgot to specify the qtgeneratedDir folder in configuration "..cfg.name)
	end
	
	local relPath = path.getrelative(cfg.project.location, cfg.qtgeneratedDir);
	--print(cfg.qtgeneratedDir, relPath)

	if relPath ~= nil then
		return relPath
	end


	-- try the objdir, if it's already baked
	if cfg.objdir ~= nil then
		return cfg.objdir
	end

	-- last resort, revert to the default obj path used by premake.
	-- note : this is a bit hacky, but there is no easy "getobjdir(cfg)" method in
	-- premake, thus this piece of code
	dir = path.join(cfg.project.location, "obj")
	if cfg.platform then
		dir = path.join(dir, cfg.platform)
	end

	dir = path.join(dir, cfg.buildcfg)
	return path.getabsolute(dir)

end

--
-- Override the project.bakeFile method to be able to add the Qt generated files
-- to the project.
--
-- @param
--		base The original getFiles method.
-- @param
--		prj The current project.
-- @return
--		The table of files.
--
function premake.extensions.qt.customBakeConfig(base, prj, buildcfg, platform)

	local qt = premake.extensions.qt
	local modules = qt.modules

	-- bake
	local config = base(prj, buildcfg, platform)

	-- check if we need to add Qt support
	if config.qtenabled == "true" then

		-- get the installation path
		local qtpath = config.qtpath or qt.defaultPath
		local qtbinpath = path.getrelative(prj.location, buildcfg.qtbinpath or qt.binPath)
		local qtlibpath = config.qtlibpath or qt.libPath
		local qtincludepath = config.qtincludepath or qt.includePath
		local suffix	= config.qtsuffix or ""

		if qtpath == nil then
			error("Qt path is not set. Either use 'qtpath' in your project configuration or set the QTDIR environment variable.")
		end

		-- add the includes and libraries directories
		table.insert(config.includedirs, qtincludepath)
		table.insert(config.libdirs, qtlibpath)
		--ms do not insert qwindows for dynamic linking  
		if config.qtDynamic then

		else 
			table.insert(config.links, 'qwindows' .. suffix .. '.lib')
		end	
		-- add the modules
		for _, modulename in ipairs(config.qtmodules) do

			if modules[modulename] ~= nil then

				local module	= modules[modulename]
				local prefix	= config.qtprefix or ""
				local libname	= prefix .. module.name .. suffix .. ".lib"
				local dllname	= prefix .. module.name .. suffix .. ".dll"

				-- configure the module
				table.insert(config.includedirs, qtincludepath.. "/" .. module.include)
				table.insert(config.links, libname)
				if module.defines ~= nil then
					qt.mergeDefines(config, module.defines)
				end

				-- add additional links
				if module.links ~= nil then
					for _, additionallink in ipairs(module.links) do
						table.insert(config.links, additionallink)
					end
				end
				if module.qtlinks ~= nil then
					for _, qtlink in ipairs(module.qtlinks) do
						table.insert(config.links, qtlink..suffix..'.lib')
					end
				end
			end
		end

	end

	-- return the modified config
	return config

end

--
-- Override the project.bakeFile method to be able to add the Qt generated files
-- to the project.
--
-- @param
--		base The original getFiles method.
-- @param
--		prj The current project.
-- @return
--		The table of files.
--
function premake.extensions.qt.customBakeFiles(base, prj)

	local qt		= premake.extensions.qt
	local project	= premake.project

	-- parse the configurations for the project
	for cfg in project.eachconfig(prj) do

		-- ignore this config if Qt is not enabled
		if cfg.qtenabled == "true" then

			local mocs	    = {}
			local qrc	    = {}
			local objdir    = qt.getGeneratedDir(cfg)

			-- add the objdir as an include path
			table.insert(cfg.includedirs, objdir)

			-- check each file in this configuration
			table.foreachi(cfg.files, function(filename)

				if qt.isQRC(filename) then
					table.insert(qrc, filename)
				elseif qt.needMOC(filename) then
					table.insert(mocs, filename)
				end

			end)

			-- the moc files
			table.foreachi(mocs, function(filename)
				table.insert(cfg.files, objdir .. "/moc_" .. path.getbasename(filename) .. ".cpp")
			end)

			-- the qrc files
			table.foreachi(qrc, function(filename)
				table.insert(cfg.files, objdir .. "/qrc_" .. path.getbasename(filename) .. ".cpp")
			end)
		end
	end

	return base(prj)

end

--
-- Override the base premake.fileconfig.addconfig method in order to add our
-- custom build rules for special Qt files.
--
-- @param base
--		The base method that we must call.
-- @param fcfg
--		The file configuration object.
-- @param cfg
--		The current configuration that we're adding to the file configuration.
--
function premake.extensions.qt.customAddFileConfig(base, fcfg, cfg)

	-- call the base method to add the file config
	base(fcfg, cfg)

	-- do nothing else if Qt is not enabled
	if cfg.qtenabled ~= "true" then
		return
	end

	-- get the current config
	local config = premake.fileconfig.getconfig(fcfg, cfg)

	-- now filter the files, and depending on their type, add our
	-- custom build rules

	local qt = premake.extensions.qt

	-- ui files
	if qt.isUI(config.abspath) then
		qt.addUICustomBuildRule(config, cfg)

	-- resource files
	elseif qt.isQRC(config.abspath) then
		qt.addQRCCustomBuildRule(config, cfg)

	-- moc files
	elseif qt.needMOC(config.abspath) then
		qt.addMOCCustomBuildRule(config, cfg)

	end

end

--
-- Checks if a file is a ui file.
--
-- @param filename
--		The file name to check.
-- @return
--		true if the file needs to be run through the uic tool, false if not.
--
function premake.extensions.qt.isUI(filename)
	return path.hasextension(filename, { ".ui" })
end

--
-- Adds the custom build for ui files.
--
-- @param fcfg
--	  The config for a single file.
-- @param cfg
--    The config of the project ?
--
function premake.extensions.qt.addUICustomBuildRule(fcfg, cfg)

	local qt = premake.extensions.qt
	local qtbinpath = path.getrelative(fcfg.project.location, cfg.qtbinpath or qt.binPath)

	-- get the output file
	local output = qt.getGeneratedDir(cfg) .. "/ui_" .. fcfg.basename .. ".h"
	local tool = path.translate(qtbinpath .. "/uic.exe");
	-- add the custom build rule
	fcfg.buildmessage	= "Uic'ing " .. fcfg.name
	fcfg.buildcommands	= { tool .." -o \"" .. output .. "\" \"" .. fcfg.relpath .. "\"" }
	fcfg.buildoutputs	= { output }

end

--
-- Checks if a file is a qrc file.
--
-- @param filename
--		The file name to check.
-- @return
--		true if the file needs to be run through the rcc tool, false if not.
--
function premake.extensions.qt.isQRC(filename)
	return path.hasextension(filename, { ".qrc" })
end

--
-- Adds the custom build for ui files.
--
-- @param fcfg
--	  The config for a single file.
-- @param cfg
--    The config of the project ?
--
function premake.extensions.qt.addQRCCustomBuildRule(fcfg, cfg)
	local qt = premake.extensions.qt
	local qtbinpath = path.getrelative(fcfg.project.location, cfg.qtbinpath or qt.binPath)

	-- get the output file
	local output = qt.getGeneratedDir(cfg) .. "/qrc_" .. fcfg.basename .. ".cpp"
	local tool = path.translate(qtbinpath .. "/rcc.exe");

	-- get the files embedded on the qrc, to add them as input dependencies :
	-- if we edit a .qml embedded in the qrc, we want the qrc to re-build whenever
	-- we edit the qml file
	local inputs = qt.getQRCDependencies(fcfg)

	-- add the custom build rule
	fcfg.buildmessage	= "Rcc'ing " .. fcfg.name
	fcfg.buildcommands	= { tool .. " -name \"" .. fcfg.basename .. "\" -no-compress \"" .. fcfg.relpath .. "\" -o \"" .. output .. "\"" }
	fcfg.buildoutputs	= { output }
	if #inputs > 0 then
		fcfg.buildinputs = inputs
	end

end

--
-- Get the files referenced by a qrc file.
--
-- @param fcfg
--		The configuration of the file
-- @return
--		The list of project relative file names of the dependencies
--
function premake.extensions.qt.getQRCDependencies(fcfg)

	local dependencies = {}
	local file = io.open(fcfg.abspath)

	-- parse the qrc file to find the files it will embed
	for line in file:lines() do

		-- try to find the <file></file> entries
		local match = string.match(line, "<file>(.+)</file>")
		if match == nil then
			match = string.match(line, "<file%s+[^>]*>(.+)</file>")
		end

		-- if we have one, compute the path of the file, and add it to the dependencies
		-- note : the QRC file are relative to the folder containing the qrc file.
		if match ~= nil then
			local dependency = path.getdirectory(fcfg.abspath) .. "/" .. match
			table.insert(dependencies, path.getrelative(fcfg.project.location, dependency))
		end

	end

	-- close the qrc file
	io.close(file)

	return dependencies

end

--
-- Checks if a file needs moc'ing.
--
-- @param filename
--		The file name to check.
-- @return
--		true if the header needs to be run through the moc tool, false if not.
--
function premake.extensions.qt.needMOC(filename)

	local needmoc = false

	-- only handle headers
	if path.iscppheader(filename) then

		-- open the file
		local file = io.open(filename)

		-- scan it to find 'Q_OBJECT'
		for line in file:lines() do
			if line:find("Q_OBJECT") or line:find("Q_GADGET") then
				needmoc = true
				break
			end
		end

		io.close(file)
	end

	return needmoc
end

--
-- Adds the custom build for a moc'able file.
--
-- @param fcfg
--	  The config for a single file.
-- @param cfg
--    The config of the project ?
--
function premake.extensions.qt.addMOCCustomBuildRule(fcfg, cfg)

	local qt = premake.extensions.qt
	local qtbinpath = path.getrelative(fcfg.project.location, cfg.qtbinpath or qt.binPath)

	-- create the output file name
	local output = qt.getGeneratedDir(cfg) .. "/moc_" .. fcfg.basename .. ".cpp"
	local tool = path.translate(qtbinpath .. "/moc.exe");
	
	local relPath = path.getrelative(fcfg.project.location, fcfg.abspath);
	-- create the moc command
	local command = tool .." \"" .. relPath .. "\" -o \"" .. output .. "\""

	-- if we have a precompiled header, add it, and don't forget to add the original header:
	-- the moc command will override it if we specify a -f include

	local prjLocal = path.getrelative(cfg.qtLocalSources, fcfg.abspath);

	if fcfg.config.pchheader then
		command = command .. " \"-f" .. fcfg.config.pchheader .. "\" -f\"" .. prjLocal .. "\""
	end

	-- append the defines to the command
	if #fcfg.config.defines > 0 then
		table.foreachi(fcfg.config.defines, function (define)
			command = command .. " -D" .. define
		end)
	end

	-- append the include directories to the command
	if #fcfg.config.includedirs > 0 then
		table.foreachi(fcfg.config.includedirs, function (include)
			command = command .. " -I\"" .. path.getrelative(fcfg.project.location, include) .. "\""
		end)
	end

	-- add the custom build rule
	fcfg.buildmessage	= "Moc'ing " .. fcfg.name
	fcfg.buildcommands	= { command }
	fcfg.buildoutputs	= { output }

end

--
-- Merge defines into the configuration, taking care of not adding the
-- same define twice.
--
-- @param config
--		The configuration object.
-- @param defines
--		The defines to add.
--
function premake.extensions.qt.mergeDefines(config, defines)

	-- a function which checks if a value is contained in a table.
	local contains = function (t, v)
		for _, d in ipairs(t) do
			if d == v then
				return true
			end
		end
		return false
	end

	-- ensure defines is a table
	if type(defines) ~= "table" then
		defines = { defines }
	end

	-- add each defines
	for _, define in ipairs(defines) do
		if contains(config.defines, define) == false then
			table.insert(config.defines, define)
		end
	end

end
