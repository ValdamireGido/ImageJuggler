    --
    -- Name:        qbs/qbs_project.lua
    -- Purpose:     Generate a qbs C/C++ project file.
    -- Author:      Lorenti Luciano    
    -- Created:     2015/05/17
    -- Copyright:   (c) 2008-2015 Jason Perkins and the Premake project
    --


            local p = premake
            local tree = p.tree
            local project = p.project
            local config = p.config
            local qbs = p.modules.qbs
            
            qbs.project = {}
            local m = qbs.project
            
            function path.isqtformfile(fname)
                local extensions = { ".ui" }
                local ext = path.getextension(fname):lower()
                return table.contains(extensions, ext)
            end
            
            function qbs.getLinks(cfg)
                -- System libraries are undecorated, add the required extension
                return config.getlinks(cfg, "system", "fullpath")
            end
            
            function qbs.getSiblingLinks(cfg)
                -- If we need sibling projects to be listed explicitly, add them on
                return config.getlinks(cfg, "siblings", "object")
            end
            
            
            m.elements = {}
            
            m.ctools = {
                gcc = "gnu gcc",
                clang = "clang",
                msc = "Visual C++",
            }
            m.cxxtools = {
                gcc = "gnu g++",
                clang = "clang++",
                msc = "Visual C++",
            }
            m.configs  = {
                Debug = "debug",
                Release= "release",
            }
            m.platforms = {
                Linux32 = "linux",
                Linux64 = "linux",
                Win32 = "windows",
                Win64 = "windows",
            }
            m.architecture = {
                Linux32 = "x86",
                Linux64 = "x86_64",
                Win32 = "x86",
                Win64 = "x86_64",
            }

            function m.getcompilername(cfg)
                local tool = _OPTIONS.cc or cfg.toolset or p.CLANG
                
                local toolset = p.tools[tool]
                if not toolset then
                    error("Invalid toolset '" + (_OPTIONS.cc or cfg.toolset) + "'")
                end
                
                if cfg.language == "C" then
                    return m.ctools[tool]
                elseif cfg.language == "C++" then
                    return m.cxxtools[tool]
                end
            end
            
            function m.getcompiler(cfg)
                local toolset = p.tools[_OPTIONS.cc or cfg.toolset or p.CLANG]
                if not toolset then
                    error("Invalid toolset '" + (_OPTIONS.cc or cfg.toolset) + "'")
                end
                return toolset
            end
            
            local function configuration_iscustombuild(cfg)
                
                return cfg and (cfg.kind == p.MAKEFILE) and (#cfg.buildcommands > 0)
            end
            
            local function configuration_isfilelist(cfg)
                
                return cfg and (cfg.buildaction == "None") and not configuration_iscustombuild(cfg)
            end
            
            local function configuration_needresoptions(cfg)
                
                return cfg and config.findfile(cfg, ".rc") and not configuration_iscustombuild(cfg)
            end
            
            
            
            function m.plugins(prj)
                
            end
            
            function m.description(prj)
                local type = m.internalTypeMap[prj.kind] or ""
                
                
            end
            function m.printFileList(name,fileList)
                formatName = name .. ': %s'
                if ( #fileList > 0) then
                    _p(formatName, "[" .. table.concat( fileList, ', \n' .. string.rep(' ',string.len(name) + 3)).."]") 
                end
            end
            function m.files(prj)
                local sources = {}
                local tr = project.getsourcetree(prj)
                tree.traverse(tr, {
                    -- folders are handled at the internal nodes
                    onbranchenter = function(node, depth)				
                end,
                onbranchexit = function(node, depth)				
            end,
            -- source files are handled at the leaves
            onleaf = function(node, depth)
            table.insert( sources, "\"".. node.relpath .."\"") 
                            end,
                    }, false, 1)
                    
                    m.printFileList('files', sources)
                    
                    
            end
            function m.splitString(inputstr, sep)
                if sep == nil then
                   sep = "%s"
                end
                local t={} ; i=1
                for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
                    t[i] = str
                    i = i + 1
                end
                return t
            end
            function m.dependencies(cfg)    
                prj = cfg.project                
                local dependencies = project.getdependencies(prj)
                cfgname = qbs.cfgname(cfg)
                if #dependencies > 0 then                              
                    for _, dependency in ipairs(dependencies) do                                        
                        depCFG = project.findClosestMatch(dependency,cfgname) 
                        _p("Depends {")
                            _p(" name: \"%s\"  ",dependency.name)
                            _p(" required: true ")
                            _p(" }")
                        end
                    end                                                   
                    
                end
                
                
                function m.global_compiler(prj)
                    
                end
                
                function m.global_linker(prj)
                    
                end
                
                function m.global_resourceCompiler(prj)
                    
                end
                
                m.elements.globalSettings = function(prj)
                return {
                    m.global_compiler,
                    m.global_linker,
                    m.global_resourceCompiler,
                }
            end
            
            function m.getQuotedList(l)
                ret ={}
                for _,v in ipairs(l) do
                    for _,v1 in ipairs(m.splitString(v)) do
                       table.insert(ret,"'"..v1.."'")
                    end
                end
                return ret;
            end
            function m.compiler(cfg)
                if configuration_iscustombuild(cfg) or configuration_isfilelist(cfg) then
                    
                    return
                end
                
                local toolset = m.getcompiler(cfg)
                local cxxflags = table.join(toolset.getcflags(cfg), toolset.getcxxflags(cfg), cfg.buildoptions)
                local cflags   =table.join(toolset.getcflags(cfg), cfg.buildoptions)
                local asmflags = ""
                local pch      = ""
                local defines ={}
                
                
                
                
                
                _p(2,"cpp.cxxFlags: [%s]",table.concat(m.getQuotedList(cxxflags),","))
                _p(2,"cpp.cFlags: [%s]",table.concat(m.getQuotedList(cflags),","))
                _p(2,"cpp.includePaths: [%s]",table.concat(m.getQuotedList(cfg.includedirs), ", "))
                _p(2,"cpp.platformDefines: [%s]",table.concat(m.getQuotedList(cfg.defines), ", "))
                
                
                
                
            end
            
            function m.linker(cfg)
                if configuration_iscustombuild(cfg) or configuration_isfilelist(cfg) then
                    return
                end
                
                local toolset = m.getcompiler(cfg)
                local flags = table.join(toolset.getldflags(cfg), cfg.linkoptions)                    
                local ldflags = m.getQuotedList(flags)
                
                local depsDir = {}
                local depsLib = {}
               
                if #cfg.libdirs > 0 then
                    local libdirs = project.getrelative(cfg.project, cfg.libdirs)
                    for _, libpath in ipairs(libdirs) do
                        table.insert(ldflags,"'-L" .. libpath .. "'")
                    end
                end
                
                local links = qbs.getLinks(cfg)
                for _, libname in ipairs(links) do                            
                    table.insert(ldflags,"'-l" .. libname .. "'")
                end
                _p(2,"cpp.linkerFlags: [%s]",table.concat(ldflags,","))
                
            end
            
            function m.resourceCompiler(cfg)
                if not configuration_needresoptions(cfg) then                
                    return
                end
                
                local toolset = m.getcompiler(cfg)
                local defines = table.implode(toolset.getdefines(table.join(cfg.defines, cfg.resdefines)), "", " ", "")
                local options = table.concat(cfg.resoptions, " ")

                
            end
            
            function m.general(cfg)
                if configuration_isfilelist(cfg) then
                    
                    return
                end
                
                local prj = cfg.project
                
                local isExe = prj.kind == "WindowedApp" or prj.kind == "ConsoleApp"
                local targetpath = path.getabsolute(project.getrelative(prj, cfg.buildtarget.directory))
                local objdir     = path.getabsolute(project.getrelative(prj, cfg.objdir))
                local targetname = path.getabsolute(project.getrelative(prj, cfg.buildtarget.abspath))
                local command    = iif(isExe, targetname, "")
                local cmdargs    = iif(isExe, table.concat(cfg.debugargs, " "), "") -- TODO: should this be debugargs instead?
                local useseparatedebugargs = "no"
                local debugargs  = ""
                local workingdir = pa iif(isExe, targetpath, "")
                local pauseexec  = iif(prj.kind == "ConsoleApp", "yes", "no")
                local isguiprogram = iif(prj.kind == "WindowedApp", "yes", "no")
                local isenabled  = iif(cfg.flags.ExcludeFromBuild, "no", "yes")
                
                _p(2,"buildDirectory:\"%s\"",objdir)
                _p(2,"destinationDirectory:\"%s\"", targetpath)
                --commands
                --cmdargs
                --workingdir
                --pauseexec
                --isguiprogram
                
            end
            
            function m.environment(cfg)
                
            end
            
            function m.debugger(cfg)
                
            end
            
            function m.preBuild(cfg)
                
            end
            
            function m.postBuild(cfg)
                
            end
            
            function m.customBuild(cfg)
                
            end
            
            function m.additionalRules(cfg)
                
                
            end
            
            function m.completion(cfg)
                
            end
            
            m.elements.settings = function(cfg)
            return {
                m.compiler,
                m.linker,
                --m.resourceCompiler,
                m.general,
                --m.environment,
                --m.debugger,
                --m.preBuild,
                --m.postBuild,
                --m.customBuild,
                --m.additionalRules,
                
            }
            end
            
            m.types =
            {
                ConsoleApp  = "CppApplication",
                Makefile    = "",
                SharedLib   = "DynamicLibrary",
                StaticLib   = "StaticLibrary",
                WindowedApp = "CppApplication"
            }
            
            function m.settings(prj)
                
                
                for cfg in project.eachconfig(prj) do
                    
                    local cfgname  = qbs.cfgname(cfg)
                    local compiler = m.getcompilername(cfg)
                    local type = m.types[cfg.kind]
                    
                    -- ms only debug and release support for now 
                    if (m.configs[cfgname]) then

                        _p(1,"Properties {")
                        _p(2,"cpp.allowUnresolvedSymbols:\"true\"")
          
                        if (type == "CppApplication") then
                            _p(2,"cpp.executableSuffix: \"%s\"", cfg.linktarget.extension)
                            _p(2,"cpp.executablePrefix: \"%s\"", cfg.linktarget.prefix)

                        elseif (type =="DynamicLibrary") then
                            _p(2,"cpp.dynamicLibraryPrefix: \"%s\"", cfg.linktarget.suffix)
                            _p(2,"cpp.dynamicLibrarySuffix: \"%s\"", cfg.linktarget.prefix)
                        elseif (type =="StaticLibrary") then
                            _p(2,"cpp.staticLibrarySuffix: \"%s\"", cfg.linktarget.suffix)
                            _p(2,"cpp.staticLibraryPrefix: \"%s\"", cfg.linktarget.prefix)
                        end      

                        _p(2,"targetName: \"%s\"",cfg.linktarget.basename)
                        
                        
                        qtConditions = {}
                        if (cfg.platform) then
                            table.insert(qtConditions, string.format("qbs.targetOS.contains(\"%s\")",m.platforms[cfg.platform]))
                        end
                                            
                        if (cfgname) then
                            table.insert(qtConditions,  string.format("qbs.buildVariant == \"%s\"",m.configs[cfgname]))
                        end
                        if (cfg.platform) then
                           table.insert(qtConditions, string.format("qbs.architecture == \"%s\"", m.architecture[cfg.platform]))
                        end
                        if (#qtConditions > 0) then
                            _p(2,"condition : %s", table.concat(qtConditions, " && "))
                        end
                        
                        _p(2,"Depends { name: \"cpp\" }")
                            p.callArray(m.elements.settings, cfg)
                        _p(1,"}")
                        
                    end    
                        
                    end
                    
                    
                end
                
                
                m.elements.project = function(prj)
                return {
                    --m.plugins,
                    m.description,
                    m.files,                            
                    m.settings,                            
                    m.dependencies,
                }
            end
            
            m.internalTypeMap = {
                ConsoleApp = "Console",
                WindowedApp = "Console",
                Makefile = "",
                SharedLib = "Library",
                StaticLib = "Library"
            }
            
            --
            -- Project: Generate the qbs project file.
            --
            function m.generate(prj)
                _p("import qbs 1.0")
                _p("%s {\n",m.types[prj.kind])               
                _p(1,"name: \"%s\"",prj.name)                   
                p.callArray(m.elements.project, prj)		
                _p(1,"Depends { name: \"cpp\" }")
                _p("}")
                end
                
                