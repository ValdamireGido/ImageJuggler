-------------------------------------------------------------------------------
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
			versionName     = "2014",
			targetFramework = "4.5",
			toolsVersion    = "14.0",
		}
	}


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

if _ACTION == "gmake" then
	newoption 
	{
	   trigger     = "jobs",
	   value       = "cnt",
	   description = "Tell gmake to use more than one core",
	}
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

-- add a new flag in order to support the v110_xp toolset to support running apps on windows xp
premake.api.addAllowed("flags", "XpCompat") 
premake.api.addAllowed("flags", "LinkerWarningAsError")
premake.api.addAllowed("flags", "LargeAdressAware")

premake.api.addAllowed("language", "ObjC")
premake.api.addAllowed("language", "ObjCpp")

premake.extensions.gl = {}

function premake.extensions.gl.customPlatformToolset(base, cfg)
	local p = cfg.flags.XpCompat and "v110_xp" or "v110"
	if _ACTION > "vs2013" then
		p = cfg.flags.XpCompat and "v140_xp" or "v140"
		
	elseif _ACTION > "vs2012" then
		p = cfg.flags.XpCompat and "v120_xp" or "v120"	
	end
	_p(2,'<PlatformToolset>%s</PlatformToolset>', p)
end

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


function premake.extensions.gl.customTreatLinkerWarningAsErrors(base, cfg)
	if cfg.flags.FatalWarnings and cfg.flags.LinkerWarningAsError then
		local el = iif(cfg.kind == premake.STATICLIB, "Lib", "Linker")
		_p(3,'<Treat%sWarningAsErrors>true</Treat%sWarningAsErrors>', el, el)
	end
end 


function premake.extensions.gl.customLink(base, cfg)
	local m = premake.vstudio.vc200x
	
	function m.largeAdressAware(cfg)	
		if cfg.flags.LargeAdressAware then
			_p(3, '<LargeAddressAware>true</LargeAddressAware>')
		end			
	end

	local cfgelements = base(cfg)
	if not cfg.fake then
		table.insert(cfgelements, m.largeAdressAware)
	end

	return cfgelements
end
--todo add flag for custom database ???

function premake.extensions.gl.customMakeLdDeps(base, cfg, toolset)
	-- ignore dependencies
	_p('  LDDEPS += ')
end

premake.override(premake.vstudio.vc2010.elements, 'link',  premake.extensions.gl.customLink)
premake.override(premake.vstudio.vc2010, 'platformToolset',  premake.extensions.gl.customPlatformToolset)
premake.override(premake.vstudio.vc2010, 'programDataBaseFileName',  premake.extensions.gl.customProgramDataBaseFileName)
premake.override(premake.vstudio.vc2010, 'treatLinkerWarningAsErrors',  premake.extensions.gl.customTreatLinkerWarningAsErrors)
premake.override(premake.make, "ldDeps", premake.extensions.gl.customMakeLdDeps)

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function IsVisualStudio(action)
	local act = action or _ACTION
	return  act == "vs2015" or act == "vs2013" or act == "vs2012" or act == "vs2010" or act == "vs2008"
end

function IsWindows()
	return os.is("windows")
end

function IsLinux()
	return os.is("linux")
end

function IsXCode(action)
	local act = action or _ACTION
	return act == "xcode4"
end

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

function GetPathFromPlatform(action)
	local act = action or _ACTION
	return _OPTIONS['arch'] and act .."/" .._OPTIONS['arch'] or act
	--return action or _ACTION
end

if _ACTION=="xcode4" then
include "../xcode/xcode.lua"
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

debugFlags = {"Symbols", "StaticRuntime", "NoRuntimeChecks", "NoBufferSecurityCheck", "NoManifest"}
releaseFlags = {"Symbols", "StaticRuntime"}

