include "../tools/premake5/common/premake5.lua"
include "../tools/premake5/xcode/xcode.lua"
include "../tools/premake5/cu/compilationunit.lua"

solution ("ImageJuggler_Test")
    startproject "Test"
    language "C++"
	location ( _OPTIONS["to"] or _ACTION)
    addCommonConfig( {
		noexceptionsConfiguration = true,
		exceptionsConfiguration = false
	})
	flags { 
        "Symbols", 
        "NoMinimalRebuild",  
        "No64BitChecks", 
        "StaticRuntime" , 
        "NoExceptions" , 
        "NoRTTI" , 
        "MultiProcessorCompile"
    }

    configurations { 
        "Debug", 
        "Release"
    }

project ("Test")
    kind "ConsoleApp"
    targetdir ("../lib")
    files {
        "../source/test/**.cpp",
        "../source/test/**.h"
    }

    includedirs {
        "../source/test", 
        "../source/ImageJuggler",
        "../source/boost"
    }

    links { 
        "../ImageJugglerLib",
        "ImageJuggler"
    }

    if IsXCode() then
        buildoptions { "-std=c++11 -stdlib=libc++ -x objective-c++ -Wno-error" }
        kind "WindowedApp"
        addCommonXcodeBuildSettings()
        xcodebuildsettings {
			["CODE_SIGN_IDENTITY"] = "iPhone Developer: Valeri Vuchov (WDTMDP2J2J)",
		}
		xcodebuildresources	{}
    end
    if IsIos() then 
        linkoptions {
            "-ObjC",
            "-fobjc-arc"
        }
        links {
            "SystemConfiguration.framework", 
            "libz.1.2.5.tbd"
        }
    end

project ("ImageJuggler")
    kind "StaticLib"
    targetdir ("../lib")
    files {
        "../source/ImageJuggler/**.cpp", 
        "../source/ImageJuggler/**.h"
    }

    includedirs {
        "../source/ImageJuggler", 
        "../source/boost"
    }

    if IsXCode() then
		buildoptions { "-std=c++11 -stdlib=libc++ -x objective-c++ -Wno-error" }
		xcodebuildsettings 
		{
			["COMPILER_FLAGS"] = "-fobjc-arc",
		}
  
		addCommonXcodeBuildSettings()
		xcodebuildsettings
		{
			["GCC_ENABLE_CPP_EXCEPTIONS"] = "NO",
		}

		configuration "Debug"
	end

    
