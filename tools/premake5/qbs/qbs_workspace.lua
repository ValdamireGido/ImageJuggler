--
-- Name:        qbs/qbs_workspace.lua
-- Purpose:     Generate a qbs solution.
-- Author:      Lorenti luciano
-- Created:     2015/05/17
-- Copyright:   (c) 2008-2015 Jason Perkins and the Premake project

        local p = premake
        local project = p.project
        local solution = p.solution
        local tree = p.tree
        local qbs = p.modules.qbs

        qbs.solution = {}
        local m = qbs.solution
        
     function m.baseFileName(prj)
           
           return string.gsub(prj["group"],"/","_") .."_"
       end
---
-- Returns the full path a file generated from any of the project
-- objects (project, solution, rule).
--
-- @param obj
--    The project object being generated.
-- @param ext
--    An optional extension for the generated file, with the leading dot.
-- @param callback
--    The function responsible for writing the file; will receive the
--    project object as its only argument.
---

function m.filename(obj, ext)
	local fname = obj.location or obj.basedir
        
	if ext and not ext:startswith(".") then
		fname =path.join(fname, ext)
	else
		fname = path.join(fname,  obj.filename)
		if ext then
			fname = fname .. ext
		end
	end
	return path.getabsolute(fname)
end
  
        function m.generate(sln)
            _p('import qbs 1.0')
            _p("Project {")
            _p(1,"name: \"%s\"", sln.name)
            nivel = 1
             local tr = solution.grouptree(sln)
             tree.traverse(tr, {
                     onleaf = function(n)
                             local prj = n.project                                
                             local prjpath = m.filename(prj, ".qbs")
                             prjpath = path.translate(path.getrelative(prj.solution.location, prjpath))
                             nivel=nivel+1
                             _p(nivel,"SubProject {")
                             _p(nivel+1,"name:\"%s\"",prj.name)
                             _p(nivel+1,"filePath:\"%s\"",prjpath)
                             _p(nivel,"}")
                             nivel=nivel-1
                     end,        
                     
                     onbranchexit = function(n)
                           
                          _p(nivel,"}\n")
                          nivel=nivel-1
                      end,
                      
                     onbranchenter = function(n)
                            
                            local grp = n.name
                            local grppath = grp .. ".qbs"
                            _p(nivel,"Project {")
                            _p(nivel+1,"name:\"%s\"",n.name)
                                nivel=nivel+1
                            
                        end,
                })
                _p("}")
        end