commonDefines = { "_SCL_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
 
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

configuration "Release"
	flags { "NoRTTI", "NoExceptions"}
	
configuration "Debug"
	flags { "NoRTTI", "NoExceptions"}
	
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addCommonXcodeBuildSettings()

	if (_OPTIONS['arch'] == "ios") then
	xcodebuildsettings
	{
		["GCC_VERSION"] = "com.apple.compilers.llvm.clang.1_0";
		["IPHONEOS_DEPLOYMENT_TARGET"] = "6.0";
		["TARGETED_DEVICE_FAMILY"]	= "1,2";
		["GCC_SYMBOLS_PRIVATE_EXTERN"] = "YES";
		["GCC_DYNAMIC_NO_PIC"] = "NO";			
		["GCC_C_LANGUAGE_STANDARD"] = "c11";			
		["CLANG_CXX_LANGUAGE_STANDARD"] = "c++11";
		["CLANG_CXX_LIBRARY"]  = "libc++";
		["ARCHS"] = "$(ARCHS_STANDARD)";
		["VALID_ARCHS"] = "$(ARCHS_STANDARD)";	
		["SDKROOT"] = "iphoneos";	
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
		["ARCHS"] = "arm64";
		["VALID_ARCHS"] = "arm64";	
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
		["GCC_VERSION"] = "com.apple.compilers.llvm.clang.1_0";
		["GCC_SYMBOLS_PRIVATE_EXTERN"] = "YES";
		["GCC_DYNAMIC_NO_PIC"] = "NO";			
		["GCC_C_LANGUAGE_STANDARD"] = "c11";			
		["CLANG_CXX_LANGUAGE_STANDARD"] = "c++11";
		["CLANG_CXX_LIBRARY"]  = "libc++";		
		["SDKROOT"] = "macosx";	
	}
	end
end	

function addCommonXcodeApplicationBuildSettings()
	addCommonXcodeBuildSettings()
	
	if IsXCode() then
		xcodebuildsettings
		{
			["CODE_SIGN_IDENTITY"] = "iPhone Developer",
			["DEVELOPMENT_TEAM"] = "A4QBZ46HAP";
			["PROVISIONING_PROFILE_SPECIFIER"] = "GAMELOFT315";
		}
	end
end

function addCommonConfig()

	language "C++"

	if  _OPTIONS["to"] then
			location ( _OPTIONS["to"] )
	else
		location (_ACTION)
	end
	
	configurations { "Debug", "Release" }

	flags {"NoMinimalRebuild" } 
		
	if IsVisualStudio(_ACTION) then
		platforms { "x32", "x64" }
		flags { "MultiProcessorCompile" }
		--defines { "_WIN32_WINNT=0x0501"} -- needed for boost 
		--buildoptions { "/EHsc"} -- neeed for c++ exception handler used but unwind semantics not enabled !!!
		buildoptions { "/wd4530"} -- silence warning about unwind semantics not enabled !!!
		--linkoptions { "/ignore:4221" } -- ignore empty obj file
	end 

	if _OPTIONS["arch"] == "x86" then
		architecture "x32"	
	end
	
	if (_OPTIONS['arch'] == "x64") then
		architecture "x64"
	end	

	if IsLinux() then
		buildoptions { "-std=c++11" }
	end

   	filter { "Debug*" }
   		targetsuffix "_d"

  	filter { "platforms:x64" }
		targetsuffix "_x64"

	filter { "Debug*","platforms:x64" }
		targetsuffix "_x64_d"

	filter {}
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addProjectPlatforms()
	if IsVisualStudio() then
		platforms { "x32", "x64" }

		filter { "platforms:x32" }
			architecture "x32"

		filter { "platforms:x64" }
			architecture "x64"

		filter {}
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addLibLink(path, dirname, libname, systemlib)
	local libname = libname or dirname

	libdirs { string.format("%s/%s/lib/%s", path, dirname, GetPathFromPlatform() or "") }

	if IsWindows() then
		filter { "Release*", "platforms:x32" }  	   
			links { libname }

  	 	filter { "Debug*", "platforms:x32" }
   			links { libname .. "_d" }

  		filter { "Release*","platforms:x64" }  	
			links { libname .. "_x64" }

		filter { "Debug*","platforms:x64" }  	
			links { libname .. "_x64_d" }

		filter {}	
	end		

	if IsMac() or IsIos() or IsTvos() then		
		if systemlib then
			links { systemlib }
		else
			filter { "Release*"}  	   
				links { libname }

			filter { "Debug*"}
				links { libname .. "_d" }

			filter {}
		end
	end

	if IsLinux() then
		if systemlib then
			links { systemlib }
		else
			filter { "release*" }
				links { libname }

			filter { "debug*" }
				links { libname .. "_d" }
				
			filter {}
		end
	end
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addUnitTestLink(rootPath)
	addLibLink(rootPath, "UnitTest++", "unit_test_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addTinyThreadLink(rootPath)
	addLibLink(rootPath, "tinythread", "tinythread_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addLuaLink(rootPath)
	addLibLink(rootPath, "lua", "lua_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addLogOgLink(rootPath)
	addLibLink(rootPath, "logog", "logog_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addProfileLibLink(rootPath)
	addLibLink(rootPath, "profile_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addTimerLibLink(rootPath)
	addLibLink(rootPath, "timer_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addUstlLink(rootPath)
	addLibLink(rootPath, "ustl", "ustl_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addPugiXmlLink(rootPath)
	addLibLink(rootPath, "pugixml10", "pugixml_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addFreeTypeLink(rootPath)
	addLibLink(rootPath, "freetype2", "freetype_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addZlibLink(rootPath)
	addLibLink(rootPath, "zlib", "zlib_lib", "z")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addLz4Link(rootPath)
	addLibLink(rootPath, "lz4", "lz4_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addGinLoaderLink(rootPath)
	addLibLink(rootPath, "GinLoader", "gin_loader_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addLocalizationLibLink(rootPath)
	addLibLink(rootPath, "localization_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addJsonCppLink(rootPath)
	addLibLink(rootPath, "json" , "json_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addVoxVehicleSoundsLink(rootPath)
	addLibLink(rootPath, "vox_vehicle_sounds" , "vox_vehicle_sounds_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addVoxReverbLink(rootPath)
	addLibLink(rootPath, "vox_reverb" , "vox_reverb_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addVoxGenericPluginLink(rootPath)
	addLibLink(rootPath, "vox_generic_plugin" , "vox_generic_plugin_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addBaseLibLink(rootPath)
	addLibLink(rootPath, "base", "base_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addNetLibLink(rootPath)
	addLibLink(rootPath, "net", "net_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addJetLink(rootPath)
	addLibLink(rootPath, "Jet", "jet_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addParticlesLink(rootPath)
	addLibLink(rootPath, "Particles", "particles_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addClaraLoaderLink(rootPath)
	addLibLink(rootPath, "ClaraLoader", "clara_loader_lib")
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function getGmakeJobs()
	local count = _OPTIONS["jobs"]
	if not count then
		return
	end

	return '--jobs=' .. count
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

function addInfoPlist()
	files { "xcode4/".. _OPTIONS["arch"] .."/Info.plist" }
end

-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------