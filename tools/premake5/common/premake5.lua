------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

newoption 
{
	trigger = "to",
	value   = "path",
	description = "Set the output location for the generated files"
}

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
  
newoption 
{
	trigger = "arch",
	value   = "String",
	description = "type of platform"
}

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------


newoption 
{
   trigger     = "abi",
   description = "abi to compile for"
}
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

newaction 
{
		-- Metadata for the command line and help system

		trigger     = "vs2015",
		shortname   = "Visual Studio 2015",
		description = "Generate Visual Studio 2015 project files",

		-- Visual Studio always uses Windows path and naming conventions

		os = "windows",

		-- The capabilities of this action

		valid_kinds     = { "ConsoleApp", "WindowedApp", "StaticLib", "SharedLib", "Makefile", "None" },
		valid_languages = { "C", "C++", "C#" },
		valid_tools     = {
			cc     = { "msc"   },
			dotnet = { "msnet" },
		},

		-- Solution and project generation logic

		onsolution = premake.vstudio.vs2005.generateSolution,
		onproject  = premake.vstudio.vs2010.generateProject,

		oncleansolution = premake.vstudio.cleanSolution,
		oncleanproject  = premake.vstudio.cleanProject,
		oncleantarget   = premake.vstudio.cleanTarget,

		-- This stuff is specific to the Visual Studio exporters

		vstudio = {
			solutionVersion = "12",
			versionName     = "14",
			targetFramework = "4.5",
			toolsVersion    = "14.0",
		}
}



---
-- Register a command-line action for Visual Studio 2017.
---

newaction
{
		trigger         = "vs2017",
		shortname       = "Visual Studio 2017",
		description     = "Generate Microsoft Visual Studio 2017 project files",
		-- Visual Studio always uses Windows path and naming conventions
		os              = "windows",

		-- The capabilities of this action

		valid_kinds     = { "ConsoleApp", "WindowedApp", "StaticLib", "SharedLib", "Makefile", "None" },
		valid_languages = { "C", "C++", "C#" },

		valid_tools     = {
			cc     = { "msc"   },
			dotnet = { "msnet" },
		},

		-- Solution and project generation logic

		onsolution = premake.vstudio.vs2005.generateSolution,
		onproject  = premake.vstudio.vs2010.generateProject,

		oncleansolution = premake.vstudio.cleanSolution,
		oncleanproject  = premake.vstudio.cleanProject,
		oncleantarget   = premake.vstudio.cleanTarget,

		-- This stuff is specific to the Visual Studio exporters
		vstudio = {
			solutionVersion = "12",
			versionName     = "15",
			targetFramework = "4.5.2",
			toolsVersion    = "15.0",
			windowsTargetPlatformVersion = "8.1",
			supports64bitEditContinue = true,
		}
}  


premake.APPEX       = "AppExtension"
premake.FRAMEWORK   = "Framework"
premake.OBJC        = "Objective-C"
premake.SWIFT       = "Swift"

-- add a new flag in order to support the v110_xp toolset to support running apps on windows xp
premake.api.addAllowed("flags", "XpCompat") 
premake.api.addAllowed("flags", "LinkerWarningAsError")
premake.api.addAllowed("language", "ObjC")
premake.api.addAllowed("language", "ObjCpp")

premake.api.addAllowed("kind", "AppExtension")
premake.api.addAllowed("kind", "Framework")


premake.api.register 
{
		name = "gmakebuildsettings",
		scope = "config",
		kind = "key-array",
}
	
premake.api.register 
{
		name = "gmakeprojectfilename",
		scope = "project",
		kind = "string",
}

-- //
-- 'generatexcconfigs' makes .xcconfig files usable
premake.api.register 
{
		name = "generatexcconfigs",
		scope = "config",
		kind = "string",
		allowed = { "NO", "YES" },
		default = "NO",
}

-- list of the default .xcconfig files
premake.api.register 
{
		name = "generatedconfigurationfiles",
		scope = "config",
		kind = "list:file",
		tokens = true,
}
-- //

premake.api.register 
{
		name = "configurationfiles",
		scope = "config",
		kind = "list:file",
		tokens = true,
}

-- backport inlining from premake-core
premake.api.register 
{
		name = "inlining",
		scope = "config",
		kind = "string",
		allowed = {
			"Default",
			"Disabled",
			"Explicit",
			"Auto"
		}
}

premake.api.register 
{
		name = "basicruntimechecks",
		scope = "config",
		kind = "string",
		allowed = {
			"StackFrameRuntimeCheck",
			"UninitializedLocalUsageCheck",
			"EnableFastChecks",
			"Default"
		}
}

premake.api.unregister "debugformat"
premake.api.register
{
		name = "debugformat",
		scope = "config",
		kind = "string",
		allowed = {
			"c7",
			"None",
			"OldStyle",
			"ProgramDatabase",
			"EditAndContinue",
		},
}

-- Zm factor (0 means disabled. By default this value is 400 to avoid broke old projects)
-- DEFAULT VALUE is deprecated. By default should be 0.
premake.api.register
{
	name = "precompiledMemoryFactor",
	scope = "config",
	kind = "integer"
}

local PrecompiledMemoryFactorDefault = 0

-- Select the use of x64 compiler and tools
-- By default true.
premake.api.register
{
	name = "UseNativeToolchain",
	scope = "config",
	kind = "boolean"
}

-- projectDependency is used to mark as dependency projects that cannot be added 
-- as 'reference' using link{"prjName"} or dependson{"prjName"}.
-- Using this (Ex: projectDependency{"prjName"}) will change 
-- only the sln file at node 'ProjectSection(ProjectDependencies) = postProject'
premake.api.register 
{
		name = "projectDependency",
		scope = "config",
		kind = "list:string",
		tokens = true,
}

---
--- This will be used in Windows Store Projects (8.0, 8.1, 10.0 Universal)
--- Usage:
--- sdkReferences { ["WindowsMobile"] = {"10.0.10586.0", "10.0.10286.0"}
---					["SQLite"] = "3.0.2",
---	}
---
premake.api.register
{
	name = "sdkReferences",
	scope = "config",
	kind = "list:keyed:list"
}


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

