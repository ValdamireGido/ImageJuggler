--
-- Name:        qbs/qbs.lua
-- Purpose:     Define the qbs action(s).
-- Author:      Lorenti Luciano
-- Created:     2015/05/17
-- Copyright:   (c) 2008-2015 Jason Perkins and the Premake project
--

	local p = premake
	p.modules = {}	
	p.modules.qbs = {}

	local qbs = p.modules.qbs
	local project = p.project


	function qbs.cfgname(cfg)
		local cfgname = cfg.buildcfg
		if qbs.solution.multiplePlatforms then
			cfgname = string.format("%s|%s", cfg.platform, cfg.buildcfg)
		end
		return cfgname
	end

	function qbs.esc(value)
		return value
	end

	function qbs.generateSolution(sln)
		p.eol("\r\n")
		p.indent("  ")
		p.escaper(qbs.esc)

		p.generate(sln, sln.name ..  "_work.qbs", qbs.solution.generate)
	end

	function qbs.generateProject(prj)
		p.eol("\r\n")
		p.indent("  ")
		p.escaper(qbs.esc)     
                
		if project.iscpp(prj) then
			p.generate(prj, ".qbs", qbs.project.generate)
		end
	end

	function qbs.cleanSolution(sln)
		p.clean.file(sln, sln.name .. ".qbs")
		p.clean.file(sln, sln.name .. "_wsp.mk")
		p.clean.file(sln, sln.name .. ".tags")
		p.clean.file(sln, ".clang")
	end

	function qbs.cleanProject(prj)
		p.clean.file(prj, prj.name .. ".qbs")
		p.clean.file(prj, prj.name .. ".mk")
		p.clean.file(prj, prj.name .. ".list")
		p.clean.file(prj, prj.name .. ".out")
	end

	function qbs.cleanTarget(prj)
		-- TODO..
	end

	include("qbs_workspace.lua")
	include("qbs_project.lua")
	

	newaction
	{
		-- Metadata for the command line and help system

		trigger         = "qbs",
		shortname       = "qbs",
		description     = "Generate qbs project files",
		--module          = "qbs",

		-- The capabilities of this action

		valid_kinds     = { "ConsoleApp", "WindowedApp", "Makefile", "SharedLib", "StaticLib" },
		valid_languages = { "C", "C++" },
		valid_tools     = {
		    cc = { "gcc", "clang", "msc" }
		},

		-- Solution and project generation logic

		onsolution = function(sln)
			p.modules.qbs.generateSolution(sln)
		end,
		onproject = function(prj)
			p.modules.qbs.generateProject(prj)
		end,
                onBranch = function(prj)
			p.modules.qbs.generateProject(prj)
		end,
		oncleansolution = function(sln)
			p.modules.qbs.cleanSolution(sln)
		end,
		oncleanproject = function(prj)
			p.modules.qbs.cleanProject(prj)
		end,
		oncleantarget = function(prj)
			p.modules.qbs.cleanTarget(prj)
		end,
	}
