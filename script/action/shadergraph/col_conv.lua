-- create node ColorspaceConversion

require "moon.sg"

local node = moon.sg.new_node("sg_col_conv")
if node then
	node:set_pos(moon.mouse.get_position())
end