premake.extensions.gl = {}

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customProjectDependencies(base, prj)

	local function getdependenciesEx(prj)
		local result = prj.dependencies['all']
		if result == nil then
			result = {}
		end
		
		local function add_to_project_list(cfg, depproj, result)
			local dep = premake.solution.findproject(cfg.solution, depproj)
			if dep ~= nil and not table.contains(result, dep) then
				table.insert(result, dep)
			end
		end

		for cfg in premake.project.eachconfig(prj) do
			for _, bd in ipairs(cfg.projectDependency) do
				if bd ~= prj.name then
					add_to_project_list(cfg, bd, result)
				end
			end
		end
		prj.dependencies['all'] = result
		return result
	end


	local deps = premake.project.getdependencies(prj, 'all')
	local depsEx = getdependenciesEx(prj)
	if (#deps > 0) or (depsEx ~= nil and #depsEx > 0) then		
		premake.push('	ProjectSection(ProjectDependencies) = postProject')
		if #deps > 0 then		
			for _, dep in ipairs(deps) do
				premake.w('{%s} = {%s}', dep.uuid, dep.uuid)
			end
		end
		if depsEx ~= nil and #depsEx > 0 then				
			for _, dep in ipairs(depsEx) do
				premake.w('{%s} = {%s}', dep.uuid, dep.uuid)
			end
		end	
		premake.pop('	EndProjectSection')
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customPlatformToolset(base, cfg)
	local p = cfg.flags.XpCompat and "v110_xp" or "v110"
	if _ACTION > "vs2015" then
		p = cfg.flags.XpCompat and "v141_xp" or "v141"
    elseif _ACTION > "vs2013" then
		p = cfg.flags.XpCompat and "v140_xp" or "v140"
    elseif (_ACTION >= "vs2012" and IsWinPhone80App()) then
		p = "v110_wp80"
	elseif (_ACTION > "vs2012" and IsWinPhone81App()) then
		p = "v120_wp81"
	elseif _ACTION > "vs2012" then
		p = cfg.flags.XpCompat and "v120_xp" or "v120"
	end	
	if _ACTION > "vs2010" then
		_p(2,'<PlatformToolset>%s</PlatformToolset>', p)
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function generateConfigurationElements200x(configName, cfg, toolset)
	local name = "VCCLCompilerTool"
	local m = premake.vstudio.vc200x
	_p('<?xml version="1.0"?>')
	_p('<VisualStudioPropertySheet ')
	_p(1, 'ProjectType="Visual C++" ')
	_p(1, 'Version="8.00"')
	_p(1, 'Name="%s"', configName)
	_p(1, '>')
	_p(1, '<Tool')
	_p(2, 'Name="VCCLCompilerTool"')
	
	local callFunc = m.elements[name]
	
	if cfg and not cfg.fake then
		premake.callArray(callFunc, cfg, toolset)
	end
	
	_p(1, '/>')
	_p('</VisualStudioPropertySheet>')
end

function premake.extensions.gl.customCompilerTool(base, cfg, toolset)
	
	local name = "VCCLCompilerTool"
	premake.push('<Tool')
	local m = premake.vstudio.vc200x
	
	local nameFunc = m[name .. "Name"]
	local callFunc = m.elements[name]

	if nameFunc then
		name = nameFunc(cfg, toolset)
	end
	premake.w('Name="%s"', name)
   
	premake.pop('/>')
end 

function premake.extensions.gl.customResourceCompilerTool(base, cfg)

end 

function premake.extensions.gl.customGenerateProject200x(base, prj,toolset)

	local p = premake
	local m = p.vstudio.vc200x
	
	p.indent("\t")
	p.callArray(m.elements.project, prj)
	p.pop('</VisualStudioProject>')

	for cfg in premake.project.eachconfig(prj) do
		
		local ext = ".vsprops"
		local fname = prj.location or prj.basedir
		local configName = string.gsub(cfg.name, "|", "_")
		
		fname = path.join(fname, "props")
		fname = path.join(fname,  prj.name .. "-" .. configName)
		fname = fname .. ext
		
		local fn = path.getabsolute(fname)
		
		local f, err = io.open(fn, "wb")
		if (not f) then
			error(err, 0)
		end

		io.output(f)
		_indentLevel = 0
		generateConfigurationElements200x(configName, cfg, toolset)
		f:close()
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customDebugInformationFormat(base, cfg)
	local value
	if cfg.flags.Symbols then
		if cfg.debugformat == "c7" or cfg.debugformat == "OldStyle" then
			value = "OldStyle"
		elseif cfg.debugformat == "None" then
			value = "None"
		elseif cfg.debugformat == "ProgramDatabase" then
			value = "ProgramDatabase"
		elseif cfg.debugformat == "EditAndContinue" then
			value = "EditAndContinue"
		elseif cfg.architecture == "x64" or
			   cfg.clr ~= premake.OFF or
			   premake.config.isOptimizedBuild(cfg) or
			   not cfg.editAndContinue
		then
			value = "ProgramDatabase"
		else
			value = "EditAndContinue"
		end
	end
	if value then
		premake.w('<DebugInformationFormat>%s</DebugInformationFormat>', value)
	end
end


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customBasicRuntimeChecks(base, cfg)
	local m = premake.vstudio.vc2010
	local runtime = premake.config.getruntime(cfg)
	if cfg.basicruntimechecks then
		local types = {
			StackFrameRuntimeCheck = "StackFrameRuntimeCheck",
			UninitializedLocalUsageCheck = "UninitializedLocalUsageCheck",
			EnableFastChecks = "EnableFastChecks",
			Default = "Default"
		}
		m.element("BasicRuntimeChecks", nil, types[cfg.basicruntimechecks])
	elseif cfg.flags.NoRuntimeChecks or (premake.config.isOptimizedBuild(cfg) and runtime:endswith("Debug")) then
		m.element("BasicRuntimeChecks", nil, "Default")
	end
end


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customClCompileElements(base, cfg)
	local m = premake.vstudio.vc2010

	function m.inlineFunctionExpansion(cfg)
		if cfg.inlining then
			local types = {
				Default = "Default",
				Disabled = "Disabled",
				Explicit = "OnlyExplicitInline",
				Auto = "AnySuitable",
			}
			m.element("InlineFunctionExpansion", nil, types[cfg.inlining])
		end
	end

	local clcompileelements = base(cfg)
	if not cfg.fake then
		table.insert(clcompileelements, m.inlineFunctionExpansion)
	end
	return clcompileelements
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customClCompile(base, cfg)
	local p = premake
	local m = p.vstudio.vc2010
	local prj = cfg.project.project
	local ext = ".props"
	local fpath = prj.location or prj.basedir
	local configName = string.gsub(cfg.name, "|", "_")
	local fname
	fpath = path.join(fpath, "props")
	
	fname = path.join(fpath, prj.name .. "-" .. configName)
	fname = fname .. ext
			
	local temp = io.output()
	
	local f, err = io.open(fname, "wb")
	if (not f) then
		error(err, 0)
	end
		
	io.output(fname)

	-- Override Zm flag
	-- increase virtual memory range for pch
	if IsVisualStudio(_ACTION) then
		if cfg.precompiledMemoryFactor == nil then
			cfg.precompiledMemoryFactor = PrecompiledMemoryFactorDefault
		end
		if (cfg.precompiledMemoryFactor ~= nil) and (cfg.precompiledMemoryFactor > 0) then
			local memoryFactor = string.format("/Zm%d", cfg.precompiledMemoryFactor)
			table.insert(cfg.buildoptions, memoryFactor)
		end
	end
	
	_p('<?xml version="1.0" encoding="utf-8"?>')
	_p('<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">')
	_p(1, '<ImportGroup Label="PropertySheets" />')
	_p(1, '<PropertyGroup Label="UserMacros" />')
	_p(1, '<PropertyGroup />')
	_p(1, '<ItemDefinitionGroup>')
	p.push('<ClCompile>')
	p.callArray(m.elements.clCompile, cfg)

	p.pop('</ClCompile>')
	_p(1, '</ItemDefinitionGroup>')
	_p(1, '<ItemGroup />')
	_p('</Project>')
	
	io.output():close()
	io.output(temp)
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customProgramDataBaseFileName(base, cfg)
	if cfg.flags.Symbols and cfg.debugformat ~= "c7" then
		if cfg.kind == premake.CONSOLEAPP then
			local filename = cfg.buildtarget.basename
			_p(3,'<ProgramDataBaseFileName>$(IntDir)vc$(PlatformToolsetVersion).pdb</ProgramDataBaseFileName>')
		else
			local filename = cfg.buildtarget.basename
			_p(3,'<ProgramDataBaseFileName>$(OutDir)%s.pdb</ProgramDataBaseFileName>', filename)			
		end	
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customTreatLinkerWarningAsErrors(base, cfg)
	if cfg.flags.FatalWarnings and cfg.flags.LinkerWarningAsError then
		local el = iif(cfg.kind == premake.STATICLIB, "Lib", "Linker")
		_p(3,'<Treat%sWarningAsErrors>true</Treat%sWarningAsErrors>', el, el)
	end
end 

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customProjectfile(base, prj)
    local extension = ".vcxproj"
    if premake.project.isdotnet(prj) then
        extension = ".csproj"

	-- add support for windows 8.1 shared projects (with extension *.vcxitems)
	elseif ( prj.kind == "SharedProject") then
		extension = ".vcxitems"

    elseif premake.project.iscpp(prj) then
        extension = iif((_ACTION > "vs2008") or (_ACTION == "android_s2g"), ".vcxproj", ".vcproj")
    end
	
    return premake.filename(prj, extension)
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customMakeConfigMap(base, sln)
	
	for cfg in premake.solution.eachconfig(sln) do
		_p('ifeq ($(config),%s)', cfg.shortname)
			for prj in premake.solution.eachproject(sln) do
				local prjcfg = premake.project.getconfig(prj, cfg.buildcfg, cfg.platform)
				if prjcfg then
					_p('  %s_config = %s', premake.make.tovar(prj.name), prjcfg.shortname)

                    local extraSettings = ""
			        if prjcfg.gmakebuildsettings then
				         extraSettings = prjcfg.gmakebuildsettings[cfg.buildcfg] or ""		
                         if (type(extraSettings) == "table") then
                            extraSettings = table.implode(extraSettings, "", "")
                         end
				    end    			  

				    _p('  %s_extra = %s', premake.make.tovar(prj.name), extraSettings) 
				end
			end
		_p('endif')
	end
	_p('')
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customMakeCleanRules(base, sln)
	_p('clean:')
		for prj in premake.solution.eachproject(sln) do
			local prjpath = premake.filename(prj, prj.gmakeprojectfilename or '.make')
			local prjdir = path.getdirectory(path.getrelative(sln.location, prjpath))
			local prjname = path.getname(prjpath)
			_x(1,'@${MAKE} --no-print-directory -C %s -f %s clean', prjdir, prjname)
		end
	_p('')

end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customMakeProjectRules(base, sln)
	
	for prj in premake.solution.eachproject(sln) do
		local deps = premake.project.getdependencies(prj)
		deps = table.extract(deps, "name")
		_p('%s:%s', premake.esc(prj.name), premake.make.list(deps))

		local cfgvar = premake.make.tovar(prj.name)
		_p('ifneq (,$(%s_config))', cfgvar)

		_p(1,'@echo "==== Building %s ($(%s_config)) ===="', prj.name, cfgvar)

		local prjpath = premake.filename(prj, prj.gmakeprojectfilename or '.make')
		local prjdir = path.getdirectory(path.getrelative(sln.location, prjpath))
		local prjname = path.getname(prjpath)

		_x(1,'@${MAKE} --no-print-directory -C %s -f %s config=$(%s_config) $(%s_extra)', prjdir, prjname, cfgvar, cfgvar)

		_p('endif')
		_p('')
	end	
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customMakeLibs(base, cfg, toolset)
	local flags = toolset.getlinks(cfg)
	local libs = {};
	for _, j in pairs(flags) do
		local fileName = path.getbasename(j);
		local lib;
		if fileName:sub(1, 3)=='lib' then
			lib = '-l'..fileName:sub(4);
		else
			lib = fileName;
		end
		table.insert(libs, lib);
	end
	_p('  LIBS +=-Wl,--start-group %s -Wl,--end-group', premake.make.list(libs)); 
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customMakeLDeps(base, cfg, toolset)
	local deps = {};
	for i = 1, #cfg.links do
		local link = cfg.links[i];
		local prj = premake.solution.findproject(cfg.solution, link);
		if prj then
	            local prjcfg = premake.project.getconfig(prj, cfg.buildcfg, cfg.platform);
        	    local item = premake.project.getrelative(cfg.project, prjcfg.linktarget.fullpath);
	            local dep = path.getdirectory(item)..'/../../lib/gmake/'..path.getname(item);
	            table.insert(deps, dep);
		end
	end
	_p('  LDDEPS +=%s', premake.make.list(premake.esc(deps)))
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function premake.extensions.gl.sdkReferences(prj)
	local sdkReference = {}
	local hasSdks = false
	for cfg in premake.project.eachconfig(prj) do
		for _, sdks in ipairs(cfg.sdkReferences) do
			for sdk, versions in pairs(sdks) do
				if sdkReference[sdk] == nil then
					sdkReference[sdk] = {}
				end

				for _, version in ipairs(versions) do
					if table.indexof(sdkReference[sdk], version) == nil then
						sdkReference[sdk][#sdkReference[sdk] + 1] = version
						hasSdks = true
					end
				end
			end
		end
	end

	if hasSdks then
		premake.push("<ItemGroup>")
		for sdk, sdks in pairs(sdkReference) do
			for _, version in ipairs(sdks) do
				print (version)
				premake.w('<SDKReference Include="%s, Version=%s" />', sdk, version)
			end
		end
		premake.pop("</ItemGroup>")
	end

end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

premake.override(premake.vstudio.vc2010.elements, "project", function(base, prj)
	local elements = base(prj, prj)
	table.insert(elements, premake.extensions.gl.sdkReferences)
	return elements
end)

function premake.extensions.gl.customPropertySheets(base, cfg)
	local m = premake.vstudio.vc2010
	local p = premake

	p.push('<ImportGroup Label="PropertySheets" %s>', m.condition(cfg))
	p.w('<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform" />')
	
	local configName = string.gsub(cfg.name, "|", "_")
	local propsLocation = "props/"
	local ext = ".props"
	p.w('<Import Project="%s" />', propsLocation ..  cfg.project.project.name .. "-" .. configName .. ext)
	
	for _, cfgfile in ipairs(cfg.configurationfiles) do
		p.w('<Import Project="%s" />', premake.project.getrelative(cfg.project, cfgfile))
	end

	if (IsWinStore()) then
		local projectBaseString = "$([Microsoft.Build.Utilities.ToolLocationHelper]::GetPlatformExtensionSDKLocation(`{SDK}, Version={VERSION}`, $(TargetPlatformIdentifier), $(TargetPlatformVersion),  $(SDKReferenceDirectoryRoot), $(SDKExtensionDirectoryRoot), $(SDKReferenceRegistryRoot)))\\DesignTime\\CommonConfiguration\\Neutral\\{SDK}.props"
		local conditionBaseString = "exists('$([Microsoft.Build.Utilities.ToolLocationHelper]::GetPlatformExtensionSDKLocation(`{SDK}, Version={VERSION}`, $(TargetPlatformIdentifier), $(TargetPlatformVersion),  $(SDKReferenceDirectoryRoot), $(SDKExtensionDirectoryRoot), $(SDKReferenceRegistryRoot)))\\DesignTime\\CommonConfiguration\\Neutral\\{SDK}.props')"
		--for a, b in pairs(premake.field._kinds) do
		--	print(a)
		--end
		for _, sdks in ipairs(cfg.sdkReferences) do
			for sdk, versions in pairs(sdks) do
			    for _, version in ipairs(versions) do
			        local projectString = string.gsub(projectBaseString, "{SDK}", sdk)
			        local conditionString = string.gsub(conditionBaseString, "{SDK}", sdk)			        
			    	projectString = projectString:gsub("{VERSION}", version)
			    	conditionString = conditionString:gsub("{VERSION}", version)
			        p.w('<Import Project="%s" Condition="%s" />', projectString, conditionString)
			    end
			end
			--for a, b in pairs(sdk) do
			--	print ("Importing:" .. a, b)
			--end
			--for _, version in pairs(versions) do
			--	print ("Importing:" .. sdk .. " " .. version)
			
			--end
		end
	end

	p.pop('</ImportGroup>')
end

premake.override(premake.vstudio.vc2010, "additionalCompileOptions", function(base, cfg, condition)
	return base(cfg, condition)
end)

-------------------------------------------------------------------------------
-- Set default compiler toolchain
-------------------------------------------------------------------------------
function premake.extensions.gl.UseNativeToolchain(cfg)
	if cfg.UseNativeToolchain == nil or cfg.UseNativeToolchain then
		premake.w("<UseNativeEnvironment>true</UseNativeEnvironment>")
	end
end

premake.override(premake.vstudio.vc2010.elements, "configurationProperties", function (base, cfg)
	local elements = base(cfg)
	if (IsVisualStudio(_ACTION)) then
		table.insert(elements, premake.extensions.gl.UseNativeToolchain)
	end
	return elements
end)

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function premake.extensions.gl.customConfigurationElements200x(base, cfg)
	local m = premake.vstudio.vc200x
	
	function m.inheritedPropertySheets(cfg)
		if #cfg.configurationfiles == 0 then return end
		local vsprops = ""
		local configName = string.gsub(cfg.name, "|", "_")
		local propsLocation = "props/"
		local ext = ".vsprops"
	
		vsprops = vsprops .. propsLocation .. cfg.project.project.name .. "-" .. configName .. ext  .. ";"
	
		for _, cfgfile in ipairs(cfg.configurationfiles) do
			vsprops = vsprops .. premake.project.getrelative(cfg.project, cfgfile) .. ";"
		end
		premake.w('InheritedPropertySheets="%s"', vsprops)
	end

	local cfgelements = base(cfg)
	if not cfg.fake then
		table.insert(cfgelements, m.inheritedPropertySheets)
	end
	return cfgelements
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

premake.override(premake.vstudio.vc200x, 'generate',  premake.extensions.gl.customGenerateProject200x)
premake.override(premake.vstudio.vc200x, 'VCCLCompilerTool',  premake.extensions.gl.customCompilerTool)
premake.override(premake.vstudio.vc200x, 'VCResourceCompilerTool',  premake.extensions.gl.customResourceCompilerTool)

premake.override(premake.vstudio.vc2010, 'clCompile',  premake.extensions.gl.customClCompile)
premake.override(premake.vstudio.vc2010.elements, 'clCompile',  premake.extensions.gl.customClCompileElements)
premake.override(premake.vstudio.vc2010, 'debugInformationFormat',  premake.extensions.gl.customDebugInformationFormat)
premake.override(premake.vstudio.vc2010, 'basicRuntimeChecks',  premake.extensions.gl.customBasicRuntimeChecks)

--todo add flag for custom database ???
premake.override(premake.vstudio.vc2010, 'platformToolset',  premake.extensions.gl.customPlatformToolset)
premake.override(premake.vstudio.vc2010, 'programDataBaseFileName',  premake.extensions.gl.customProgramDataBaseFileName)
premake.override(premake.vstudio.vc2010, 'treatLinkerWarningAsErrors',  premake.extensions.gl.customTreatLinkerWarningAsErrors)    
premake.override(premake.vstudio, 'projectfile',  premake.extensions.gl.customProjectfile)

premake.override(premake.vstudio.vc2010, 'propertySheets',  premake.extensions.gl.customPropertySheets)
premake.override(premake.vstudio.vc200x.elements, 'configuration',  premake.extensions.gl.customConfigurationElements200x)

premake.override(premake.make, "configmap", premake.extensions.gl.customMakeConfigMap)
premake.override(premake.make, "cleanrules", premake.extensions.gl.customMakeCleanRules)
premake.override(premake.make, "projectrules", premake.extensions.gl.customMakeProjectRules)
premake.override(premake.make, "libs", premake.extensions.gl.customMakeLibs)
premake.override(premake.make, "ldDeps", premake.extensions.gl.customMakeLDeps)

premake.override(premake.vstudio.sln2005, "projectdependencies",  premake.extensions.gl.customProjectDependencies)
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsVisualStudio(action)
	local act = action or _ACTION
	return act == "vs2017" or act == "vs2015" or act == "vs2013" or act == "vs2012" or act == "vs2010" or act == "vs2008"
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsWindows()
	return os.is("windows")
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsLinux()
	return os.is("linux")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsXCode(action)
	local act = action or _ACTION
	return act == "xcode4"
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsAndroid(action)
	local act = action or _ACTION
	return act == "ndk-makefile" or act == "ndk-manifest"
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsAndroidTegra(action)
	local act = action or _ACTION
	return act == "android"
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsVSAndroid(action)
	return (action or _ACTION) == "vsandroid"
end


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function IsNDKMakefile()
	return _OPTIONS['arch'] == 'ndkmake'
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

-- begin: support for Android sln2gcc make tool
function IsAndroidS2G(action)
	local act = action or _ACTION
	return act == "android_s2g"
end
-- end: support for Android sln2gcc make tool

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsMacSystem()
	return os.is("macosx")
end

function IsMac()
	return _OPTIONS['arch'] == 'macosx'
end

function IsIos()
	return _OPTIONS['arch'] == 'ios'
end

function IsTvos()
	return _OPTIONS['arch'] == 'tvos' 
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsWin8StoreApp()
    if not _OPTIONS['arch'] then
       	return false
    else
    	return _OPTIONS['arch'] == 'w81app'
    end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsWinPhone81App()
    if not _OPTIONS['arch'] then
       	return false
    else
    	return _OPTIONS['arch'] == 'wp81app'
    end
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsWinPhone80App()
    if not _OPTIONS['arch'] then
       	return false
    else
    	return _OPTIONS['arch'] == 'wp80app'
    end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsWin10App()
    if not _OPTIONS['arch'] then
       	return false
    else
    	return _OPTIONS['arch'] == 'w10app'
    end
end

-------------------------------------------------------------------------------
-- Check if it is managed app
-------------------------------------------------------------------------------

function IsWinStore()
	return IsWin10App() or IsWinPhone80App() or IsWinPhone81App() or IsWin8StoreApp()
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsWin32()
  return (IsVisualStudio() and (not IsWin8StoreApp()) and (not IsWinPhone80App()) and (not IsWinPhone81App()) and (not IsWin10App()))
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsFastBuild()
	local act = action or _ACTION
	return act == "vs2015-fastbuild"  
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsSteamApp()
    if not _OPTIONS['arch'] then
       	return false
    else
    	return _OPTIONS['arch'] == 'steam'
    end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function GetPathFromPlatform(action)
    local act = action or _ACTION 
    if act == "ndk-makefile" then
        return "ndk-build/local/$(TARGET_ARCH_ABI)" --.. _OPTIONS['abi']
    -- begin: support for Android sln2gcc make tool
    elseif act == "android_s2g" then
        return act  
    -- end: support for Android sln2gcc make tool
    elseif act == "android" then
    	return act
    elseif act == "vsandroid" then
    	return act
	-- support for steam projects 
    elseif act == "vs2015" and _OPTIONS['arch'] == "steam" then
        return act .. "_steam"		
    -- begin: support for windows 8 and windows phone 8 
    elseif act == "vs2013" and _OPTIONS['arch'] == "w81app" then
        return act .. "_w81"
    elseif act == "vs2013" and _OPTIONS['arch'] == "wp80app" then
        return act .. "_wp80"
    elseif act == "vs2012" and _OPTIONS['arch'] == "wp80app" then
        return act .. "_wp80"
    elseif act == "vs2013" and _OPTIONS['arch'] == "wp81app" then
        return act .. "_wp81"
	elseif act == "vs2015" and _OPTIONS['arch'] == "w10app" then
        return act .. "_w10"
    -- end: support for windows 8 and windows phone 8      
    elseif _ACTION=="xcode4" then
		return act .. "/" .._OPTIONS['arch']
	elseif _ACTION=="gmake" and _OPTIONS['arch'] == "ndkmake" then
		return "ndkmake"
	else
		return act
	end    
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function GetGlGetInfoFromPlatform(action)
	local act = action or _ACTION
	local info = {
		platform = "UNK",
		architecture = "UNK",
		compiler = "UNK"
	}

	local getConfigArch = 'string.gsub(cfg.longname, "^(.+)%|(.+)$", "%2")'
	local windowsArch = '%{(' .. getConfigArch .. ' == "x32") and "X86" or (' .. getConfigArch .. ' == "x64") and "X86_64" or "UNK"}'

	if act == "ndk-makefile" then
		info.platform = "Android"
		info.architecture = "$(TARGET_ARCH_ABI)"
		info.compiler = "GCC"
	elseif act == "android_s2g" then
		info.platform = "Android"
		info.architecture = "ARM"
		--info.compiler = ""
	elseif act == "vs2012" then
		info.compiler = "VC110"
		if _OPTIONS['arch'] == "wp80app" then
			info.platform = "WindowsPhone"
			info.architecture = "ARM"
		else
			info.platform = "Windows"
			info.architecture = windowsArch
		end
	elseif act == "vs2013" then
		info.compiler = "VC120"
		info.architecture = windowsArch
		if _OPTIONS['arch'] == "w81app" then
			info.platform = "Windows"
		elseif _OPTIONS['arch'] == "wp80app" then
			info.platform = "WindowsPhone"
			info.architecture = "ARM"
		elseif _OPTIONS['arch'] == "wp81app" then
			info.platform = "WindowsPhone"
			info.architecture = "ARM"
		else
			info.platform = "Windows"
		end
	elseif act == "vs2015" then
		info.platform = "Windows"
		info.compiler = "VC130"
		if _OPTIONS['arch'] == "w10app" then
			info.architecture = "Universal"
		else
			info.architecture = windowsArch
		end
	elseif _ACTION=="android" then
		info.compiler = "CLANG"
		info.platform = "Tegra-Android"
		info.architecture = "ARM"
	elseif _ACTION=="vsandroid" then
		info.compiler = "CLANG"
		info.platform = "android"
		info.architecture = "ARM"
	elseif _ACTION=="xcode4" then
		info.compiler = "CLANG"
		if _OPTIONS['arch'] == "ios" then
			info.platform = "iOS"
			info.architecture = "ARM"
		elseif _OPTIONS['arch'] == "macosx" then
			info.platform = "MacOSX"
			info.architecture = "X86_64"
		end
	elseif _ACTION=="gmake" then
		info.compiler = "GCC"
		if _OPTIONS['arch'] == "ndkmake" then
			info.platform = "Android"
			info.architecture = "ARM"
		else
			info.platform = "Linux"
			info.architecture = "X86_64"
		end
	end

	return info
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

-- begin: support for VisualStudio projects that includes all architectures (Win32, x64 for now)

function GetLibOutPathForAction(action)   
    return GetPathFromPlatform(act) .."_%{cfg.shortname}"
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function linkBoost(list, suffix) 
	local sfx = suffix or ""
	for _, link in pairs(list) do
		links ( "boost_" .. link .. sfx)
	end	
end	
-- used to generate the correct path to the boost libs
function AddBoostLink(relative_folder_to_boost_libs , list)
   	configuration { "x32" }  	
   		libdirs
		{
			relative_folder_to_boost_libs .. GetPathFromPlatform() .. '/x86'
		}
		--linkBoost(list)
	configuration { "x64" }  	
		libdirs
		{
			relative_folder_to_boost_libs .. GetPathFromPlatform() .. '/x64'
		}
		--linkBoost(list)
   	--[[configuration { "vs*" }
		libdirs
		{
			relative_folder_to_boost_libs .. '/' .. _ACTION .. '/' .._OPTIONS['arch']
		}
		]]
	configuration "xcode4"
			libdirs
			{
				relative_folder_to_boost_libs .. '/' .. GetPathFromPlatform()
			}

	configuration {"Debug", "xcode4 or linux"}
			linkBoost(list, "_d")

	configuration {"Release", "xcode4 or linux"}
			linkBoost(list)

	configuration "android_s2g"
		libdirs
		{
			relative_folder_to_boost_libs .. '/android/arm/'
		}    

	configuration {"Debug", "android_s2g"}
		linkBoost(list, "-gcc-mt-sd-1_55")

	configuration {"Release", "android_s2g"}
		linkBoost(list, "-gcc-mt-s-1_55")

	configuration "ndk-makefile"
		libdirs
		{
			relative_folder_to_boost_libs .. 'android/$(TARGET_ARCH)/'
		}      
	configuration {"Debug", "ndk-makefile"}
		linkBoost(list, "-gcc-mt-sd-1_55")
	configuration {"Release", "ndk-makefile"}
		linkBoost(list, "-gcc-mt-s-1_55")

	configuration { "not vs*", "not xcode4", "not android_s2g", "not ndk-makefile"}
		if IsNDKMakefile() then
			libdirs
			{
				relative_folder_to_boost_libs .. '/' .. 'android/$(arch)',
			}
			filter { "Debug" }
				linkBoost(list, "-gcc-mt-sd-1_55")
			filter { "Release" }
				linkBoost(list, "-gcc-mt-s-1_55")
			filter {}
			links "gnustl_static"
		elseif IsAndroidTegra() then
			local arch = "arm" -- Could be arm, arm64, mips, mips64, x86, x86_64

			libdirs
			{
				relative_folder_to_boost_libs .. '/' .. 'android/' .. arch,
			}
			filter { "Debug" }
				linkBoost(list, "-gcc-mt-sd-1_55")
			filter { "Release" }
				linkBoost(list, "-gcc-mt-s-1_55")
			filter {}
		elseif IsVSAndroid() then
			libdirs
			{
				relative_folder_to_boost_libs .. '/' .. 'android/$(PlatformArchitecture)', -- This will be removed in future versions
				relative_folder_to_boost_libs .. '/' .. 'vsandroid/$(PlatformArchitecture)',
			}
			filter { "Debug" }
				linkBoost(list, "-gcc-mt-sd-1_55")
			filter { "Release" }
				linkBoost(list, "-gcc-mt-s-1_55")
			filter {}
		else 
			libdirs 
			{ 
				relative_folder_to_boost_libs .. '/' .. _ACTION .. '/%{cfg.architecture}'
			}
		end
	configuration {}
end
-- end: support for VisualStudio projects that includes all architectures (Win32, x64)

function AddBoostSupport(base_jet_path , list)
	local relative_folder_to_boost_libs =  base_jet_path .. '/libs/Boost/stage/lib/'
	includedirs
	{
		base_jet_path .. '/libs/Boost',
	}

	AddBoostLink(relative_folder_to_boost_libs, list)
end

function NeedsBoostLink()
	if IsIos() or IsMac() or IsAndroidS2G() then
		return true
	end
	return false
end


function AddQwertySupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder

	includedirs
	{
		rootPath .. '/Qwerty/include',
	}

	AddQwertyLink(rootPath)
end

function AddQwertyLink(rootPath)  
	rootPath = rootPath or '../..' -- default is lib folder
	
	libdirs 
	{ 
		rootPath .. '/Qwerty/lib/'  .. GetPathFromPlatform(),
	}

	if IsWindows() then
		filter { "Release*", "platforms:x32" }  	   
			links "Qwerty_x86"

  	 	filter { "Debug*", "platforms:x32" }
   			links "Qwerty_x86_d"

  		filter { "Release*","platforms:x64" }  	
			links "Qwerty_x64"   		

		filter { "Debug*","platforms:x64" }  	
			links "Qwerty_x64_d"  
		filter {}	 	
	end		
	if IsMac() or IsIos() or IsAndroidTegra() then
		filter { "Release*"}  	   
			links "Qwerty"

  	 	filter { "Debug*"}
   			links "Qwerty_d"
   		filter {}	
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddLibLink(path, name, systemlib)
	path = path or '../..' -- default is lib folder
	
	libdirs 
	{
		string.format("%s/%s/lib/%s", path, name, GetPathFromPlatform() or ""),
	}
	
	if IsWindows() then
	
		filter { "Release*", "platforms:x32" }  	   
			links { name .. "_x86" }

  	 	filter { "Debug*", "platforms:x32" }
   			links { name .. "_x86_d" }

  		filter { "Release*","platforms:x64" }  	
			links { name .. "_x64" }

		filter { "Debug*","platforms:x64" }  	
			links { name .. "_x64_d" }

		filter {}	
	end		

	if IsMac() or IsIos() or IsLinux() or IsAndroidTegra() then		
		if systemlib then
			links { systemlib }
		else
			filter { "Release*"}  	   
				links { name }

			filter { "Debug*"}
				links { name .. "_d" }

			filter {}
		end
	end
end

function AddLibIncludes(path, name)
	path = path or '../..' -- default is lib folder
	
	includedirs 
	{
		string.format("%s/%s/include", path, name),
	}
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

-- CV: the old AddZipLink should be called AddZlibLink!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-- for now the AddZipLink will add both zlib and zip libraries

function AddZlibLink(rootPath)  
	AddLibLink(rootPath, "Zlib", "z")		
end

function AddZipSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	includedirs
	{
		rootPath .. '/zip/src',
	}
	
	AddZipLink(rootPath)
end

function AddZipLink(rootPath)	
	AddLibLink(rootPath, "Zip")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddHarfbuzzLink(rootPath)
	AddLibLink(rootPath, "Harfbuzz")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddLibunibreakLink(rootPath)
	AddLibLink(rootPath, "Libunibreak")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddHarfbuzzSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	includedirs
	{
		rootPath .. '/Harfbuzz/include',
	}
	
	AddHarfbuzzLink(rootPath)
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddFreetypeLink(rootPath)
	AddLibLink(rootPath, "Freetype")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddFreetypeSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	includedirs
	{
		rootPath .. '/Freetype/include',
	}
	
	AddFreetypeLink(rootPath)
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddGlfwLink(rootPath)
	AddLibLink(rootPath, "Glfw")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddSfwLink(rootPath)
	AddLibLink(rootPath, "Sfw")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddQwertySupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder

	includedirs
	{
		rootPath .. '/Qwerty/include',
	}

	AddLibLink(rootPath, "Qwerty")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddCurlLink(rootPath)
	AddLibLink(rootPath, "Curl")		
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddPugixmlLink(rootPath)
	AddLibLink(rootPath, "Pugixml")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddPugixmlSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder

	includedirs
	{
		rootPath .. '/Pugixml/include',
	}

	AddPugixmlLink(rootPath)
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddLuaLink(rootPath)
	AddLibLink(rootPath, "Lua")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddLuaSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	includedirs
	{
		rootPath .. '/Lua/src',
	}
	
	AddLibLink(rootPath, "Lua")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddSfwSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	includedirs
	{
		rootPath .. '/Sfw/include',
	}
	
	AddLibLink(rootPath, "Sfw")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddRapidjsonSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder

	defines 
	{
		"RAPIDJSON_HAS_STDSTRING",
	}

	AddLibIncludes(rootPath, "Rapidjson")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddLz4Support(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder

	defines 
	{
	
	}

	AddLibIncludes(rootPath, "Lz4")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddGTestLink(rootPath)
	AddLibLink(rootPath, "GTest")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddGTestSupport(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	includedirs
	{
		rootPath .. '/GTest/include',
	}
	
	AddLibLink(rootPath, "GTest")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
function AddOpenSSLLink(rootPath)
	rootPath = rootPath or '../..' -- default is lib folder
	
	libdirs 
	{ 
		rootPath .. '/Openssl/lib/'  .. GetPathFromPlatform(),
	}

	
	if IsWindows() then
		filter { "Release", "platforms:x32" }  	   
			links "openssl_x86"
			links "crypto_x86"

  	 	filter { "Debug", "platforms:x32" }
   			links "openssl_x86_d"
			links "crypto_x86_d"

  		filter { "Release","platforms:x64" }  	
			links "openssl_x64"   		
			links "crypto_x64"

		filter { "Debug","platforms:x64" }  	
			links "openssl_x64_d"  
			links "crypto_x64_d"
		filter {}	 	
	end	
	if IsMac() or IsIos() or IsAndroidTegra() then
		filter { "Release"}  	   
			links "openssl"
			links "crypto"

  	 	filter { "Debug"}
   			links "openssl_d"
			links "crypto_d"
   		filter {}	
	end				
end

function AddLz4Link(rootPath)
	AddLibLink(rootPath, "Lz4")		
end
	
function AddCurlLink(rootPath)  
	rootPath = rootPath or '../..' -- default is lib folder
	
	libdirs 
	{ 
		rootPath .. '/Curl/lib/'  .. GetPathFromPlatform(),
	}

	if IsWindows() then
		filter { "Release*", "platforms:x32" }  	   
			links "curl_x86"

  	 	filter { "Debug*", "platforms:x32" }
   			links "curl_x86_d"

  		filter { "Release*","platforms:x64" }  	
			links "curl_x64"   		

		filter { "Debug*","platforms:x64" }  	
			links "curl_x64_d"  

		filter {}	 	
	end		
	if IsMac() or IsIos() or IsAndroidTegra() then
		filter { "Release*"}  	   
			links "curl"			

  	 	filter { "Debug*"}
   			links "curl_d"
			
   		filter {}	
	end		
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function AddAndroidNativeAppGlue()
	local_static_libraries { "android_native_app_glue" }
	includedirs
	{
		"$(NDK_ROOT)/sources/android/native_app_glue",
	}
	import_modules
	{
		"android/native_app_glue"
	}

	local useAndroidNativeAppGlue = true
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
-- begin: support for Android sln2gcc make tool
function  AddAndroidS2GDefaultConfig()  
    if not AndroidEntryPoint then 
        -- possible values: ACP_SDL_Activity, AF_Activity, Native_Activity, SDL_Activity
        AndroidEntryPoint = "AF_Activity"
    end
    if not AndroidToolBoxPath then 
        AndroidToolBoxPath = "../../AndroidToolBox/"
    end
    if not AndroidFramework then 
        AndroidFramework = "../../AndroidFramework/"
    end    
    if not AndroidCompiler then 
        AndroidCompiler = "Gcc4.8"
    end
    if not AndroidNativePlatform then 
        AndroidNativePlatform = "14"
    end    
    if not AndroidSDKPlatform then     
        AndroidSDKPlatform = "22"
    end
    if not AndroidCompilerJobs then     
        AndroidCompilerJobs = "4"
    end
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

if IsNDKMakefile() then
    include "../ndkmake/makefile.lua"
end
if _ACTION=="android_s2g" then
	AddAndroidS2GDefaultConfig()
include "../android_s2g/android_s2g.lua"
end	
-- end: support for Android sln2gcc make tool

if _ACTION=="android" then
include "../android/android.lua"
end	

if _ACTION == "vsandroid" then
	include "../vsandroid/vsandroid.lua"
end

if _ACTION=="xcode4" then
include "../xcode/xcode.lua"
end

if _ACTION=="ndk-makefile" or _ACTION=="ndk-manifest" then
include "../ndk/ndk.lua"
end

if IsWin8StoreApp() then
include "../w81app/w81app.lua"
end

if IsWinPhone81App() then 
include "../wp81app/wp81app.lua"
end

if IsWinPhone80App() then 
include "../wp80app/wp80app.lua"
end

if IsWin10App() then 
include "../w10app/w10app.lua"
end

if IsFastBuild() then
	include "../fastbuild/_fastbuild.lua"
end	
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

debugFlags = {"Symbols", "StaticRuntime"}
releaseFlags = {"Symbols", "StaticRuntime"}

commonDefines = { "_SCL_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS"}
 
debugDefines = {"_DEBUG"}
releaseDefines = {"NDEBUG", "_SECURE_SCL=0", "_SECURE_SCL_THROWS=0"}


-- for all release configurations including with exceptions
configuration "Release*"
	defines { releaseDefines, commonDefines }
	flags{ releaseFlags }
	optimize "Speed"

-- for all debug configurations including with exceptions
configuration "Debug*"
	defines { debugDefines, commonDefines }
	flags { debugFlags }	
	targetsuffix "_d"

if not IsLinux() then	
	configuration "Release"
		flags { "NoRTTI", "NoExceptions"}
		
	configuration "Debug"
		flags { "NoRTTI", "NoExceptions"}
end

configuration {}

-------------------------------------------------------------------------------
-- Solution level settings for all projects
-------------------------------------------------------------------------------

function addCommonConfig(options)

	language "C++"
	
	options = options or {}

	--if options.noexceptionsConfiguration == nil then options.noexceptionsConfiguration=true end
	--if options.exceptionsConfiguration == nil then options.exceptionsConfiguration=true end
    
	if not _OPTIONS["to"] then
		_OPTIONS["to"] = GetPathFromPlatform()
	end

	location ( _OPTIONS["to"] )
	
	--print("options.noexceptionsConfiguration="..tostring(options.noexceptionsConfiguration))
	--print("options.exceptionsConfiguration="..tostring(options.exceptionsConfiguration))
	
	--if options.exceptionsConfiguration == true and not options.noexceptionsConfiguration then
	--	print("Creating configurations with exceptions only")
	--	configurations { "Debug Exceptions" , "Release Exceptions" }
	--elseif not options.exceptionsConfiguration == true and options.noexceptionsConfiguration then
	--	print("Creating configurations without exceptions only")
	configurationsList = options.configurations or { "Debug", "Release" }
	configurations { configurationsList }
	--else
	--	print("Creating all configurations")
	--	configurations { "Debug", "Release" , "Debug Exceptions" , "Release Exceptions" }
	--end

	--ms removed fatal warnigs for now 
	-- {"FatalWarnings",
	flags {"NoMinimalRebuild" } 
	
	if IsNDKMakefile() then
		-- check if NDK_ROOT is set
		if not ( os.getenv("NDK_ROOT") ) then
			print "Environment variable NDK_ROOT is not defined. Aborting process."
			os.exit(ERROR_FAILURE)
		end
		
		-- need to specify linux system so it looks for .a files instead of .lib files.
		system "linux"

		-- STL support
		libdirs
		{
			'$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ndkver)/libs/$(stlarchsubdir)'
		}
		
        -- Use C++11, specify the API sysroot, and PIE compilation.
		buildoptions { "-std=c++11 --sysroot=$(sysrootpath) $(pieargs)" }
		linkoptions { "--sysroot=$(sysrootpath) $(pieargs)" }
		
		-- These defines are needed to workaround a known bug in libstdc++ for arm:
        -- see http://llvm.org/bugs/show_bug.cgi?id=12730
        defines {"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1", "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2", "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4", "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8"}
 		
		includedirs
		{
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ndkver)/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ndkver)/libs/$(stlarchsubdir)/include",
		}
			
	end
		
	if IsVisualStudio(_ACTION) or IsFastBuild() then
		flags { "MultiProcessorCompile" }
		if (IsWin10App()) then
			defines { "_WIN32_WINNT=0x0602"} -- 5.01 needed for Win10 File API
		else
			defines { "_WIN32_WINNT=0x0601"} -- 5.01 needed for boost and 5.02 needed for GetThreadId in JTL
		end
		--buildoptions { "/EHsc"} -- neeed for c++ exception handler used but unwind semantics not enabled !!!
		buildoptions { "/wd4530"} -- silence warning about unwind semantics not enabled !!!
		--linkoptions { "/ignore:4221" } -- ignore empty obj file

		buildoptions { "/we4715" } -- force error for not all control paths return a value
		buildoptions { "/we4716" }
		buildoptions { "/we4150" } -- force error for deletion of pointer to incomplete type 'type'; no destructor called
		PrecompiledMemoryFactorDefault = 400

        system "Windows"
        platforms { "x32", "x64"}

        if _ACTION == "vs2013" then
        	--http://blogs.msdn.com/b/vcblog/archive/2014/09/26/feedback-making-zc-inline-default-for-debug-release-configs-in-14.aspx
        	buildoptions {"/Zc:inline"}
        end
 	
  		filter { "Debug*"}  		
			inlining "Auto"
			debugformat "ProgramDatabase"
			basicruntimechecks "UninitializedLocalUsageCheck" 
		filter {}

		if options.staticRuntime == nil then options.staticRuntime = true end
		if not options.staticRuntime then
			useDynamicRuntimeLibs()
		end
	else
		-- Disable /Zm for another compilers!
		precompiledMemoryFactor(0)
	end

	if IsLinux() then		
		architecture "x64"

		linkoptions { "-pthread" }	
		linkoptions { "-rdynamic" }	
		
		local getcxxflags = premake.tools.gcc.getcxxflags;
		function premake.tools.gcc.getcxxflags(cfg)
		    local r = getcxxflags(cfg)
		    table.insert(r, "-std=c++11")
		    return r
		end
	end	

	if IsAndroid() then
		abis 
		{
			_OPTIONS['abi']
		}
		
		local_cflags 
		{
			"-std=c++11",
		}
		
		linkoptions 
		{
			"-landroid",
			"-lGLESv2",
			"-lEGL",
			"-llog"
		}
		
		defines 
		{
			"PLATFORM_ANDROID"
		}
		
		local _ndk_toolchain_version = "4.8"
		local _app_platform = "android-9"
		ndk_toolchain_version( _ndk_toolchain_version )
		app_platform ( _app_platform )
		
		includedirs
		{
			"$(NDK_ROOT)/sources",
			"$(NDK_ROOT)/platforms/".._app_platform.."/arch-$(TARGET_ARCH)/usr/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/" .. _ndk_toolchain_version .. "/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/" .. _ndk_toolchain_version .. "/libs/" .. _OPTIONS["abi"] .. "/include",
		}
			
		platforms { "android" }			
	end
	if IsAndroidTegra() then

		filter { "system:Android", "kind:not StaticLib" }
		links
		{
			"android",
			"GLESv2",
			"EGL",
			"log"
		}
		filter {}
		
		defines 
		{
			"PLATFORM_ANDROID",
			"OS_ANDROID",
			"ANDROID"
		}
		
		local _ndk_toolchain_version = "4.9"
		local _app_platform = "android-18"
		local _target_arch = "arm" -- Could be arm, arm64, x86, x86_64, mips, mips64
		local _target_abi = "armeabi-v7a" -- Could be armeabi, armeabi-v7a, armeabi-v7a-hard, arm64-v8a, mips, mips64, x86, x86_64

		androidapilevel(18)
		toolchainversion "Clang"
		cppstandard "c++11"
		stl "gnu"

		filter { "system:Android", "kind:WindowedApp" }
			-- Prevent strip of entry point
			linkoptions {"-Wl,-u,ANativeActivity_onCreate"}

		filter {}

		
		includedirs
		{
			"$(NDK_ROOT)/sources",
			"$(NDK_ROOT)/platforms/".._app_platform.."/arch-" .. _target_arch .. "/usr/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/" .. _ndk_toolchain_version .. "/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/" .. _ndk_toolchain_version .. "/libs/" .. _target_abi ..  "/include",
		}
		
		platforms { "Tegra-Android" }
	end
	if IsVSAndroid() then
		filter { "system:Android", "kind:not StaticLib" }
		links
		{
			"android",
			"GLESv2",
			"EGL",
			"log"
		}
		filter {}
		
		defines 
		{
			"PLATFORM_ANDROID",
			"OS_ANDROID",
			"ANDROID"
		}
		
		local _ndk_toolchain_version = "4.9"
		local _app_platform = "android-18"
		local _target_arch = "arm" -- Could be arm, arm64, x86, x86_64, mips, mips64
		local _target_abi = "armeabi-v7a" -- Could be armeabi, armeabi-v7a, armeabi-v7a-hard, arm64-v8a, mips, mips64, x86, x86_64

		androidapilevel(18)
		cppstandard "c++11"
		stl "gnu"
		toolset "clang"

		filter { "kind:SharedLib" }
			-- Prevent strip of entry point
			linkoptions {"-Wl,-u,ANativeActivity_onCreate"}

		filter {}

		system "vsandroid"
		platforms { "ARM", "ARM64" }

		filter {"architecture:ARM64"}
			androidapilevel(21)
			_app_platform = "android-21"
			_target_arch = "arm64"
			_target_abi = "arm64-v8a"
		filter {}

		includedirs
		{
			"$(NDK_ROOT)/sources",
			"$(NDK_ROOT)/platforms/".._app_platform.."/arch-" .. _target_arch .. "/usr/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/" .. _ndk_toolchain_version .. "/include",
			"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/" .. _ndk_toolchain_version .. "/libs/" .. _target_abi ..  "/include",
		}
	end
-- begin: support for Android sln2gcc make tool
    
    if IsAndroidS2G() then
		local_cflags 
		{
		}
		
		linkoptions 
		{
		}
		
		defines {"PLATFORM_ANDROID", "OS_ANDROID"}		
		
	elseif IsWin8StoreApp() then
		flags { 'Unicode' }
		removeflags { 'StaticRuntime' }
		editAndContinue "Off"
		removedefines { '_WIN32_WINNT=0x0601' }
		platforms "arm"
        defines {"OS_W8"}
		
	elseif IsWinPhone81App() then
		flags { 'Unicode' }
		removeflags { 'StaticRuntime' }
		editAndContinue "Off"
		removedefines { '_WIN32_WINNT=0x0601' }
		platforms "arm"
        defines {"OS_WP81"}
		
	elseif IsWinPhone80App() then
		flags { 'Unicode', 'StaticRuntime' }
		editAndContinue "Off" 
		removedefines { '_WIN32_WINNT=0x0601' }
		platforms "arm"
        defines {"OS_WP8"}
		
	elseif IsWin10App() then
		flags { 'Unicode' }
		removeflags { 'StaticRuntime' }
		editAndContinue "Off" 
		removedefines { '_WIN32_WINNT=0x0601' }
		platforms "arm"
        defines {"OS_W10"}
	
	end
	
-- end: support for Android sln2gcc make tool

	filter { "platforms:x32" }  	   
		targetsuffix "_x86"

   	filter { "Debug*", "platforms:x32" }
   		targetsuffix "_x86_d"

  	filter { "platforms:x64" }  	
		targetsuffix "_x64"   		

	filter { "Debug*","platforms:x64" }  	
		targetsuffix "_x64_d"   	

	filter {}		
end

-------------------------------------------------------------------------------
-- Force dynamic linking against the standard runtime libraries.
-------------------------------------------------------------------------------

function useDynamicRuntimeLibs()
	configuration "Release*"
    	removeflags { 'StaticRuntime' }

	configuration "Debug*"
	    removeflags { 'StaticRuntime' }

	configuration {}
end

-------------------------------------------------------------------------------
-- xcode specific settings
-------------------------------------------------------------------------------

function addCommonXcodeBuildSettings()

	if IsXCode() then
		if (_OPTIONS['arch'] == "ios") then
			
		xcodebuildsettings
		{
			-- Architectures
			["ARCHS"] = "$(ARCHS_STANDARD)";
			["VALID_ARCHS"] = "$(ARCHS_STANDARD)";	
			["SDKROOT"] = "iphoneos";
			["IPHONEOS_DEPLOYMENT_TARGET"] = "8.0";
			["TARGETED_DEVICE_FAMILY"]	= "1,2";
			-- always build all arch so we dont have problems with 64
			["ONLY_ACTIVE_ARCH"] = "NO";
		
			-- Build Options
			["GCC_VERSION"] = "com.apple.compilers.llvm.clang.1_0";
			
			-- Deployment
			-- this must be set to YES for all librarires and to NO for APPLICATIONS
			["SKIP_INSTALL"] = "YES";
			
			-- Linking
			["GCC_SYMBOLS_PRIVATE_EXTERN"] = "YES";
			
			-- Code Generation
			["GCC_DYNAMIC_NO_PIC"] = "NO";		
			
			-- Language
			["GCC_C_LANGUAGE_STANDARD"] = "c11";	
			
			-- Language C++
			["CLANG_CXX_LANGUAGE_STANDARD"] = "c++11";
			["CLANG_CXX_LIBRARY"]  = "libc++";
			['CLANG_WARN_BOOL_CONVERSION'] = 'YES';
			['CLANG_WARN_CONSTANT_CONVERSION'] = 'YES';
			['CLANG_WARN_EMPTY_BODY'] = 'YES';
			['CLANG_WARN_ENUM_CONVERSION'] = 'YES';
			['CLANG_WARN_INT_CONVERSION'] = 'YES';
			['CLANG_WARN_UNREACHABLE_CODE'] = 'YES';
			['CLANG_WARN__DUPLICATE_METHOD_MATCH'] = 'YES';
			
			-- Preprocessing
			['ENABLE_STRICT_OBJC_MSGSEND'] = 'YES';

			-- Warnings
			-- ms disable these warnings for now 	
			["GCC_WARN_CHECK_SWITCH_STATEMENTS"] = "NO";
			["GCC_WARN_UNUSED_VARIABLE"] = "NO";
			--ms this needs to be done in another way 
			--["OTHER_CFLAGS"] = {"-Wno-braced-scalar-init"};		
			['GCC_WARN_64_TO_32_BIT_CONVERSION'] = 'YES';
			['GCC_WARN_UNDECLARED_SELECTOR'] = 'YES';
			['GCC_WARN_UNINITIALIZED_AUTOS'] = 'YES';
			['GCC_WARN_UNUSED_FUNCTION'] = 'NO';
			['GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS'] = 'YES';

			['GCC_NO_COMMON_BLOCKS'] = 'YES';
			-- xcode 7 
			--['ENABLE_TESTABILITY'] = 'YES';
			['CLANG_WARN_INFINITE_RECURSION'] = 'YES';
			['CLANG_WARN_SUSPICIOUS_MOVE'] = 'YES';


		}
		elseif (_OPTIONS['arch'] == "tvos") then
			buildoptions {"-fembed-bitcode"}
			
			xcodebuildsettings
			{
				["GCC_VERSION"] = "com.apple.compilers.llvm.clang.1_0";
				["TVOS_DEPLOYMENT_TARGET"] = "9.0";
				["TARGETED_DEVICE_FAMILY"]	= "3";
				["GCC_SYMBOLS_PRIVATE_EXTERN"] = "YES";
				["GCC_DYNAMIC_NO_PIC"] = "NO";			
				["GCC_C_LANGUAGE_STANDARD"] = "c11";			
				["CLANG_CXX_LANGUAGE_STANDARD"] = "c++11";
				["CLANG_CXX_LIBRARY"]  = "libc++";
				["ARCHS"] = "$(ARCHS_STANDARD)";
				["VALID_ARCHS"] = "$(ARCHS_STANDARD)";	
				["SDKROOT"] = "appletvos";	
				-- this must be set to YES for all librarires and to NO for APPLICATIONS
				["SKIP_INSTALL"] = "YES";
				-- always build all arch so we dont have problems with 64
				["ONLY_ACTIVE_ARCH"] = "NO";

				-- ms disable these warnings for now 	
				["GCC_WARN_CHECK_SWITCH_STATEMENTS"] = "NO";
				["GCC_WARN_UNUSED_VARIABLE"] = "NO";

				-- ms build precompiled headers only for c++ !!!
				["GCC_PFE_FILE_C_DIALECTS"] = "c++"; 

				-- code recommnede settings 
				["CLANG_WARN_BOOL_CONVERSION"] = "YES";
				["CLANG_WARN_CONSTANT_CONVERSION"] = "YES";
				["CLANG_WARN_EMPTY_BODY"] = "YES";
				["CLANG_WARN_ENUM_CONVERSION"] = "YES";
				["CLANG_WARN_INT_CONVERSION"] = "YES";
				["CLANG_WARN_UNREACHABLE_CODE"] = "YES";
				["CLANG_WARN__DUPLICATE_METHOD_MATCH"] = "YES";
				["ENABLE_STRICT_OBJC_MSGSEND"] = "YES";
				["GCC_WARN_64_TO_32_BIT_CONVERSION"] = "YES";
				["GCC_WARN_UNDECLARED_SELECTOR"] = "YES";
				["GCC_WARN_UNINITIALIZED_AUTOS"] = "YES";
				["GCC_WARN_UNUSED_FUNCTION"] = "YES";
			}	
		elseif (_OPTIONS['arch'] == "macosx") then

		xcodebuildsettings
		{
			-- Architectures
			["SDKROOT"] = "macosx";	
		
			-- Build Options
			["GCC_VERSION"] = "com.apple.compilers.llvm.clang.1_0";
			
			-- Linking
			["GCC_SYMBOLS_PRIVATE_EXTERN"] = "YES";
			
			-- Code Generation
			["GCC_DYNAMIC_NO_PIC"] = "NO";

			-- Language
			["GCC_C_LANGUAGE_STANDARD"] = "c11";

			-- Language C++
			["CLANG_CXX_LANGUAGE_STANDARD"] = "c++11";
			["CLANG_CXX_LIBRARY"]  = "libc++";		
		}
		end
	end	
end	

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

local function addExternalImplementation(name, path, type)
	local projectPath = path .. '/prj/' .. GetPathFromPlatform();
	local projectFilePath = projectPath .. '/'..name..'.vcxproj';
	external(name);
	location(projectPath);
	kind(type);
	f = io.open(projectFilePath, 'r') or nil;
	if f ~= nil then
		s = f:read("*a");
		first, second, third, fourth, fifth = string.match(s, 
			'<ProjectGuid>{(%x+)-(%x+)-(%x+)-(%x+)-(%x+)}</ProjectGuid>');
		uuid(first..'-'..second..'-'..third..'-'..fourth..'-'..fifth);
		io.close(f);
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addExternal(name, libPath)
	addExternalImplementation(name, libPath .. '/' .. name, 'StaticLib');
	return 
	{
		includedirs = {
			libPath .. '/' .. name .. '/include',
		},
		links = {
			name,
		},
		libdirs = {
			libPath .. '/' .. name .. '/lib/' .. GetPathFromPlatform(),
		}
	};
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addGlGetStructuredExternal(name, libPath, version)

	local fullName = name
	if version then
		fullName = name .. '_' .. version
	end

	addExternalImplementation(name, libPath .. '/' .. fullName, 'StaticLib');

	return
	{
		includedirs = {
			libPath .. '/' .. fullName .. '/lib/include',
		},
		links = {
			name,
		},
		-- Below path needs to be checked/tested
		libdirs = {
			libPath .. '/' .. fullName .. '/lib/' .. GetPathFromPlatform()
		}
	};
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addGlGetExternal(name, libPath, version)
	addExternalImplementation(name, libPath .. '/' .. fullName .. '/lib', 'StaticLib');

	local info = GetGlGetInfoFromPlatform(_ACTION)
	local getConfigName = 'string.gsub(cfg.longname, "^(.+)%|(.+)$", "%1")'
	local flags = '%{(' .. getConfigName .. ' == "Debug") and "ds-er" or (' .. getConfigName .. ' == "Release") and "os-" or "UNK"}'
	local target_name = name .. "--" .. version .. "--" .. info.platform .. "-" .. info.architecture .. "-" .. info.compiler .. "-" .. flags

	return
	{
		includedirs = {
			libPath .. '/' .. name .. '/lib/include',
		},
		links = {
			target_name,
		},
		libdirs = {
			libPath .. '/' .. name .. '/lib-bin/' .. target_name,
		}
	};
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addLocalExternal(relPath, name, libPath)
	addExternalImplementation(name, relPath .. '/' .. name, 'StaticLib');
	return 
	{
		includedirs = {
			libPath .. '/' .. name .. '/include',
		},
		links = {
			name,
		},
		libdirs = {
			libPath .. '/' .. name .. '/lib/' .. GetPathFromPlatform(),
		}
	};
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addPremake(toolsPath)
	if not IsVisualStudio() then
		return;
	end
	if toolsPath == nil then
		toolsPath = "../../../bin/premake5";
	end
	project("Premake");
	files({
		"../prj/premake5.lua",
		toolsPath.."/**.lua",
		});
	vpaths({
			["."] = "**.lua",
			["*"] = toolsPath,
		});
	kind('None');
	uuid("BC227E6B-C19F-49E5-8DD2-D5EBB9E8B57B"); -- avoid duplicates.
end

function GenerateVsProps(file)
	if IsVisualStudio() or IsAndroidS2G() then
		local temp = io.output()
		local f =io.open(file, "w")
		if (not f) then
			error(err, 0)
		end
		io.output(file)
		_p('<?xml version="1.0" encoding="utf-8"?>')
		_p('<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">')
		_p(1,'<ImportGroup Label="PropertySheets" />')
		_p(1,'<PropertyGroup Label="UserMacros" />')
		_p(1,'<PropertyGroup />')
		_p(1,'<ItemDefinitionGroup />')
		_p(1,'<ItemGroup />')
		_p('</Project>')
		io.output():close()
		io.output(temp)	
	end
end

function GenerateProps(file)
	GenerateVsProps(file)
end

function GenerateMultipleProps(file)
	if IsVisualStudio() or IsAndroidS2G() then
		local configurationList={"-Debug_arm.props","-Release_arm.props","-Debug_x32.props","-Debug_x64.props","-Release_x32.props","-Release_x64.props"}
		if IsWin32() then
			table.remove(configurationList, 1);
			table.remove(configurationList, 1);
		end
		for k, v in pairs (configurationList) do
			fileProps=file..v
			GenerateVsProps(fileProps)
			
		end
	end
end

function ImportMultipleProps(name)
	if IsVisualStudio() or IsAndroidS2G() then
	configuration{"debug","x32"}
		configurationfiles( { 
			name.."-Debug_x32.props"
		} )
	configuration{"release","x32"}
		configurationfiles( { 
			name.."-Release_x32.props"
		} )
	configuration{"debug","x64"}
		configurationfiles( { 
			name.."-Debug_x64.props"
		} )
	configuration{"release","x64"}
		configurationfiles( { 
			name.."-Release_x64.props"
		} )
	configuration{"debug","arm"}
		configurationfiles( { 
			name.."-Debug_arm.props"
		} )
	configuration{"release","arm"}
		configurationfiles( { 
			name.."-Release_arm.props"
		} )
	end
end

function ImportProps(fileName)
	configurationfiles( { fileName } )
end
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
