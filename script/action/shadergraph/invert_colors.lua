-- create node InvertColors

require "moon.sg"

local node = moon.sg.new_node("sg_invert_colors")
if node then
	node:set_pos(moon.mouse.get_position())
end