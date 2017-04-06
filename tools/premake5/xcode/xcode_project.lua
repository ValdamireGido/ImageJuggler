--
-- xcode_project.lua
-- Generate an Xcode C/C++ project.
-- Copyright (c) 2009 Jason Perkins and the Premake project
--
    local project = premake.project
    local config = premake.config
	local fileconfig = premake.fileconfig
	local xcode = premake.xcode
	local tree = premake.tree
	
--
-- Create a tree corresponding to what is shown in the Xcode project browser
-- pane, with nodes for files and folders, resources, frameworks, and products.
--
-- @param prj
--    The project being generated.
-- @returns
--    A tree, loaded with metadata, which mirrors Xcode's view of the project.
--

	function xcode.buildprjtree(prj)
			
		local tr = project.getsourcetree(prj, nil , false)

		tr.project = prj
		

		-- put project id in all children 
		tree.traverse(tr, {
			onnode = function(node)						
				node.prjid = prj.xcode.projectnode.id				
		 	end
		 }, true)
			
		local projectIdMap = {}	
		
		-- get files from extensions and insert them in this tree with different project iD 		
		for _, dep in ipairs(project.getdependencies(prj, "sibling", "object")) do
			-- create a child node for the dependency's xcodeproj
			if isExtension(dep.kind) then
				
				projectIdMap[dep.xcode.projectnode.id] = dep
				
				local depTree = project.getsourcetree(dep, nil , false)
			
				tree.traverse(depTree, {
					onnode = function(node)						
						node.prjid = dep.xcode.projectnode.id				
					end
				}, true)
				-- merge this tree with the original tree 
				tree.insert(tr, depTree)	
				end
		end
		
		--insert xcconfig files 
		local parent = tree.new() -- crete a new parent node to hold all the config files
	
		
		for cfg in project.eachconfig(prj) do
			local cfgname = xcode.getconfigname(cfg) -- obtain the current config name
			-- insert xcconfig files if any 
			-- we are creating a 'xcconfigs' group where all the config files will be added
			if cfg.generatexcconfigs == 'YES' and cfg.generatedconfigurationfiles then
				for k, v in ipairs(cfg.generatedconfigurationfiles) do
					if string.find(v, '.xcconfig') and string.find(v, string.lower(cfgname)) then
						local filePath = path.getrelative(prj.location, v)
						local fileName = path.getname(filePath)
						local child = tree.new(fileName)
						child.path = filePath
						tree.insert(parent, child)
						parent.path = path.getname(path.getdirectory(filePath)) -- this is added here and not after the for() loop, because of the local scope of filePath variable
					end	
				end
			-- this was added to keep backwards compatibility to libs that still use common.xcconfig
			elseif cfg.configurationfiles then
				for k, v in ipairs(cfg.configurationfiles) do
					if string.find(v, '.xcconfig') then
						local filePath = path.getrelative(prj.location, v)
						local fileName = path.getname(filePath)
						local child = tree.new(fileName)
						child.path = filePath--premake.project.getrelative(cfg.project, filename)
						tree.insert(tr, child)
					end	
				end
			end
		end
		
		if #parent.children > 0 then
			parent.name = 'xcconfigs'
			tree.insert(tr, parent) -- add the parent node to the tree
		end
		
		-- create a list of build configurations and assign IDs
		tr.configs = {}
		 
		for cfg in project.eachconfig(prj) do
			cfg.xcode = {}
			--cfg.xcode.targetid = xcode.newid(prj.xcode.projectnode.name, cfg.buildcfg)
			cfg.xcode.projectid = xcode.newid(tr.name, cfg.buildcfg)
			table.insert(tr.configs, cfg)					
		end
		
	
        -- convert localized resources from their filesystem layout (English.lproj/MainMenu.xib)
		-- to Xcode's display layout (MainMenu.xib/English).
		 tree.traverse(tr, {
		 	onbranch = function(node)
		 		if path.getextension(node.name) == ".lproj" then
		 			local lang = path.getbasename(node.name)  -- "English", "French", etc.
					
		 			-- create a new language group for each file it contains
		 			for _, filenode in ipairs(node.children) do
		 				local grpnode = node.parent.children[filenode.name]
		 				if not grpnode then
		 					grpnode = tree.insert(node.parent, tree.new(filenode.name))
		 					grpnode.kind = "vgroup"
		 					grpnode.prjid = filenode.prjid
		 				end
						
		 				-- convert the file node to a language node and add to the group
		 				filenode.name = path.getbasename(lang)
		 				tree.insert(grpnode, filenode)
		 			end
					
					-- remove this directory from the tree
		 			tree.remove(node)
		 		end
		 	end
		 })
		
        -- the special folder "Frameworks" lists all linked frameworks
		tr.frameworks = tree.new("Frameworks")
		for cfg in project.eachconfig(prj) do
			for _, link in ipairs(config.getlinks(cfg, "system", "fullpath")) do
				local name = path.getname(link)
				if xcode.isframework(name) and not tr.frameworks.children[name] then
					node = tree.insert(tr.frameworks, tree.new(name))
					node.path = link
					
					node.prjids = {}
					node.prjids[prj.xcode.projectnode.id] = {}			
		 		end
		 	end
		 end
		
		for lprj in premake.solution.eachproject(prj.solution) do
			if lprj.kind == premake.APPEX or lprj.kind == premake.WATCHEX then
				for cfg in project.eachconfig(lprj) do
					for _, link in ipairs(config.getlinks(cfg, "system", "fullpath")) do
						local name = path.getname(link)
						if xcode.isframework(name) then
							if not tr.frameworks.children[name] then
								node = tree.insert(tr.frameworks, tree.new(name))
								node.path = link													
								node.prjids = {}
							end	
							local node = tr.frameworks.children[name]
							node.prjids[lprj.xcode.projectnode.id] = {}
						end
					end	
		 		end
		 	end
		end	
			
		-- only add it to the tree if there are frameworks to link
		if #tr.frameworks.children > 0 then 
			tree.insert(tr, tr.frameworks)
		end
				
		-- the special folder "Projects" lists sibling project dependencies
		tr.projects = tree.new("Projects")
		 
		 -- first insert our project ? 
		 -- this is what xcode seems todo 
		tr.mainProject = tree.new("MainProjects")
		local xcpath = xcode.getxcodeprojname(prj)
		local xcnode = tree.insert(tr.mainProject, tree.new(path.getname(xcpath)))
		xcnode.id = xcode.newid(xcnode.name, nil, xcnode.path)
		xcnode.path = xcpath
		xcnode.prj = prj
		xcnode.productgroupid = xcode.newid(xcnode.name, "prodgrp")
		xcnode.productproxyid = xcode.newid(xcnode.name, "prodprox")
		xcnode.targetproxyid  = xcode.newid(xcnode.name, "targprox")
		xcnode.targetdependid = xcode.newid(xcnode.name, "targdep")
			
		-- insert new targets 	
		for _, dep in ipairs(project.getdependencies(prj, "sibling", "object")) do
			if (dep.kind == premake.APPEX) then
				-- create a grandchild node for the dependency's link target
				--[[local cfg = project.findClosestMatch(dep, dep.configurations[1])				
				local node = tree.insert(xcnode, tree.new(cfg.linktarget.name))
				node.id = "08FB7793FE84155DC02AAC07"
				node.path =  cfg.linktarget.fullpath 
				node.targetid = xcode.newid(path.getbasename(node.name), "target")
				node.targetproxyid = xcode.newid(path.getbasename(node.name), "targprox")
				node.prjids = {}
				node.prjids[prj.xcode.projectnode.id] = {}		
]]--				
			end	
		end
		
		local function insertDep(dep, prj)
			local xcpath = xcode.getxcodeprojname(dep)
			local cnode = tree.insert(tr.projects, tree.new(path.getname(xcpath)))
			
			cnode.id = xcode.newid(cnode.name, nil, cnode.path)
			cnode.path = path.getname(xcpath)
			cnode.prj = dep
			cnode.containerid = cnode.id
			if isExtension(dep.kind) then
				cnode.containerPrj = prj
			end
			cnode.productgroupid = xcode.newid(cnode.name, "prodgrp")
			cnode.productproxyid = xcode.newid(cnode.name, "prodprox")
			cnode.targetproxyid  = xcode.newid(cnode.name, "targprox")
			cnode.targetdependid = xcode.newid(cnode.name, "targdep")
			
			-- create a grandchild node for the dependency's link target
			local cfg = project.findClosestMatch(dep, dep.configurations[1])
			--"dep" .. path.getbasename(cnode.name)..".a")
			local node = tree.insert(cnode, tree.new(cfg.linktarget.name))
			node.path =  cfg.linktarget.relpath --"dep" .. cnode.name				
			node.prjids = {}
			node.prjids[prj.xcode.projectnode.id] = {}		
		end
		 -- insert dependecies
		for _, dep in ipairs(project.getdependencies(prj, "sibling", "object")) do
			-- create a child node for the dependency's xcodeproj
			insertDep(dep, prj)
			
			if isExtension(dep.kind) then
				-- this project is and extension 
				-- go trough all it's dependecies and insert projects 
				for _, lprj in ipairs(project.getdependencies(dep, "sibling", "object")) do
					insertDep(lprj, dep)
				end	
			else
			 
			end	
		end
			
		 if #tr.projects.children > 0 then
		 	tree.insert(tr, tr.projects)
		 end
		
		-- create a list of embedded binaries
		tr.binaries = tree.new("Embedded Binaries")
		for _, dep in ipairs(project.getdependencies(prj, "sibling", "object")) do
			-- create a child node for the dependency's xcodeproj
			-- need a configuration to get the target information
			if isExtension(dep.kind) then 
				local cfg = project.getconfig(dep, dep.configurations[1], dep.platforms[1])
			
				local nodename = cfg.targetname
				if cfg.kind == premake.APPEX or cfg.kind == premake.WATCHEX then
					nodename = nodename .. ".appex"
				end
				if cfg.kind == premake.WATCHAPP then
					nodename = nodename .. ".app"
				end			
				
				local binary = tree.new(nodename)
				binary.path = path.getrelative(prj.location, cfg.targetdir .. "/" .. nodename)
				binary.prj = dep
				binary.id = xcode.newid("emb", binary.name, "id")
				binary.targetproxyid  = xcode.newid("emb", binary.name, "targprox")
				binary.targetdependid = xcode.newid("emb", binary.name, "targdep")

				tree.insert(tr.binaries, binary)
			end	
		end	
		
		if #tr.binaries.children > 0 then 			
			tree.insert(tr, tr.binaries)
		end
		
		-- the special folder "Products" holds the target produced by the project; this
		-- is populated below	
		tr.products = tree.insert(tr, tree.new("Products"))
		
		local onnode = function(node)
			-- assign IDs to every node in the tree
			node.id = xcode.newid(node.name, node.prjid, node.path)
			
			local project = prj
			
			if node.prjid and node.prjid ~= prj.xcode.projectnode.id then
				--find other project 
				project = projectIdMap[node.prjid]
			end
								
			node.isResource = xcode.isItemResource(project, node)
			
			-- assign build IDs to buildable files
			if xcode.getbuildcategory(node) then		
				node.buildid = xcode.newid(node.name, "build", node.path)
				node.buildfilesettings = project.xcodebuildfilesettings[node.name]
				
			end						
		end
		local functor = {
			onnode = onnode,
		}
        -- Final setup
		tree.traverse(tr, functor, true)
			
        -- Plug in the product node into the Products folder in the tree. The node
		-- was built in xcode.preparesolution() in xcode_common.lua; it contains IDs
		-- that are necessary for inter-project dependencies
		local embedExtIds = {}
		for lprj in premake.solution.eachproject(prj.solution) do		
			if isExtension(lprj.kind) then
				
				-- need a configuration to get the target information
				local cfg = project.getconfig(lprj, lprj.configurations[1], lprj.platforms[1])
			
				local nodename = cfg.targetname
				if cfg.kind == premake.APPEX or cfg.kind == premake.WATCHEX then
					nodename = nodename .. ".appex"
				end
				if cfg.kind == premake.WATCHAPP then
					nodename = nodename .. ".app"
				end
				
				
				xnode = tree.insert(tr.products, tree.new(nodename))
				xnode.name = path.getbasename(xnode.name)
				
				-- just mark it so we know that this node has embeded stuff
				table.insert(embedExtIds, xcode.newid(xnode.name, "embext"))
								
				xnode.kind = "product"
				xnode.cfg = cfg		
				xnode.path = path.getrelative(prj.location, cfg.targetdir .. "/" .. nodename)
				
			--	xnode.id = xcode.newid(xnode.name, nil, xnode.path)
				
				xnode.prj = lprj
				xnode.prjid = lprj.xcode.projectnode.id
				xnode.id = xcode.newid(xnode.name, nil, xnode.path)
				xnode.targetid = premake.xcode.newid(xnode.name, "target")
				xnode.containerProxyId = premake.xcode.newid(xnode.name, "container")
				xnode.targetProxyId = premake.xcode.newid(xnode.name, "targetProxy")				
				
				--xnode.targetDependency
				
				xnode.cfgsection = xcode.newid(xnode.name, "cfg")
				xnode.resstageid = xcode.newid(xnode.name, "rez")
				xnode.sourcesid  = xcode.newid(xnode.name, "src")
				xnode.fxstageid  = xcode.newid(xnode.name, "fxs")
			
				--xnode.embfwkid  = xcode.newid(xnode.name, "embfwk")
				--xnode.embextid  = xcode.newid(xnode.name, "embext")
				
				tree.traverse(tr, {
					onnode = function(file)						
						if string.endswith(file.name, "Info.plist") then					
							if file.prjid == xnode.prjid then
								xnode.infoplist = file		
							end	
						end							
					end
				 }, true)

				xnode.configs = {}
				for cfg in project.eachconfig(lprj) do
					cfg.xcode = {}
					cfg.xcode.targetid = xcode.newid(lprj.xcode.projectnode.name, cfg.buildcfg)
					--cfg.xcode.projectid = xcode.newid(lprj.name, cfg.buildcfg)
					table.insert(xnode.configs, cfg)		
				end
			end	
		end
		
		
		local node = tree.insert(tr.products, prj.xcode.projectnode)
		node.kind = "product"		
		node.name = path.getbasename(prj.name)
		node.path = node.cfg.buildtarget.fullpath
		--node.id = xcode.newid(node.name, nil, node.path)
		
		node.prj = prj
		node.prjid = prj.xcode.projectnode.id
		node.cfgsection = xcode.newid(node.name, "cfg")
		node.resstageid = xcode.newid(node.name, "rez")
		node.sourcesid  = xcode.newid(node.name, "src")
		node.fxstageid  = xcode.newid(node.name, "fxs")
		
		node.embfwkid  = xcode.newid(node.name, "embfwk")
		if #embedExtIds > 0 then
			node.embextid  = xcode.newid(node.name, "embext")
		end	
		node.embedExtIds = embedExtIds
		-- create a list of build configurations and assign IDs
		node.configs = {}
		if prj.kind == "Framework" then
			node.headers = xcode.newid(node.name, "headers")
		end
		
		for cfg in project.eachconfig(prj) do
			--cfg.xcode = {}
			cfg.xcode.targetid = xcode.newid(prj.xcode.projectnode.name, cfg.buildcfg, "prj")
			--cfg.xcode.projectid = xcode.newid(tr.name, cfg.buildcfg, "prj")
			table.insert(node.configs, cfg)		
		end
		
		-- now we need to check if a product depends on another product ...
		-- for this we have xcodeTargetDependency
		for _, child in ipairs(tr.projects.children) do
			for _, otherChild in ipairs(tr.projects.children) do 
			end
		end
		tree.traverse(tr, {
			onnode = function(file)						
				if string.endswith(file.name, "Info.plist") then					
					if file.prjid == node.prjid then
						node.infoplist = file		
					end	
				end							
		 	end
		 }, true)

		-- remember key files that are needed elsewhere
		
		return tr		
	end

