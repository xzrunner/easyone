-- create node from file with cursor position

local filename = moon.gui.open_file_dlg("select scene node")
if filename then
	local node = moon.scene.new_node(filename)
	if node then
		node:set_pos(moon.mouse.get_position())
	end
end
