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
end