-- this functions adds the .xcconfig file names to 'cfg'
-- the .xcconfig files will be created at a later step !!!
function xcode.AddDefaultXCConfigs(prj)
	for cfg in project.eachconfig(prj) do
		-- if 'generatexcconfigs' is not defined, then it becomes 'NO', so we don't use .xcconfig files
		if cfg.generatexcconfigs == nil then
			cfg.generatexcconfigs = 'NO'
		end
	
		-- if 'generatexcconfigs' is defined to 'YES'
		if cfg.generatexcconfigs == 'YES' then
			local cfgname = string.lower(xcode.getconfigname(cfg))
			-- insert xconfig files if any
			local fpath = prj.location or prj.basedir
			fpath = path.join(fpath, 'xcconfigs_' .. cfg.project.name)
			local projectConfigFile = path.join(fpath, cfgname .. '-project.xcconfig')
			local targetConfigFile = path.join(fpath, cfgname .. '-target.xcconfig')
			cfg.generatedconfigurationfiles[1] = projectConfigFile
			cfg.generatedconfigurationfiles[2] = targetConfigFile
		end
	end
end		
-- //

--
-- Generate an Xcode .xcodeproj for a Premake project.
--
-- @param prj
--    The Premake project to generate.
--

	function premake.xcode.project(prj)	
		
		xcode.AddDefaultXCConfigs(prj)
		
		xcode.buildcategories = xcode.getbuildbuildcategories(prj.kind)
		
		local tr = xcode.buildprjtree(prj)
		xcode.Header(tr)
		xcode.PBXBuildFile(tr)
		xcode.PBXContainerItemProxy(tr)
		xcode.PBXCopyFilesBuildPhase(tr)
		xcode.PBXFileReference(tr)
		xcode.PBXFrameworksBuildPhase(tr)
		xcode.PBXGroup(tr)
		xcode.PBXHeadersBuildPhase(tr)
		xcode.PBXNativeTarget(tr)
		xcode.PBXProject(tr)
		xcode.PBXReferenceProxy(tr)
		xcode.PBXResourcesBuildPhase(tr)
		xcode.PBXShellScriptBuildPhase(tr)
		xcode.PBXSourcesBuildPhase(tr)
		xcode.PBXTargetDependency(tr)
		xcode.PBXVariantGroup(tr)			
		xcode.XCBuildConfiguration(tr)
		xcode.XCBuildConfigurationList(tr)
		xcode.Footer(tr)
	end
