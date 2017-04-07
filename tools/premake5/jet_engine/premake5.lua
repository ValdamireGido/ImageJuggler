-------------------------------------------------------------------------------
-- jet_engine specific stuff
-------------------------------------------------------------------------------

include "../common/"

releaseCheckedDefines = { "JTL_FORCE_DEBUG" }

-- release checked configuration
configuration "ReleaseChecked"
    defines { releaseCheckedDefines }

function addJetEngineConfig()
    
    addCommonConfig
    {
        staticRuntime = false,
        configurations = { "Debug", "Release", "ReleaseChecked" }
    }

    filter { "ReleaseChecked", "platforms:x32" }
        targetsuffix "_x86_rch"

    filter { "ReleaseChecked", "platforms:x64" }      
        targetsuffix "_x64_rch"

 	filter { "Debug"}
 		removeflags {"NoRTTI"} 
 		
    filter {}

    if (IsVisualStudio()) then 
        -- disable -'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed.Specify / EHsc
        buildoptions { "/wd4577 /we4390" }
    end
    	
	if IsXCode() then

    	filter { "Release*"}      
			xcodebuildsettings
			{
				["ENABLE_BITCODE"] = "YES",				
			}
		filter {}	
	end
end
